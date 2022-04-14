#if !defined(AFX_LIVEWINDOW_H__6B5BBEDC_9757_11D3_A870_004005A19028__INCLUDED_)
#define AFX_LIVEWINDOW_H__6B5BBEDC_9757_11D3_A870_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LiveWindow.h : header file
//
#include "DisplayWindow.h"

class CWaitEvent : public CEvent
{
public:
	BOOL Wait(DWORD dwTimeOut, LPCTSTR sFkt, CSecID id);
};
/////////////////////////////////////////////////////////////////////////////
// CLiveWindow window
			  
class CLiveWindow : public CDisplayWindow
{			 
	DECLARE_DYNAMIC(CLiveWindow)
// Construction
public:
	CLiveWindow(CMainFrame* pParent, CServer* pServer, const CIPCOutputRecord &or);
	virtual ~CLiveWindow();
// Attributes
public:
	virtual CSecID		GetID() const;
	virtual CString		GetName() const;
	virtual CString		GetTitle(CDC* pDC);
	virtual void		GetFooter(CByteArray& Array);
	virtual CString		GetDefaultText();
	virtual COLORREF	GetOSDColor();
	virtual BOOL		IsShowable();
			BOOL		IsPanelActive();
	virtual int			GetType() const { return DISPLAYWINDOW_LIVE;};

	int		GetMDAlarmSensitivity();
	int		GetMDMaskSensitivity();
	BOOL	GetSequencerDwellTime(int& iSequencerDwellTime);
	BOOL	IsMD();
	BOOL	IsPTZ();
	BOOL	IsJPEG();
	BOOL	IsMpeg4();

	inline BOOL IsLastAlarmTime() const;
	inline BOOL IsAlarmActive() const;
	inline const CSystemTime& GetLastAlarmTime() const;
	CString GetTimer(int iDay);

	int  GetAlarmCallEvent();
	int  GetAlarmCallEventByCardType();

	BOOL GetBrightness(int &nBrightness);
	BOOL GetContrast(int &nContrast);
	BOOL GetSaturation(int &nSaturation);
	BOOL GetCompression(int &nCompression);
	BOOL GetCameraFPS(int &nFPS);
	BOOL GetCameraTermination(BOOL &bCameraTermination);

	BOOL GetCameraPTZType(int &nPTZType);
	BOOL GetCameraPTZID(DWORD &dwCameraPTZID);
	BOOL GetCameraPTZComPort(int &nCameraPTZComPort);

	// Operations
public:
	virtual BOOL PictureData(const CIPCPictureRecord *pPict,DWORD dwX, DWORD dwY);
			void SetOutputRecord(const CIPCOutputRecord &or);
			void UpdateAlarmState();
	virtual void InitialRequests();
	virtual BOOL Request(int iMax=-1);
	virtual BOOL IndicateAlarmNr(BOOL bAlarmActive, WORD wX, WORD wY);
	virtual BOOL SetActiveWindowToAnalogOut();
	virtual void OnChangeVideo();

	BOOL SetBrightness(int iBrightness);
	BOOL SetContrast(int iContrast);
	BOOL SetSaturation(int iSaturation);
	BOOL SetCompression(int nCompression);
	BOOL SetCameraFPS(int nFPS);
	BOOL SetCameraTermination(BOOL bCameraTermination);
	BOOL SetAlarmCallEvent(int nAlarmCallEvent);

	BOOL SetCameraPTZType(int nPTZType);
	BOOL SetCameraPTZID(DWORD dwCameraPTZID);
	BOOL SetCameraPTZComPort(int nCameraPTZComPort);
	void SetLastCameraControlCmd(CameraControlCmd cccC, CameraControlCmd cccStop, DWORD dwValue);

	BOOL SetMDAlarmSensitivity(int nLevel);
	BOOL SetMDMaskSensitivity(int nLevel);
	BOOL SetSequencerDwellTime(int iSeconds);
	BOOL EnableUVV();
	BOOL EnableMD();
	BOOL SetTimer(int iDay, const CString& sTimer);
	BOOL SetName(const CString &sName);
	void SetShowable();
	BOOL SetHighPerf(BOOL bSet);

	BOOL ConfirmGetValue(const CString &sKey,const CString &sValue);
	BOOL ConfirmSetValue(const CString &sKey,const CString &sValue);

	static void ReleaseMMTimer();

		// Implementation
protected:
#ifdef _SWITCHABLE_LIVE_WINDOWS_
	virtual void OnDraw(CDC* pDC);
#endif // _SWITCHABLE_LIVE_WINDOWS_
	void RequestPTZ(CameraControlCmd ccc, DWORD dwValue, BOOL bOn);
	void RequestPTZParams();
	void GetNextPictureOfGOP();
	void ShowVideo();
private:
	void SetGOPTimer(UINT uElapse);
	void KillGOPTimer(BOOL bDeleteGOPs);
	static void CALLBACK GOPTimer(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);
	static UINT GOPThread(LPVOID lpParam);
	void	GOPThread();
	int		CalculateFrameStep(int nTimeStep);
	void	DrawNextGOPPicture();
	void	DrawNextPicture();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLiveWindow)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CLiveWindow)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
#ifdef _SWITCHABLE_LIVE_WINDOWS_
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
#endif //_SWITCHABLE_LIVE_WINDOWS_
	DECLARE_MESSAGE_MAP()

private:
	BOOL			 m_bAlarmActive;
	CSystemTime		 m_stLastAlarmTime;
	BOOL			 m_bLastAlarmTime;
	CIPCOutputRecord m_orOutput;

	// Image Values
	int		m_iBrightness;
	int		m_iContrast;
	int		m_iSaturation;
	int		m_nCompression;
	int		m_nCameraFPS;
	int		m_nCameraPTZType;
	DWORD	m_dwCameraPTZID;
	int		m_nCameraPTZComPort;
	BOOL	m_bCameraTermination;

	CWaitEvent	m_evConfirmSet;
	volatile BOOL	m_bSetEventUsed;
	CWaitEvent	m_evConfirmGet;
	volatile BOOL	m_bGetEventUsed;

	// MD values
	int		m_iMDMaskSensitivity;
	int		m_iMDAlarmSensitivity;

	// Mode
	CString m_sMode;

	// Timer
	CString m_sTimer[7];
	/*
	CEvent  m_evConfirmGetTimer[7];
	CEvent  m_evConfirmSetTimer[7];*/

	// Timer Active ?
	CString m_sIsTimerActive;

	// Sequencer dwell time
	int		m_iSequencerDwellTime;

	// Alarmcall events
	int m_iAlarmCallEvent;
	int m_iAlarmCallEventByCardType;

	// PTZ
	CameraControlType	m_ccType;
	DWORD				m_dwPTZFunctions;
	DWORD				m_dwPTZFunctionsEx;
	CameraControlCmd	m_LastCameraControlCmd;
	CameraControlCmd	m_LastCameraControlCmdStop;
	DWORD				m_dwPTZSpeed;

	CStatistics*		m_pStatistics;
	BOOL				m_bShowable;
	// For GOP representation
	CIPCPictureRecords	m_GOPs;
	CDWordArray			m_MDpoints;
	volatile UINT		m_uGOPTimer;
	CWK_Average<DWORD>	m_AvgStepMS;
	int					m_nCurrentTimeStep;
	int					m_nCurrentPicture;
	HANDLE				m_hGOPTimerEvent;
	CWinThread*			m_pGOPThread;
	int					m_nCurrentFPS;

	static UINT			gm_wTimerRes;
	static BOOL			gm_bTraceGOPTimer;
	static int			gm_nHighPerfWnds;
	BOOL				m_bHighPerfWnd;
#ifdef _SWITCHABLE_LIVE_WINDOWS_
	CSkinButton*		m_pBtnCancel;
	CRgn				m_rgnBtnCancel;
#endif // _SWITCHABLE_LIVE_WINDOWS_
};
////////////////////////////////////////////////////////////////
inline BOOL CLiveWindow::IsLastAlarmTime() const
{
	return m_bLastAlarmTime;
}
////////////////////////////////////////////////////////////////
inline const CSystemTime& CLiveWindow::GetLastAlarmTime() const
{
	return m_stLastAlarmTime;
}
////////////////////////////////////////////////////////////////
inline BOOL CLiveWindow::IsAlarmActive() const
{
	return m_bAlarmActive;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIVEWINDOW_H__6B5BBEDC_9757_11D3_A870_004005A19028__INCLUDED_)
