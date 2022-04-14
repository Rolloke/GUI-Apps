/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: WndMpeg4.h $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/WndMpeg4.h $
// CHECKIN:		$Date: 4.05.06 12:58 $
// VERSION:		$Revision: 48 $
// LAST CHANGE:	$Modtime: 4.05.06 12:08 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __MPEG4_WND_H_
#define __MPEG4_WND_H_

#define CRITICAL_FRAME_RATE	80

#include "WndLive.h"
/////////////////////////////////////////////////////////////////////////////
// CWndMpeg4 window
class CWndMpeg4 : public CWndLive
{
	DECLARE_DYNAMIC(CWndMpeg4)
// Construction
public:
	CWndMpeg4(CServer* pServer, const CIPCOutputRecord &or);
	virtual ~CWndMpeg4();

// Attributes
public:
	virtual	int  GetRequestedFPS();
	
// Operations
public:
	virtual	CSize GetPictureSize();
	virtual void Set1to1(BOOL b1to1);
	virtual BOOL PictureData(const CIPCPictureRecord &pict, 
							 CSecID idArchive,
							 DWORD dwX,
							 DWORD dwY);
	virtual CString GetTitleText(CDC* pDC);
	virtual void OnSetCmdActive(BOOL bActive, CWndSmall* pWndSmall);
	virtual void OnSetSmallBig(BOOL bActive, CWndSmall* pWndSmall);
	virtual BOOL CopyPictureForPrinting();
	static BOOL	CheckPerformanceLevel(int nPerformanceBias);
	static int	DetermineNetworkStatistics(WORD wServer, int &nCameras);
	static void CalculateFps(CServer *pS, int nCameras);
	static void SetLimitedFps(CServer *pServer);
	static void LimitFps(CServer *pServer, CWndMpeg4*pMP4, short&n25fps, short&n5fps, short&n1fps);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndMpeg4)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnDraw(CDC* pDC);
	virtual void DrawBitmap(CDC* pDC, const CRect &rcDest);
	void	GetNextPictureOfGOP();
	int		CalculateFrameStep(int nTimeStep);
	void	DrawNextGOPPicture();
	void	DrawNextPicture();

private:
	void SetGOPTimer(UINT uElapse);
	void KillGOPTimer(BOOL bDeleteGOPs);
	void GOPThread();
	static UINT GOPThread(LPVOID lpParam);
public:
	static void ReleaseMMTimer();
	static CString GetPerformanceLevelString();
	// Generated message map functions
protected:
	//{{AFX_MSG(CWndMico)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnChangeVideo(WPARAM, LPARAM);
	afx_msg LRESULT OnDecodeVideo(WPARAM, LPARAM);
	afx_msg LRESULT OnUser(WPARAM, LPARAM);
	afx_msg void OnFps_X_X(UINT);
	afx_msg void OnUpdateFps_X_X(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVideoSaveReference();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
protected:
	BOOL CanAdjustFPS(int fps);
	void ShowVideo();

	// data members
public:

	// data members
private:
	CStatistics*	m_pStatistics;
	int				m_nCurrentFPS;
	int				m_nCurrentRate;
	DWORD			m_dwLastMD_TC;
	bool			m_bMD;
	bool			m_bDropped;
	bool			m_bForceHighPerformance;
	bool			m_bNeedPrintImage;
	// For GOP representation
	int				m_nCurrentPicture;
	CIPCPictureRecords	m_GOPs;
	CDWordArray			m_MDpoints;
	volatile UINT	m_uGOPTimer;
	CWK_Average<int> m_AvgStepMS;
	int				m_nCurrentTimeStep;
	HANDLE			m_hGOPTimerEvent;
	CWinThread*		m_pGOPThread;


	static UINT		gm_wTimerRes;
	static const int gm_nQualities[MAX_PERFORMANCE_LEVEL+1][3];
public:
};
/////////////////////////////////////////////////////////////////////////////
#endif
