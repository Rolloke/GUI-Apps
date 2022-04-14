// AudioUnitDlg.h : header file
//

#if !defined(AFX_AUDIOUNITDLG_H__8F543AE6_A20F_44D1_B598_F9D947FB4C03__INCLUDED_)
#define AFX_AUDIOUNITDLG_H__8F543AE6_A20F_44D1_B598_F9D947FB4C03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Streams.h>
#include "resource.h"
#include "common\Keyprovider.h"
#include "ipcmediaaudiounit.h"

/////////////////////////////////////////////////////////////////////////////
// CAudioUnitDlg dialog

#define ICON_NOCOLOR		0
#define ICON_MAIN			1
#define ICON_RUNNING		2
#define ICON_RECORDING	3

#define WM_REQUESTSTARTMEDIAENCODING	(WM_USER + 2)
#define WM_REQUESTSTOPMEDIAENCODING		(WM_USER + 3)
#define WM_REQUESTHARDWARE				(WM_USER + 4)
   #define USE_CIPCMEDIA             1
   #define USE_CIPCCONTROL           2
   #define USE_CIPCINPUT             3
#define WM_REQUESTAUDIOQUALITIES		(WM_USER + 5)
#define WM_REQUESTSETSAMPLEFREQUENCY	(WM_USER + 6)
#define WM_REQUESTSTOPMEDIADECODING		(WM_USER + 8)
#define WM_DECODEMEDIADATA				(WM_USER + 9)
#define WM_SET_DLG_ITEM_INT				(WM_USER + 10)
#define WM_INPUTMIXER_VALUE				(WM_USER + 11)
#define WM_REPORT_ERROR					(WM_USER + 12)
#define WM_INPUTLIST_SETCHECK			(WM_USER + 13)

//#define WM_TRIGGER_UNIT (WM_USER + 2)


// predeclarations
class CIPCMediaAudioUnit;
class CIPCMediaSampleRecord;
class CIPCInputAudioUnit;

interface IWaveSplitter;
interface IPushSource;

//#define _TEST 1

struct ErrorStruct;

class CAudioUnitDlg : public CDialog
{
	friend class CIPCMediaAudioUnit;
	friend class CIPCInputAudioUnit;
// Construction
public:
	CAudioUnitDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CAudioUnitDlg();	// standard destructor

// Dialog Data
	//{{AFX_DATA(CAudioUnitDlg)
	enum { IDD = IDD_AUDIOUNIT_DIALOG };
	CSliderCtrl	m_cVolume;
	CProgressCtrl	m_cRecPeakLevel;
	CSliderCtrl	m_cRecordingLevel;
	CComboBox	m_cAudioDevices;
	CComboBox	m_cQuality;
	CListBox	m_ListFilters;
	CListCtrl	m_cInputPinList;
	CComboBox	m_cRenderer;
	CProgressCtrl	m_cRecLevel;
	CSliderCtrl	m_cThreshold;
	CComboBox	m_cCompressors;
	CComboBox	m_cCapture;
	BOOL		m_bStereo;
	CString	m_strFrq;
	BOOL		m_b2Bytes;
	int		m_nBytesPerSecond;
	int		m_nStereoChannel;
	int		m_nDwellTime;
	int      m_nThreshold;
	BOOL     m_bGenerateAlarm;
	int		m_nSequence;
	BOOL	m_bRecordingLevel;
	BOOL	m_bListen;
	int		m_nQuality;
	//}}AFX_DATA
	
#ifdef _TEST
	CSkinButton	m_btnInitAudioRec;
	CSkinButton	m_btnPause;
	CSkinButton	m_btnStart;
	CSkinButton	m_btnStop;
#else
	CButton	m_btnInitAudioRec;
	CButton	m_btnPause;
	CButton	m_btnStart;
	CButton	m_btnStop;
#endif


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioUnitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
public:
	void InitResetTimer();
	void IndicateLastMediaError();
	void UpdateAudioDetector(bool);
	void ActivateDetector(bool);
	void SetNewEncodingSegent();
	BOOL EnumRenderers(ICreateDevEnum*);
	BOOL SetComboBoxSelection(CComboBox&, CString&);
	void GetStateFlags(CIPCMediaRecord::MediaRecordFlags&);
	BOOL TestConfig(DWORD);
	void UpdateThreshold();

	void OnRequestValues(CIPCMediaAudioUnit*, CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData);
	void OnDecodeMediaData(const CIPCMediaSampleRecord&rec, DWORD dwFlags);
	void OnRequestSetValue(CSecID, const CString &, const CString &, DWORD);
	void OnRequestDisconnect(CIPCMediaAudioUnit*);
	void OnRequestReset();
	void DoConfirmMediaState(DWORD dwID=0);

// Implementation
protected:
	void FlushReceivedSamples();
	void AddGraphToRot(BOOL bAdd);
	void ReadRegistryKeys();
	void InitGraphBuilder();
	void SetButtonIcon(int, int);
	void ControlStartStopBtn(bool);

	void SetAUIcon(WORD wColor, BOOL bType=FALSE);
   void ReportError(ErrorStruct*, bool bMsgBox=true);
	void ReleaseFilters(bool bAll=true);
	void ReleasePlayFilters();
	int  PinsToList();
	void KillDwellTimer();
	HRESULT ReceiveMediaSample(IMediaSample*, long);
	BOOL InitDecodeMediaData();
	void SetOutputParam();

	HICON m_hIcon[3];

	// Generated message map functions
	//{{AFX_MSG(CAudioUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAbout();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCkStereo();
	afx_msg void OnChange();
	afx_msg void OnBtnEncoderProperty();
	afx_msg void OnInitAudioRecord();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnPause();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAppExit();
	afx_msg void OnSettings();
	afx_msg void OnApplyNow();
	afx_msg void OnCkGenerateAlarm();
	afx_msg void OnReleasedcaptureSliderThreshold(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEdtThreshold();
	afx_msg void OnCkRecordingLevel();
	afx_msg void OnClickListInputPins(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnInitAudioPlay();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDbgAddGraphToRot();
	afx_msg void OnDbgReleaseFilters();
	afx_msg void OnSelchangeComboQuality();
	afx_msg void OnSelchangeComboFrq();
	afx_msg void OnBtnRendererProperty();
	afx_msg void OnBtnSourceProperty();
	afx_msg void OnBtnAudioDeviceProperty();
	afx_msg void OnUpdateDbgAddGraphToRot(CCmdUI* pCmdUI);
	afx_msg void OnDbgRemoveGraphToRot();
	afx_msg void OnUpdateDbgRemoveGraphToRot(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDbgReleaseFilters(CCmdUI* pCmdUI);
	afx_msg void OnSettingsVolume();
	afx_msg void OnUpdateSettingsVolume(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnAudioDeviceProperty(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnRendererProperty(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnSourceProperty(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnEncoderProperty(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStop(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePause(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInitAudioRecord(CCmdUI* pCmdUI);
	afx_msg void OnReleasedcaptureSliderRecordingLevel(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdLeft();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCkListen();
	afx_msg void OnCkDeliverAlways();
	afx_msg void OnReleasedcaptureSliderVolume(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnGraphNotify(WPARAM, LPARAM);
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg long OnTraceTextA(WPARAM wParam, LPARAM lParam);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEnterMenuLoop(WPARAM, LPARAM);
	afx_msg LRESULT OnRequestStartMediaEncoding(WPARAM, LPARAM);
	afx_msg LRESULT OnRequestStopMediaEncoding(WPARAM dwFlags, LPARAM);
	afx_msg LRESULT OnRequestHardware(WPARAM, LPARAM);
	afx_msg LRESULT OnRequestAudioQualities(WPARAM, LPARAM);
	afx_msg LRESULT OnRequestSetSampleFrequency(WPARAM, LPARAM);
	afx_msg LRESULT OnRequestStopMediaDecoding(WPARAM dwFlags, LPARAM);
	afx_msg LRESULT OnDecodeMediaData(WPARAM, LPARAM);
	afx_msg LRESULT OnInputListSetCheck(WPARAM, LPARAM);
	afx_msg LRESULT OnDeviceChange(WPARAM, LPARAM);
	afx_msg LRESULT OnWinthreadStopThread(WPARAM, LPARAM);
	afx_msg LRESULT OnTriggerUnit(WPARAM, LPARAM);
	afx_msg LRESULT OnSetDlgItemInt(WPARAM, LPARAM);
	afx_msg LRESULT OnLanguageChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInputMixerValue(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReportError(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
//	void MakeSkin();
	BOOL				 m_bStandalone;
	// Filtergraph and control interfaces
	IGraphBuilder	*m_pGB;						// Capture Graph
	ICaptureGraphBuilder2 *m_pCaptureGB;		// Graph Builder	

	IMediaControl  *m_pMC;						// Media Control interface
	IMediaEventEx  *m_pME;						// Media Event interface
	// Recording and encoding filters and interfaces
	IBaseFilter		*m_pCapture;				// Capture Filter
	IBaseFilter		*m_pSplitter;				// splitter filter to split audio channels
	IWaveSplitter  *m_pSplitterInterface;		// interface to set threshold and callback function
	IBaseFilter		*m_pEncode;					// encoder filter
	IBaseFilter		*m_pDump;					// interface to captured media data
	// Decoding and rendering filters and interfaces
	IBaseFilter		*m_pMediaSampleSource;		// Source filter for mediasamples from CIPC
	IPushSource    *m_pIPushSource;				// interface to set media buffers
	IBaseFilter		*m_pRender;					// render filter
	AM_MEDIA_TYPE   m_MediaType;				// MediaType for the Source Filter
	ALLOCATOR_PROPERTIES m_AP;					// AllocatorProperties for the Source Filter
	CKeyProvider    m_Prov;						// Security Key for Windows Media Audio
	// CIPC
	CIPCMediaAudioUnit *m_pCIPCMedia;			// CIPC Audio interface
	CIPCMediaAudioUnit *m_pCIPCcontrol;			// CIPC Control interface for System Management
	CIPCInputAudioUnit *m_pCIPCInput;			// Detector
	// miscelaneous
	UINT			m_nDwellTimer;				// Timer for recording dwell time
	int				m_nDwellTimeCount;			// 
	UINT			m_nResetTimer;				// Timer for collecting resets
	int				m_nRecLevel;				// Recording level meter
	int				m_nAverageLevel;			// Recording level meter
	CWK_Average<int>m_AverageLevel;				// average recording level
	int             m_nLowerThreshold;
	int             m_nVolume;					// the Volume level from Registry
	int             m_nBalance;					// the Balance from Registry
	CString         m_sCapture;					// Audio Input Devices
	CString         m_sCompressor;				// Audio Compressor Devices
	CString         m_sRenderer;				// Audio Renderer Devices
	CString         m_sAudioDevice;				// AudioDevices
	int             m_nInputChannel;			// Input channel of the audio input device
	DWORD           m_dwRegisterROT;			// Graphedit Debugger
	CString			m_strKey;					// Registry Key
	BOOL            m_bDeliverAlways;			// delivers samples independently from threshold value
	bool            m_bNoCompression;			// disables compression
	bool            m_bCapturing;				// indicates Capturing
	bool            m_bRendering;				// indicates Rendering
	bool            m_bHeaderRequested;			// indicates that a MediaHeader has been requested
	bool            m_bLastSampleSyncPoint;		// the last sample was a syncpoint
	bool            m_bConfirmReceivedSamples;	// Confirms Received Samples
	bool            m_bMergeSamples;			// merge samples to decrease the no. of samples
	DWORD           m_dwOldMediaState;			// old state to compare and deliver only changes
	CIPCMediaSampleRecords m_MediaSamples;		// list of samples delivered to decoder
	CIPCMediaSampleRecordsEx m_ReceivedSamples;	// list of samples from the encoder
	int             m_nSequenceLength;			// minimum Length of a delivered sequence

	ErrorStruct    *m_pLastError;
	// for testing only
	CString         m_strOutputFileName;
	CFile           m_File;
	int             m_nNoOfSamples;				// no. of samples to render (should be 0 in the end)
	
#ifdef	_USE_AS_MEDIAPLAYER
	UINT			m_nPosUpdatetimer;			// Timer for updating the media position
#endif	// _USE_AS_MEDIAPLAYER
	static unsigned int WINAPI StopThread(void *);

	static HRESULT WINAPI ReceiveMediaSample(IMediaSample*, long, long);
	static HRESULT WINAPI CheckMediaSample(IMediaSample*, AM_MEDIA_TYPE*, long, long);
	static HRESULT WINAPI ReleaseBufferFunction(void*, long);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOUNITDLG_H__8F543AE6_A20F_44D1_B598_F9D947FB4C03__INCLUDED_)
