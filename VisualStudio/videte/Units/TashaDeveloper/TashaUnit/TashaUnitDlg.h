// TashaUnitDlg.h : header file
//

#if !defined(AFX_TASHAUNITDLG_H__27414A59_AE97_465A_AB8F_404C289508EE__INCLUDED_)
#define AFX_TASHAUNITDLG_H__27414A59_AE97_465A_AB8F_404C289508EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "resource.h"

#define WM_UPDATE_DIALOG (WM_USER + 1)

#define EPLD_FILENAME	"EPLD_DATA.rbf"
#define BOOT_IMAGE_533	"TashaFirmware533_BF%d.ldr"

/////////////////////////////////////////////////////////////////////////////
// CTashaUnitDlg dialog

class CVideoInlayWindow;
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

	BOOL OnConfirmSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue);

	BOOL OnConfirmGetMDTreshold(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmGetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmGetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmGetMaskDelay(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmChangePermanentMask(DWORD dwUserData, WORD wSource, int nX, int nY, int nValue);
	BOOL OnConfirmClearPermanentMask(DWORD dwUserData, WORD wSource);
	BOOL OnConfirmInvertPermanentMask(DWORD dwUserData, WORD wSource);


	BOOL OnConfirmResumeCapture(); 
	BOOL OnConfirmPauseCapture(); 
	BOOL OnConfirmStopCapture(); 
	BOOL OnConfirmStartCapture(); 

	BOOL OnConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask);

	BOOL OnConfirmSetChannel(WORD wSource, WORD wNewSource);

	BOOL OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose);
	BOOL OnConfirmGetRelayState(DWORD dwRelayStateMask);

	BOOL OnConfirmGetAlarmState(DWORD dwAlarmStateMask);

	BOOL OnConfirmSetAlarmEdge(DWORD dwAlarmEdge);
	BOOL OnConfirmGetAlarmEdge(DWORD dwAlarmEdge);

	BOOL OnReceiveNewCodecData(DATA_PACKET* pPacket);
	BOOL OnIndicationAlarm(DWORD dwAlarmMask);

	void OnReceivedCameraStatus(WORD wSource, BOOL bVideoState);

	BOOL OnNotifySlaveInitReady(WORD wSource);

	BOOL SaveJpegFile(DATA_PACKET* pPacket);

	void UpdateDialog();
	void PollProzess();

    CSemaVideoJobs& GetJobQueueEncode(WORD wSource) {return m_jobQueueEncode[wSource];}

	void ClearAllJobs();

	// Dialog Data
	//{{AFX_DATA(CTashaUnitDlg)
	enum { IDD = IDD_TASHAUNIT_DIALOG };
	CSliderCtrl	m_ctrlMDTreshold;
	CSliderCtrl	m_ctrlMaskTreshold;
	CSliderCtrl	m_ctrlMaskIncrement;
	CSliderCtrl	m_ctrlMaskDelay;
	CSliderCtrl	m_ctrlBrightness;
	CSliderCtrl	m_ctrlContrast;
	CSliderCtrl	m_ctrlSaturation;
	CString		m_sBrightness;
	CString		m_sContrast;
	CString		m_sSaturation;
	BOOL	m_bMultiView;
	BOOL	m_bFrameSync;
	BOOL	m_bNoiseReduction;
	BOOL	m_bShowAdaptiveMask;
	BOOL	m_bShowPermanentMask;
	BOOL	m_bShowDiffImage;
	CString	m_sMaskTreshold;
	CString	m_sMDTreshold;
	CString	m_sMaskDelay;
	CString	m_sMaskIncrement;
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
	afx_msg void OnRelais0();
	afx_msg void OnRelais1();
	afx_msg void OnRelais2();
	afx_msg void OnRelais3();
	afx_msg void OnCheckCam0();
	afx_msg void OnCheckCam1();
	afx_msg void OnCheckCam2();
	afx_msg void OnCheckCam3();
	afx_msg void OnCheckCam4();
	afx_msg void OnCheckCam5();
	afx_msg void OnCheckCam6();
	afx_msg void OnCheckCam7();
	afx_msg void OnPreview();
	afx_msg void OnCheckMultyView();
	afx_msg void OnDownloadEPLD();
	afx_msg void OnCheckFrameSync();
	afx_msg void OnCheckNoisereduction();
	afx_msg void OnCheckShowAdaptiveMask();
	afx_msg void OnDownloadEEEProm();
	afx_msg void OnCheckShowDiffImage();
	afx_msg void OnSaveEEProm();
	afx_msg void OnResetBF533();
	afx_msg void OnCheckShowPermanentMask();
	virtual void OnOK();
	afx_msg void OnClearPermanentMask();
	afx_msg void OnInvertPermanentMask();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg long OnUpdateDialog(WPARAM wParam, LPARAM lParam);

private:
	void ProzessManagement();
	void ClearAllEncodeJobs();
	void ClearCurrentEncodeJob(WORD wSource);
	void OnCheckCam(int nCamNr);
	BOOL SelectFile(CString &sFilename, const CString& sExt);
	
private:
	CString						m_sIniFile;

	CCodec*						m_pCodec;
	CIPCInputTashaUnit*			m_pInput;				// Pointer auf CIPC-Input 
	CIPCInputTashaUnitMDAlarm*	m_pInputMDAlarm;		// Pointer auf CIPC-Input
	CIPCOutputTashaUnit*		m_pOutputTashaCamera;	// Pointer auf CIPC-Output
	CIPCOutputTashaUnit*		m_pOutputTashaRelay;	// Pointer auf CIPC-Output

	CVideoInlayWindow* 			m_pPrewiewWnd;

	CWK_PerfMon* m_pPerf1;
	DWORD	m_dwEncodedFrames;
	CString	m_sJpegPath;
	DWORD	m_dwStartTime;

	// Processmanagement
	CWinThread*				m_pPollProzessThread;
	CVideoJob*				m_pCurrentEncodeJob[MAX_CAMERAS];	// Aktueller Encoderjob
	CSemaVideoJobs			m_jobQueueEncode[MAX_CAMERAS];		// Encoderjobqueue
	DWORD					m_dwFieldID[MAX_CAMERAS];
	CCriticalSection		m_csEncode;
	HANDLE					m_hSyncSemaphore;
	HANDLE					m_hShutDownEvent;
	DWORD					m_dwStandbyTime;

	int						m_nTimePerFrame;
	BOOL					m_bStandalone;
	BOOL					m_bSaveInLog;

	WORD					m_wSource;
	WORD					m_wRelayStateMask;

	CEvent					m_evRelayStateConfirm;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASHAUNITDLG_H__27414A59_AE97_465A_AB8F_404C289508EE__INCLUDED_)
