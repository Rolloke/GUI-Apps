/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: SavicUnitDlg.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/SavicUnitDlg.h $
// CHECKIN:		$Date: 20.06.03 13:15 $
// VERSION:		$Revision: 14 $
// LAST CHANGE:	$Modtime: 20.06.03 12:36 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __SAVICUNITDLG_H__
#define __SAVICUNITDLG_H__

#include "resource.h"
#include "PictureDef.h"
#include "videojob.h"
#include "jpeglib\CJpeg.h"

UINT PollProzessThread(LPVOID pData);
UINT PollEncoderThread(LPVOID pData);
UINT SwitchThread(LPVOID pData);

/////////////////////////////////////////////////////////////////////////////
// CSaVicUnitDlg dialog
class CCodec;
class CIPCInputSaVicUnit;
class CIPCInputSaVicUnitMDAlarm;
class CIPCOutputSaVicUnit;
class CSettings;
class CUser;
class CPermission;
class CVideoControl;
class CVideoInlayWindow;

/////////////////////////////////////////////////////////////////////////////

class CSaVicUnitDlg : public CDialog
{			  
	// Construction
public:
	CSaVicUnitDlg();	// standard constructor
	~CSaVicUnitDlg();

	// Attributes
public:
    CSemaVideoJobs& GetJobQueueEncode() {return m_jobQueueEncode;}
    CSemaVideoJobs& GetJobQueueDecode() {return m_jobQueueDecode;}

	// Operations
public:
	BOOL Create();
	BOOL InitializeCodec();

	void ClearAllJobs();
	void ClearAllEncodeJobs();
	void ClearCurrentEncodeJob();
	
	void RequestOutputReset();
	void RequestInputReset();

	void PollProzess();

	void OnReceivedImageData(const IMAGE* pImage);
	void OnReceivedEncoderError(int nErrorCode);
	void OnReceivedAlarm(WORD wCurrentAlarmState);
	void OnReceivedCameraStatus(WORD wSourceBOOL, BOOL bStatus);
	void OnReceivedMotionAlarm(WORD wSource, CMDPoints& Points);
	void OnPowerOffButtonPressed() const;
	void OnResetButtonPressed() const;
	void OnReceivedMotionDIB(WORD wSource, HANDLE hDIB);

	void SwitchThread();

	BOOL VideoTimeOutCheck(WORD wCamSubID);

	// Dialog Data
	//{{AFX_DATA(CSaVicUnitDlg)
	enum { IDD = IDD_SAVIC_APP_DIALOG };
	CString	m_sFPS;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaVicUnitDlg)
	public:
	virtual BOOL OnQueryOpen();
   	virtual void OnSysCommand(UINT nID, LPARAM lParam);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void Dump( CDumpContext &dc ) const;
	virtual void PostNcDestroy();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
public:
	// Generated message map functions
	//{{AFX_MSG(CSaVicUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDlgLuminance();
	afx_msg void OnFeNtscCCIR();
	afx_msg void OnFePalCCIR();
	afx_msg void OnSource0();
	afx_msg void OnSource1();
	afx_msg void OnSource2();
	afx_msg void OnSource3();
	afx_msg void OnFeLNotch();
	afx_msg void OnFeLDec();
	afx_msg void OnRelais0();
	afx_msg void OnEncStart();
	afx_msg void OnEncStop();
	afx_msg void OnTest();
	afx_msg void OnCompress1();
	afx_msg void OnCompress2();
	afx_msg void OnCompress3();
	afx_msg void OnCompress4();
	afx_msg void OnCompress5();
	afx_msg void OnHighResolution();
	afx_msg void OnLowResolution();
	afx_msg void OnDestroy();
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnVideoInlay();
	afx_msg void OnCompress0();
	afx_msg void OnDlgMdConfig();
	afx_msg void OnEnableDoubleFieldMode();
	afx_msg void OnCancelMode();
	afx_msg void OnLed1();
	afx_msg void OnLed2();
	afx_msg void OnChangeEditFps();
	afx_msg void OnInitPll();
	afx_msg void OnNoiseReduction();
	afx_msg void OnSelectCompressionTypeJpeg();
	afx_msg void OnSelectCompressionTypeDIB();
	//}}AFX_MSG

	afx_msg long OnWmRequestOutputReset(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmRequestInputReset(WPARAM wParam, LPARAM lParam);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg long DoOpenMDDlg(WPARAM wParam, LPARAM lParam);
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWmPowerBroadcast(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()

	// implementation
private:
	BOOL UpdateDlg();
	void VideoDataService();
	void ProzessManagement();
	BOOL Login();
	BOOL SetDlgText(UINT nID, LPCTSTR	szText);
	BOOL IsValid();
	void SaveImageToFile(const IMAGE* pImage);
	void SetSaVicIcon(WORD wColor, BOOL bType = FALSE);
	WORD GetNextSource(WORD wSource);
	void OnSource(int nSource, int nNextSource);

	
private:
	BOOL					m_bRun;
	BOOL					m_bOn;
	CWinThread*				m_pPollProzessThread;
	CWinThread*				m_pSwitchThread;
	CWinThread*				m_pPollEncoderThread;
			   
	HICON					m_hIcon1;
	HICON					m_hIcon2;
	HICON					m_hIcon3;

	CCodec*						m_pCodec;				// Pointer auf das Codec-Objekt
	CIPCInputSaVicUnit*			m_pInput;				// Pointer auf CIPC-Input 
	CIPCInputSaVicUnitMDAlarm*	m_pInputMDAlarm;		// Pointer auf CIPC-Input
	CIPCOutputSaVicUnit*		m_pOutputSaVicCamera;	// Pointer auf CIPC-Output
	CIPCOutputSaVicUnit*		m_pOutputSaVicRelay;	// Pointer auf CIPC-Output

	CVideoJob*				m_pCurrentEncodeJob;	// Aktueller Encoderjob
	CSemaVideoJobs			m_jobQueueEncode;		// Encoderjobqueue
	CSemaVideoJobs			m_jobQueueDecode;		// Decoderjobqueue
	DWORD					m_dwDecodedFrames;		// Anzahl der empfangenen JPEG'S
	DWORD					m_dwEncodedFrames;		// Anzahl der verschickten JPEG'S
	DWORD					m_dwSourceSwitch;		// Anzahl der Kameraumschaltungen
	DWORD					m_dwFrameNumber[MAX_CAMERAS];
	int						m_nEncoderState;		// ENCODER_ON, ENCODER_OFF

	BOOL					m_bOk;
	BOOL					m_bStandalone;
	WORD					m_wFormat;				// Auflösung
	DWORD					m_dwBPF;				// Bytes per Frame
	CSettings				*m_pSettings;
	CUser*					m_pUser;
	CPermission*			m_pPermission;

	BOOL					m_bSaveInLog;			// Jpegdaten nur ins Logfile sichern
	CString					m_sJpegPath;			// Pfad zum Speichern der Jpegs
	CString					m_sIniFile;				// Name der Ini-Datei	
	int						m_nTimePerFrame;		// Zeit zwischen 2 Bildern (20ms/40ms)

	// Videosignalüberwachung
	BOOL					m_bVideoPresentCheck;	// TRUE -> Videosignal wird überwacht
	CVideoControl*			m_pCVideoControl;		// Überwachung des Videosignals

	DWORD					m_dwStandbyTime;		// Standbyzeit
	CCriticalSection		m_csEncode;
	CCriticalSection		m_csDecode;
	
	HANDLE					m_hSyncSemaphore;
	HANDLE					m_hShutDownEvent;

	CVideoInlayWindow*		m_pVideoInlayWindow;
	friend UINT PollProzessThread(LPVOID pData);
	friend UINT PollEncoderThread(LPVOID pData);
	friend UINT SwitchThread(LPVOID pData);

	BOOL					m_bLED1State;
	BOOL					m_bLED2State;

	DWORD					m_dwLastJpegArrivedTC; // Zeitpunkt des zuletzt eingetroffenen Bildes.

	WORD					m_dwCamMask;

	CEvent					m_evImage;
	int						m_nSource;
	CJpeg					m_Jpeg;

	BOOL					m_bDoNoiseReduction;

	CompressionType			m_eCompressionType;

protected:
	void OnAbout();
};

#endif // __SAVICUNITDLG_H__
