/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: TashaUnitDlg.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/TashaUnitDlg.h $
// CHECKIN:		$Date: 11.02.05 9:04 $
// VERSION:		$Revision: 69 $
// LAST CHANGE:	$Modtime: 9.02.05 12:18 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASHAUNITDLG_H__27414A59_AE97_465A_AB8F_404C289508EE__INCLUDED_)
#define AFX_TASHAUNITDLG_H__27414A59_AE97_465A_AB8F_404C289508EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "resource.h"

#define WM_UPDATE_DIALOG						(WM_USER + 1)
#define WM_NOTIFY_SLAVE_INIT_READY				(WM_USER + 2)
#define	WM_ON_INDICATION_NO_BF533_RESPONSE		(WM_USER + 3)
#define	WM_ON_INDICATION_NO_BF535_RESPONSE		(WM_USER + 4)
#define WM_ON_INDICATION_VIDEOSTREAM_ABORTED	(WM_USER + 5)
#define WM_ON_INDICATION_VIDEOSTREAM_RESUMED	(WM_USER + 6)
#define	WM_ON_CONFIRM_OPEN_DEVICE				(WM_USER + 7)

/////////////////////////////////////////////////////////////////////////////
// CTashaUnitDlg dialog

class CVideoInlayWindow;
class CCodec;
class CIPCInputTashaUnit;
class CIPCInputTashaUnitMDAlarm;
class CIPCOutputTashaUnit;
class CAnalogVideoDlg;
class CEncoderParameterDlg;
class CProgressDlg;
class CTashaUnitDlg : public CDialog
{
// Construction			   
public:
	CTashaUnitDlg(CWnd* pParent = NULL);	// standard constructor
	~CTashaUnitDlg();

	BOOL Create();

	BOOL OnConfirmSetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam);
	BOOL OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue);

	BOOL OnConfirmGetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam);
	BOOL OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue);

	BOOL OnConfirmSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue);
	BOOL OnConfirmSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue);

	BOOL OnConfirmSetMDMaskSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel);
	BOOL OnConfirmGetMDMaskSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel);
	BOOL OnConfirmSetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel);
	BOOL OnConfirmGetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel);

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

	BOOL OnConfirmSetAnalogOut(DWORD dwCrosspointMask);
	BOOL OnConfirmGetAnalogOut(DWORD dwCrosspointMask);
	BOOL OnConfirmSetTerminationState(BYTE byTermMask);
	BOOL OnConfirmGetTerminationState(BYTE byTermMask);
	BOOL OnConfirmSetVideoEnableState(BOOL bState);
	BOOL OnConfirmGetVideoEnableState(BOOL bState);
	BOOL OnConfirmSetCrosspointEnableState(BOOL bState);
	BOOL OnConfirmGetCrosspointEnableState(BOOL bState);

	BOOL OnConfirmSetPowerLED(BOOL bState);
	BOOL OnConfirmSetResetLED(BOOL bState);

	BOOL OnConfirmSetNR(DWORD dwUserData, WORD wSource, BOOL bEnable);
	BOOL OnConfirmGetNR(DWORD dwUserData, WORD wSource, BOOL bEnable);

	BOOL OnReceiveNewCodecData(DATA_PACKET* pPacket);
	BOOL OnIndicationAlarmStateChanged(DWORD dwAlarmMask);
	BOOL OnIndicationAdapterSelectStateChanged(DWORD dwAdapterSelectMask);
	BOOL OnIndicationPCKeysStateChanged(DWORD dwPCKeysMask);
	BOOL OnIndicationCameraStatusChanged(WORD wSource, BOOL bVideoState);
	BOOL OnNotifySlaveInitReady(WORD wSource);

	BOOL SaveJpegFile(DATA_PACKET* pPacket);
	BOOL SaveMPEGStream(DATA_PACKET* pPacket);

	void UpdateDialog();

 
	void OnCheckCam(int nCamNr);
	WORD GetCurentCam(){return m_wSource;};

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
	int		m_nRegisterNumber;
	int		m_nRegisterValue;
	BOOL	m_bSaveToDisk;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTashaUnitDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIconEnabled;
	HICON m_hIconDisabled;
	HICON m_hIconActivity;
	HICON m_hIconFailure;

	// Generated message map functions
	//{{AFX_MSG(CTashaUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HCURSOR OnQueryDragIcon();

public:
	afx_msg void OnDestroy();
protected:
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
	afx_msg void OnPreview1();
	afx_msg void OnPreview2();
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
	virtual void PostNcDestroy();
	afx_msg void OnClearPermanentMask();
	afx_msg void OnInvertPermanentMask();
	afx_msg void OnCrosspointDlg();
	afx_msg void OnSetRegister();
	afx_msg void OnSetPowerLED();
	afx_msg void OnSetResetLED();
	afx_msg void OnCheckSaveToDisk();
	afx_msg void OnEncoderParamDlg();
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmPowerBroadcast(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmQueryEndSession(WPARAM wParam, LPARAM lParam);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg long OnUpdateDialog(WPARAM wParam, LPARAM lParam);
public:	
	afx_msg long OnIndicationVideoStreamAborted(WPARAM wParam, LPARAM lParam);
	afx_msg long OnIndicationVideoStreamResumed(WPARAM wParam, LPARAM lParam);
	afx_msg long OnIndicationNoBF533Response(WPARAM wParam, LPARAM lParam);
	afx_msg long OnIndicationNoBF535Response(WPARAM wParam, LPARAM lParam);
	afx_msg long OnConfirmOpenDevice(WPARAM wParam, LPARAM lParam);

private:
	void SetTashaIcon(WORD wColor, BOOL bType = FALSE);
	BOOL SelectFile(CString &sFilename, const CString& sExt);
	BOOL Login();

private:
	CString						m_sIniFile;

	CCodec*						m_pCodec;
	CIPCInputTashaUnit*			m_pInput;				// Pointer auf CIPC-Input 
	CIPCInputTashaUnitMDAlarm*	m_pInputMDAlarm;		// Pointer auf CIPC-Input
	CIPCOutputTashaUnit*		m_pOutputTashaCamera;	// Pointer auf CIPC-Output
	CIPCOutputTashaUnit*		m_pOutputTashaRelay;	// Pointer auf CIPC-Output

	CVideoInlayWindow* 			m_pPrewiewWnd1;
	CVideoInlayWindow* 			m_pPrewiewWnd2;

	DWORD	m_dwEncodedFrames;
	CString	m_sStreamPath;
	DWORD	m_dwStartTime;

	CCriticalSection		m_csMpegDecoder;

	BOOL					m_bStandalone;
	BOOL					m_bSaveInLog;
	BOOL					m_bPowerLED;
	BOOL					m_bResetLED;
	BOOL					m_bEnableNR;
	WORD					m_wSource;
	DWORD					m_dwRelayStateMask;
	DWORD					m_dwAlarmMask;
	DWORD					m_dwAdapterSelectMask;
	DWORD					m_dwPCKeysMask;

	CEvent					m_evRelayStateConfirm;

	CAnalogVideoDlg*		m_pAnalogDlg;
	CEncoderParameterDlg*	m_pEncoderParameterDlg;
	CProgressDlg*			m_pProgressDlg;

	int						m_nActiveRefreshRate;
	int						m_nIdleRefreshRate;
	BOOL					m_bShowOnlyIFrames;
	BOOL					m_bEnableBF533Recovery;
	BOOL					m_bEnableBF535Recovery;
	BOOL					m_bEnableStreamRecovery;

public:
	afx_msg void OnFileAbout();
	afx_msg void OnFileSettings();
	afx_msg void OnFileExit();
	afx_msg void OnBnClickedSetPermMask();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASHAUNITDLG_H__27414A59_AE97_465A_AB8F_404C289508EE__INCLUDED_)
