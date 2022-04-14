// TashaUnitDlg.h : header file
//

#if !defined(AFX_TASHAUNITDLG_H__27414A59_AE97_465A_AB8F_404C289508EE__INCLUDED_)
#define AFX_TASHAUNITDLG_H__27414A59_AE97_465A_AB8F_404C289508EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "resource.h"

#define WM_UPDATE_DIALOG (WM_USER + 1)

/////////////////////////////////////////////////////////////////////////////
// CTashaUnitDlg dialog

class CWK_PerfMon;
class CCodec;
class CIPCInputTashaUnit;
class CIPCInputTashaUnitMDAlarm;
class CIPCOutputTashaUnit;
class CTashaUnitDlg : public CDialog
{
// Construction			   
public:
	CTashaUnitDlg(CWnd* pParent = NULL);	// standard constructor
	~CTashaUnitDlg();

	BOOL OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue);

	BOOL OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue);

	BOOL OnConfirmResumeCapture(); 
	BOOL OnConfirmPauseCapture(); 
	BOOL OnConfirmStopCapture(); 
	BOOL OnConfirmStartCapture(); 

	BOOL OnConfirmSetInputSource(WORD wSource, DWORD dwProcessID);
	BOOL OnConfirmGetInputSource(WORD wSource, DWORD dwProcessID);

	BOOL OnConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask);

	BOOL OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose);
	BOOL OnConfirmGetRelayState(DWORD dwRelayStateMask);

	BOOL OnConfirmGetAlarmState(DWORD dwAlarmStateMask);

	BOOL OnConfirmSetAlarmEdge(DWORD dwAlarmEdge);
	BOOL OnConfirmGetAlarmEdge(DWORD dwAlarmEdge);

	BOOL OnReceiveNewCodecData(DataPacket* pPacket);
	BOOL SaveJpegFile(DataPacket* pPacket);

	void UpdateDialog();
	void PollProzess();

    CSemaVideoJobs& GetJobQueueEncode() {return m_jobQueueEncode;}

	void ClearAllJobs();

	// Dialog Data
	//{{AFX_DATA(CTashaUnitDlg)
	enum { IDD = IDD_TASHAUNIT_DIALOG };
	CScrollBar	m_ctrlBrightness;
	CScrollBar	m_ctrlContrast;
	CScrollBar	m_ctrlSaturation;
	CString		m_sBrightness;
	CString		m_sContrast;
	CString		m_sSaturation;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTashaUnitDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	BOOL					m_bRun;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTashaUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnResumeCapture();
	afx_msg void OnPauseCapture();
	afx_msg void OnStopCapture();
	afx_msg void OnStartCapture();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelectCamera1();
	afx_msg void OnSelectCamera2();
	afx_msg void OnSelectCamera3();
	afx_msg void OnSelectCamera4();
	afx_msg void OnRelais0();
	afx_msg void OnRelais1();
	afx_msg void OnRelais2();
	afx_msg void OnRelais3();
	afx_msg void OnTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg long OnUpdateDialog(WPARAM wParam, LPARAM lParam);

private:
	void ProzessManagement();
	void ClearAllEncodeJobs();
	void ClearCurrentEncodeJob();

private:
	CCodec*						m_pCodec;
	CIPCInputTashaUnit*			m_pInput;				// Pointer auf CIPC-Input 
	CIPCInputTashaUnitMDAlarm*	m_pInputMDAlarm;		// Pointer auf CIPC-Input
	CIPCOutputTashaUnit*		m_pOutputTashaCamera;	// Pointer auf CIPC-Output
	CIPCOutputTashaUnit*		m_pOutputTashaRelay;	// Pointer auf CIPC-Output

	CWK_PerfMon* m_pPerf1;
	DWORD	m_dwEncodedFrames;
	CString	m_sJpegPath;
	DWORD	m_dwStartTime;

	// Processmanagement
	CWinThread*				m_pPollProzessThread;
	CSemaVideoJobs			m_jobQueueEncode;		// Encoderjobqueue
	CCriticalSection		m_csEncode;
	HANDLE					m_hSyncSemaphore;
	HANDLE					m_hShutDownEvent;
	DWORD					m_dwStandbyTime;
	CVideoJob*				m_pCurrentEncodeJob;	// Aktueller Encoderjob

	int						m_nTimePerFrame;
	BOOL					m_bStandalone;

	WORD					m_wSource;
	WORD					m_wRelayStateMask;

	CEvent					m_evRelayStateConfirm;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASHAUNITDLG_H__27414A59_AE97_465A_AB8F_404C289508EE__INCLUDED_)
