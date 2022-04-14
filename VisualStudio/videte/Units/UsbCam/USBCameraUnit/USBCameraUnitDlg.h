// USBCameraUnitDlg.h : header file
//

#if !defined(AFX_USBCAMERAUNITDLG_H__EC54F91B_8F63_42F8_85E0_25E8636BAC56__INCLUDED_)
#define AFX_USBCAMERAUNITDLG_H__EC54F91B_8F63_42F8_85E0_25E8636BAC56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CUSBCameraUnitDlg dialog

//#define MAX_CAMERAS 16


#include <streams.h>
#include "resource.h"
#include "..\MotionDetection\CMDPoints.h"
#include "VideoJob.h"

#define ICON_NOCOLOR		0
#define ICON_MAIN			1
#define ICON_RUNNING		2
#define ICON_RECORDING	3

#define WM_REQUEST_CAMERA_CONTROL	(WM_USER + 2)
#define WM_DECODE_MEDIA_DATA		(WM_USER + 3)
#define WM_CONFIRM_PICTURE_RECORD	(WM_USER + 4)


enum CameraControlStructProp
{
	eCCSPInvalid = 0,
	eCCSPCameraProperty = 1,
	eCCSPCameraControlCmd = 2,
	eCCSPVideoProcAmpProperty = 3,
};

class CameraControlStruct
{
public:
	CameraControlStruct(CSecID ID, CString sKey, CString sValue, DWORD dwUserData, bool bSet, bool bAuto, CameraControlProperty CamCtrlProp)
	{
		m_ID = ID;
		m_sKey = sKey;
		m_sValue = sValue;
		m_dwUserData = dwUserData;
		m_Type = eCCSPCameraProperty;
		m_CamCtrl.CamCtrlProp = CamCtrlProp;
		m_bSet = bSet;
		m_bAuto = bAuto;
	}

	CameraControlStruct(CSecID ID, CString sKey, CString sValue, DWORD dwUserData, bool bSet, bool bAuto, VideoProcAmpProperty VidProcAmpProp)
	{
		m_ID = ID;
		m_sKey = sKey;
		m_sValue = sValue;
		m_dwUserData = dwUserData;
		m_Type = eCCSPVideoProcAmpProperty;
		m_CamCtrl.VideoProcAmpProp = VidProcAmpProp;
		m_bSet = bSet;
		m_bAuto = bAuto;
	}

	CameraControlStruct(CSecID ID, CString sKey, CString sValue, DWORD dwUserData, bool bSet, CameraControlCmd CamCtrlCmd)
	{
		m_ID = ID;
		m_sKey = sKey;
		m_sValue = sValue;
		m_dwUserData = dwUserData;
		m_Type = eCCSPCameraControlCmd;
		m_CamCtrl.CamCtrlCmd = CamCtrlCmd;
	}

	CSecID	m_ID;
	CString m_sKey, m_sValue;
	CameraControlStructProp m_Type;
	DWORD m_dwUserData;
	union tagCamCtrl
	{
		CameraControlProperty	CamCtrlProp;
		CameraControlCmd		CamCtrlCmd;
		VideoProcAmpProperty	VideoProcAmpProp;
	}m_CamCtrl;
	bool m_bSet, m_bAuto;
};

// predeclarations
class CIPCMediaVideo;
class CIPCMediaSampleRecord;
class CMotionDetection;
class CIPCOutputUSBcamUnit;
class	CIPCInputUSBcamUnit;
class	CIPCInputUSBcamUnitMDAlarm;
struct ErrorStruct;
interface IMediaSource;
interface IPushSource;




class CUSBCameraUnitDlg : public CDialog
{
	struct CameraStates;
	friend class CIPCMediaVideo;
	friend class CIPCOutputUSBcamUnit;
	friend class CIPCInputUSBcamUnit;
// Construction
public:
	CUSBCameraUnitDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CUSBCameraUnitDlg();	// standard destructor

// Dialog Data
	//{{AFX_DATA(CUSBCameraUnitDlg)
	enum { IDD = IDD_USBCAMERAUNIT_DIALOG };
	CComboBox	m_cTest;
	CComboBox	m_cOutputFormats2;
	CComboBox	m_cCameraNo;
	CComboBox	m_cTestFilters;
	CStatic	m_cFrame;
	CComboBox	m_cFrameRates;
	CComboBox	m_cEncoders;
	CButton	m_btnRelease;
	CButton	m_btnInitialize;
	CButton	m_btnStop;
	CButton	m_btnStart;
	CComboBox	m_cOutputFormats;
	CComboBox	m_cCameras;
	BOOL	m_bShowPicture;
	BOOL	m_bStillImage;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSBCameraUnitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

public:
	BOOL	m_bDTS;

// Implementation
protected:
	void InitMotionDetection();
	void InitCameraFilters(int);
	void InitBasicFilters();
	void InitVideoDisplay();
	void ReleaseFilters(int);
	void DeleteOutputFormats();

	void ReportError(ErrorStruct*, bool bMsgBox=true);
	void SetControlStates(CameraStates &sState);
	void SetButtonIcon(int nBtnID, int nIconID);
	void SetVUIcon(WORD wColor, BOOL bType);
	UINT SetTimer(UINT nEvent, UINT nMilliSeconds);
	UINT GetTimerByID(UINT nID);
	
#ifdef _DEBUG
	// Testfunktionen fuer Filtertester
	void DisplayPicture(VIDEOINFOHEADER*pVIH, void *pBuffer, int nLen);
	inline int CheckRange(int nValue, int nMin, int nMax)
	{
		if      (nValue  > nMax) return nMax;
		else if (nValue  < nMin) return nMin;
		return nValue;
	}
	typedef struct
	{
		BYTE bY1;
		BYTE bU;
		BYTE bY2;
		BYTE bV;
	}YUV422;

	afx_msg void OnInitVideoPlay();
	afx_msg LRESULT OnDecodeMediaData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConfirmPictureRecord(WPARAM wParam, LPARAM lParam);
	static HRESULT WINAPI TestMediaSample(IMediaSample *, long, long);
	static HRESULT WINAPI ReleaseBufferFunction(void*, long);
	// weitere Testfunktionen
	void InitCameraFilters2(int);
	HRESULT FindMatchingFilter2(DWORD, IPin*, BOOL, GUID, GUID, BOOL, GUID, GUID*, int, BOOL, IBaseFilter **, AM_MEDIA_TYPE **, LPCWSTR);
#endif
	
public:
	void SaveCamerasToRegistry();
	void ReadRegistry();
	void InitFunctions();
	void InitResetTimer();

	void    RemovePendigJobs();
	void    InitCameras();
	DWORD   GetActiveCameraMask();
	BOOL    IsCameraInitialised(WORD);
	BOOL    IsCameraPresent(WORD);
	BOOL    IsCameraActive(WORD);
	BOOL    IsShuttingDown() {return m_bIsShuttingDown;};
	void    SetCameraActive(WORD, bool);
	DWORD   ScanForCameras(DWORD);
	void    SetMotionDetection(WORD, bool);
	HRESULT CameraProperties(WORD, VideoProcAmpProperty, bool, bool&, DWORD&);
	HRESULT CameraControl(WORD, CameraControlProperty, bool, bool &, DWORD &);
	HRESULT CameraControlPTZ(WORD wSource, CameraControlCmd Cmd, DWORD*);

	CMotionDetection *GetMD() const {return m_pMD;};
	BOOL IsStarted()         const {return m_bStarted;};
	CCriticalSection*	GetCsVideoJob() {return &m_csVideoJobs;};
	CVideoJobList* GetVideoJobQueue(int nCam) 
	{
#ifdef TEST_WITH_SAVIC 
		if (nCam == -1)
		{
			return &m_SyncJob;
		}
#endif
		ASSERT(IsBetween(nCam, 0, MAX_CAMERAS-1));
		return &m_VideoJobList[nCam];
	}

protected:
	HRESULT FindMatchingFilter(DWORD, IPin*, BOOL, GUID, GUID, BOOL, GUID, GUID*, int, BOOL, IBaseFilter **, AM_MEDIA_TYPE **, LPCWSTR);

	HRESULT OnReceiveMediaSample(IMediaSample*, long, WORD);
	HRESULT OnReceiveStillImage(IMediaSample*, long, WORD);
	HRESULT OnCheckData(IMediaSample*, WORD);
	static HRESULT WINAPI ReceiveMediaSample(IMediaSample*, long, long);
	static HRESULT WINAPI ReceiveStillImage(IMediaSample*, long, long);
	static HRESULT WINAPI CheckData(IMediaSample*, long);

	static LRESULT CALLBACK VideoWindowFrameProc(HWND, UINT, WPARAM, LPARAM);

	HRESULT AddToRot(IUnknown *);

	// Generated message map functions
	//{{AFX_MSG(CUSBCameraUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnInit();
	afx_msg void OnDestroy();
	afx_msg void OnBtnStart();
	afx_msg void OnBtnStop();
	afx_msg void OnSelchangeVideoSource();
	afx_msg void OnBtnUnInit();
	afx_msg void OnCkShowPicture();
	afx_msg void OnReleaseAllFilters();
	afx_msg void OnRemoveFromRot();
	afx_msg void OnAddToRot();
	afx_msg void OnUpdateDbgAddToRot(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDbgRemoveRot(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReleaseAll(CCmdUI* pCmdUI);
	afx_msg void OnSettingsEncoder();
	afx_msg void OnUpdateSettingsCamera(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSettingsEncoder(CCmdUI* pCmdUI);
	afx_msg void OnSelchangeComboFormats();
	virtual void OnOK();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnApplyNow();
	afx_msg void OnSelchangeComboCameraNo();
	afx_msg void OnSelchangeComboFramerates();
	afx_msg void OnSettingsEncoderInfo();
	afx_msg void OnUpdateSettingsEncoderInfo(CCmdUI* pCmdUI);
	afx_msg void OnAppAbout();
	afx_msg void OnUpdateAppAbout(CCmdUI* pCmdUI);
	afx_msg void OnAppExit();
	afx_msg void OnSettingsDlg();
	afx_msg void OnCkMotionDetection();
	afx_msg void OnSettingsMd();
	afx_msg void OnUpdateSettingsMd(CCmdUI* pCmdUI);
	afx_msg void OnRequestReset();
	afx_msg void OnUpdateRequestReset(CCmdUI* pCmdUI);
	afx_msg void OnSettingsSourceFilter();
	afx_msg void OnUpdateSettingsSourceFilter(CCmdUI* pCmdUI);
	afx_msg void OnCkStillImage();
	afx_msg void OnSettingsCamera(UINT);
	afx_msg void OnUpdateBtnStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnStop(CCmdUI* pCmdUI);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGraphNotify(WPARAM, LPARAM);
	afx_msg LRESULT OnEnterMenuLoop(WPARAM, LPARAM);
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRequestCameraControl(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL                   m_bStandalone;
	HICON                  m_hIcon[3];
	// Filtergraph and control interfaces
	IGraphBuilder         *m_pGraph;
	ICaptureGraphBuilder2 *m_pGraphBuilder;
	IMediaControl         *m_pMediaControl;
	IMediaEventEx			 *m_pMediaEvent;
	IVideoWindow			 *m_pVideoWindow;
	// Recording and encoding filters and interfaces
	IBaseFilter		       *m_pCapture[MAX_CAMERAS];
	IBaseFilter		       *m_pEncoder[MAX_CAMERAS];
	// control variables for graph filters
	CString					m_sVideoInput[MAX_CAMERAS];
	int						m_nOutputFormat[MAX_CAMERAS];
	int						m_nOutputRate[MAX_CAMERAS];
	CMDPoints				m_mdPoints[MAX_CAMERAS];
	CString					m_sEncoder;
	struct CameraStates
	{
		DWORD nPTZCmd          :6; //  6
		DWORD nPTZSpeed        :4;	// 10
		DWORD nCapturePin      :4;	// 14
		DWORD nStillPin        :4;	// 18
		DWORD bInitialized     :1;	// 19
		DWORD bMotionDetection :1;	// 20
		DWORD bVideoPlay       :1;	// 21
		DWORD bActive          :1; // 22
		DWORD bStillImage      :1; // 23
		DWORD bDummy           :9;
	}						m_sCameraState[MAX_CAMERAS];;

	int						m_nRefPictRate;				// Default Picture Rate in ms
	BOOL					m_bStarted;					// Graph started (true, false)
	CString					m_strOutputFileName;		// Output file name to save the captured data
	int						m_nSelectedCam;				// The selected camera in the Combobox with capture devices
	int						m_nCamIndex;					// The selected Index of the Camera
	VIDEO_STREAM_CONFIG_CAPS **m_ppVSccOutputFormats;// stored outputformats
	DWORD					m_dwRegister;				// ROT register identifier
	CString					m_sKey;						// USBcam registry key
	CString					m_sDeviceKey;				// USBcam Device registry key
	BOOL					m_bIsShuttingDown;
	CSize					m_szResolution;

	CMap<UINT, UINT, DWORD, DWORD> m_TimerEvents;

	CMotionDetection*		m_pMD;						// Motion detection class
	CIPCInputUSBcamUnitMDAlarm*m_pInputMDAlarm;			// Pointer auf CIPC-Input MD
	CIPCInputUSBcamUnit*	m_pInput;					// Pointer auf CIPC-Input 
	CIPCOutputUSBcamUnit*	m_pOutput;
	CCriticalSection		m_csVideoJobs;				// 
	CVideoJobList			m_VideoJobList[MAX_CAMERAS];// Listen für Videojobs
	BOOL					m_bMotionDetection;
	BOOL					m_bCameraDetectors;

#ifdef TEST_WITH_SAVIC 
	HANDLE					  m_hSyncSemaphore;
	CVideoJobList          m_SyncJob;					// Liste für Synchronisation
#endif
	
	static CUSBCameraUnitDlg *gm_pThis;


#ifdef _DEBUG
	// Decoding and rendering filters and interfaces
	IBaseFilter		*m_pMediaSampleSource;	// Source filter for mediasamples from CIPC
	IPushSource    *m_pIMediaSampleSource; // interface to set media buffers
	AM_MEDIA_TYPE   m_MediaType;				// MediaType for the Source Filter
	ALLOCATOR_PROPERTIES m_AP;					// AllocatorProperties for the Source Filter
	CIPCMediaVideo        *m_pCIPCMedia;
	BOOL						  m_bColor;
#endif

public:
	afx_msg void OnBnClickedBtnServer();
	BOOL m_bServer;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USBCAMERAUNITDLG_H__EC54F91B_8F63_42F8_85E0_25E8636BAC56__INCLUDED_)
