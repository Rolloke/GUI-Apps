#ifndef _DISPLAYWINDOW_H
#define _DISPLAYWINDOW_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DisplayWindow.h : header file
//
#include "SmallWindow.h"
#include "CSBDialog.h"
#include "VideoSNDialog.h"
#include "SNConfigDialog.h"

#define WPARAM_CONTRAST		0x01
#define WPARAM_SATURATION	0x02
#define WPARAM_BRIGHTNESS	0x03
#define WPARAM_CCT			0x04
#define WPARAM_CCF			0x05
#define WPARAM_LCLICK		0x06
#define RANGE	15

class CMDConfigDlg;

/////////////////////////////////////////////////////////////////////////////
// CDisplayWindow window

class CDisplayWindow : public CSmallWindow
{
// Construction
public:
	CDisplayWindow(CServer* pServer, const CIPCOutputRecord &or);

// Attributes
public:
	virtual		 CSecID		GetID();
	inline		 CSecID		GetCamID();
	inline const CString	GetName() const;
	inline		 Resolution	GetResolution() const;
	inline		 int		GetQuality() const;
	inline		 BOOL		IsSn() const;
	inline		 BOOL		IsStoring() const;
	inline		 BOOL		WasSometimesActive() const;

	virtual	BOOL IsRequesting();
	virtual	BOOL IsWaitingForRequest();
	virtual	int	 IsWaitingForPicture();
	
	virtual	BOOL IsStreaming();
	virtual	int  GetRequestedFPS();

// Operations
public:
	// window creation
	BOOL Create(const RECT& rect, CVisionView* pParentWnd);

	void SetOutputRecord(const CIPCOutputRecord &or);

	// picture data
	virtual BOOL PictureData(const CIPCPictureRecord &pict);
			void RequestSetContrast(int contrast);
			void RequestSetBrightness(int brightness);
			void RequestSetColor(int color);
			void OnToggleResolution();
			void PopupMenu(CPoint pt);

			void ActivateCamera();
			void DeactivateCamera();

	virtual	void ClearWasSomeTimesActive();
			void SetStore(BOOL bOn);
	virtual	CSize GetPictureSize()=0;
	virtual void Set1to1(BOOL b1to1)=0;
	virtual void SetMDValues(WORD wX, WORD wY);
			void CheckVideo(DWORD dwCurrentTime);
	virtual	void OnIndicateError(DWORD dwCmd,CIPCError error,int iErrorCode);
			
			BOOL SetActiveRequested(BOOL bActive=TRUE);

			void SetMode(const CString& sMode);
			void ConfirmGetMask(const CIPCActivityMask& mask);
			void ConfirmSetMask(MaskType type);
	BOOL operator < (CDisplayWindow & second);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayWindow)
	//}}AFX_VIRTUAL

	// Overrides
public:
	virtual	BOOL IsDisplayWindow();
	virtual BOOL IsCoco();
	virtual BOOL IsMico();
	virtual BOOL IsVcs();
	virtual BOOL IsPresence();
	virtual void SetCIPCOutputWindow();
	virtual BOOL DoRequestPictures();
#ifndef _DTS
	virtual BOOL DoRequestVideoOut();
#endif

protected:
	virtual CString GetTitleText(CDC* pDC);

// Implementation
public:
	virtual ~CDisplayWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDisplayWindow)
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
	afx_msg void OnSmallContext();
	afx_msg void OnVideoCsb();
	afx_msg void OnUpdateVideoCsb(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnVideoSn();
	afx_msg void OnUpdateVideoSn(CCmdUI* pCmdUI);
	afx_msg void OnVideoSn1();
	afx_msg void OnVideoSn2();
	afx_msg void OnVideoSn3();
	afx_msg void OnVideoSn4();
	afx_msg void OnVideoSn5();
	afx_msg void OnVideoSn6();
	afx_msg void OnVideoSn7();
	afx_msg void OnVideoSn8();
	afx_msg void OnVideoSn9();
	afx_msg void OnUpdateVideoSn1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVideoSn2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVideoSn3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVideoSn4(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVideoSn5(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVideoSn6(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVideoSn7(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVideoSn8(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVideoSn9(CCmdUI* pCmdUI);
	afx_msg void OnVideoStop();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVideoSnConfig();
	afx_msg void OnUpdateVideoSnConfig(CCmdUI* pCmdUI);
	afx_msg void OnVideoSnHome();
	afx_msg void OnUpdateVideoSnHome(CCmdUI* pCmdUI);
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
	//}}AFX_MSG
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnterMenuLoop(BOOL);
	afx_msg void OnExitMenuLoop(BOOL);
	DECLARE_MESSAGE_MAP()

	// Implementation
protected:
	virtual void OnDraw(CDC* pDC);
	virtual void DrawBitmap(CDC* pDC, const CRect &rcDest);
	virtual void GetVideoClientRect(LPRECT lpRect); 
	void DrawCinema(CDC* pDC, LPRECT lpRectKeyColor = NULL);
	void DrawFace(CDC* pDC);
	void DrawCross(CDC* pDC, WORD wX, WORD wY);
	void StorePicture(const CIPCPictureRecord &pict);
	void OnCSBChanged();
	void RequestCSBUpdate();
	void InitialRequest();
	void RequestPTZ(CameraControlCmd ccc, DWORD dwValue, DWORD dwFlags, BOOL bOn = FALSE);
	void ZoomDisplayWindow(const CPoint &point);
	BOOL ScrollDisplayWindow();
	BOOL CanMultipleRequests();

	CString GetLocalReferenceFileName();
	CString GetRemoteReferenceFileName();
	
	// data member
protected:

	CIPCOutputRecord	m_orOutput;
	CameraControlType	m_ccType;
	DWORD				m_dwPTZFunctions;
	Resolution			m_Resolution;
	BOOL				m_bStoring;
	int					m_iX;
	int					m_iY;

	BOOL				m_bWasSomeTimesActive;
	BOOL				m_bIsRequesting;
	BOOL				m_bWaitingForRequest;
	int					m_iWaitingForPicture;
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
	BOOL	m_b1to1;
	CSecID  m_hostID;
	WORD	m_wMD_X;
	WORD	m_wMD_Y;
	BOOL	m_iCountZooms; //3 zooms alowed, then back to normal view
	CString m_sMode;
	DWORD	m_dwPTZSpeed;

	// child dialogs
	CCSBDialog*	m_pCSBDialog;
	CVideoSNDialog*		m_pVideoSNDialog;
	CSNConfigDialog*	m_pSNConfigDialog;
	CMDConfigDlg*		m_pMDConfigDlg;


	// buffers for software decoding
	CIPCPictureRecord*	m_pPictureRecord;
	CCriticalSection	m_csPictureRecord;
	BOOL				m_bTimeValid;

	CRect				m_rcZoom;
	CCriticalSection	m_csTracker;

private:
	CMenu*	m_pContextMenu; 	// save the menu for updating
	UINT	m_uTimerTest;
	UINT	m_uTimerInit;
	DWORD	m_dwPTZTemp;

	friend class CVisionView;
	friend class CCSBDialog;
	friend class CSNConfigDialog;
	friend class CVideoSNDialog;
	friend class CMDConfigDlg;
};
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDisplayWindow::IsDisplayWindow()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDisplayWindow::IsCoco()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDisplayWindow::IsMico()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDisplayWindow::IsVcs()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDisplayWindow::IsPresence()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CDisplayWindow::GetCamID()
{
	return m_orOutput.GetID();
}
/////////////////////////////////////////////////////////////////////////////
inline const CString CDisplayWindow::GetName() const
{
	return m_orOutput.GetName();
}
/////////////////////////////////////////////////////////////////////////////
inline Resolution CDisplayWindow::GetResolution() const 
{
	return m_Resolution;
}
/////////////////////////////////////////////////////////////////////////////
inline int CDisplayWindow::GetQuality() const
{
	return m_iQuality;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDisplayWindow::IsSn() const
{
	return !m_orOutput.CameraIsFixed();
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDisplayWindow::IsStoring() const
{
	return m_bStoring;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDisplayWindow::WasSometimesActive() const
{
	return m_bWasSomeTimesActive;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDisplayWindow::IsWaitingForRequest()
{
	return m_bWaitingForRequest;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDisplayWindow::IsRequesting()
{
	return m_bIsRequesting;
}
/////////////////////////////////////////////////////////////////////////////
inline int CDisplayWindow::IsWaitingForPicture()
{
	return m_iWaitingForPicture++;
}
////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
