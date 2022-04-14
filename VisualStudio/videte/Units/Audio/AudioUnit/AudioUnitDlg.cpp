/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AudioUnit
// FILE:		$Workfile: AudioUnitDlg.cpp $
// ARCHIVE:		$Archive: /Project/Units/Audio/AudioUnit/AudioUnitDlg.cpp $
// CHECKIN:		$Date: 14.08.06 16:53 $
// VERSION:		$Revision: 195 $
// LAST CHANGE:	$Modtime: 14.08.06 16:40 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AudioUnit.h"
#include "AudioUnitDlg.h"
#include "IPCMediaAudioUnit.h"
#include "CipcInputAudioUnit.h"
#include "oemgui\oemguiapi.h"

#pragma warning( push )
#pragma warning( disable : 4201 )
#include <KS.h>
#pragma warning( pop )

#include <KSMEDIA.h>
#include <atlbase.h>
#include "ksproxy.h"

#include "common\mfcutil.h"
#include "common\dshowutil.h"
#include "include\dmoreg.h"
#include "common\namedguid.h"

#include <initguid.h>    // DEFINE_GUID to declare an EXTERN_C const.
#include "filters\Dump\idump.h"
#include "filters\inftee\iinftee.h"
#include "filters\WaveSplitter\iwavesplitter.h"
#include "filters\PushSource\iPushsource.h"

#include <process.h>
#include <math.h>

#define FN_OF_CAPTURE_DEVICE		L"AC::Capture Device"
#define FN_OF_INFINITE_TEE			L"AC::Infinite Tee Filter"
#define FN_OF_WAVE_SPLITTER			L"AC::Wave Splitter"
#define FN_OF_ENCODER				L"AC::Encoder"
#define FN_OF_SAMPLE_DUMP			L"AC::Sample Dump"
#define FN_OF_CAPTURERS  			L"AC::"

#define FN_OF_AUDIO_RENDERER		L"AR::Audio Renderer"

//#include <strmif.h>
//#include <uuids.h>
//#include "dshow.h"

#include <Dsound.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CAudioUnitApp theApp;

#define PEAK_DETECTION 1
#ifndef PEAK_DETECTION
#include <math.h>
#endif

#define EVENT_REC_LEVEL    100
#define TIMER_HOLDTIME     200
#define TIMER_RESET        202
#define TIMER_UPDATE	   203

#ifdef _DEBUG
// #define DEBUG_WITOUT_COMPRESSORS 1
 #define TRACE_NAME_OF(a) case a: TRACE(_T("%s\n"), _T(#a)); break;
#else
  #define TRACE_NAME_OF //
#endif

#ifdef _DEBUG
 #define TEST_TRACE TRACE
#else 
 #define  TEST_TRACE // 
// #define  TEST_TRACE WK_TRACE 
#endif


struct ErrorStruct
{
	ErrorStruct(HRESULT hr, int nLine, const char*sFile) 
	{
		m_hr = hr;
		m_nLine = nLine;
		m_sFile = sFile;
	}
	int         m_nLine;
	HRESULT     m_hr;
	const char* m_sFile;
};

#define HRESULT_ERROR_AT_POS(hr) new ErrorStruct(hr, __LINE__, __FILE__)
#define HR_EXCEPTION(HR) if (FAILED(hr=HR))	throw HRESULT_ERROR_AT_POS(hr);
#define HR_REPORT(HR) if (FAILED(hr=HR))	ReportError(HRESULT_ERROR_AT_POS(hr), false);
#define HR_MSG_BOX(HR) if (FAILED(hr=HR))	ReportError(HRESULT_ERROR_AT_POS(hr));


#define UPDATE_DLG_CTRL_VALUE 0x80000000

#ifdef _DEBUG
//  #define  TEST_PERFORMANCE_BUFFER_RELEASE
#endif

#ifdef TEST_PERFORMANCE_BUFFER_RELEASE
class CDebugPM
{
public:
	CDebugPM(LPCTSTR sName, DWORD dwBuffer) 
		: m_Perfmon(sName)
	{
		m_dwBuffer = dwBuffer;
		m_Perfmon.Start();
	}

	CWK_PerfMon m_Perfmon;
	DWORD m_dwBuffer;
};
CPtrList perfmon;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAudioUnitDlg dialog
CAudioUnitDlg::CAudioUnitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAudioUnitDlg::IDD, pParent)
{
	m_strKey.Format(AUDIO_UNIT_DEVICE, theApp.GetInstanceCount());
	m_strKey = WK_PROFILE_ROOT + m_strKey;
	
	//{{AFX_DATA_INIT(CAudioUnitDlg)
	m_nBytesPerSecond = -1;
	m_nQuality = -1;
	//}}AFX_DATA_INIT

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon[0] = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon[1] = AfxGetApp()->LoadIcon(IDR_RUNNING);
	m_hIcon[2] = AfxGetApp()->LoadIcon(IDR_RECORDING);

	m_pGB                 = NULL;
	m_pCaptureGB          = NULL;
	m_pCapture            = NULL;
	m_pSplitter           = NULL;
	m_pEncode             = NULL;
	m_pDump               = NULL;
	m_pRender             = NULL;
	m_pMediaSampleSource  = NULL;
	m_pIPushSource        = NULL;
	m_pMC                 = NULL;
	m_pME                 = NULL;
	m_pSplitterInterface  = NULL;


	m_nDwellTimer         = 0;
	m_nDwellTimeCount     = 0;
	m_nResetTimer         = 0;
#ifdef	_USE_AS_MEDIAPLAYER
	m_nPosUpdatetimer	  = 0;
#endif // _USE_AS_MEDIAPLAYER
	m_dwRegisterROT       = 0;
	m_nNoOfSamples        = 0;
	m_dwOldMediaState     = 0;
	m_nSequenceLength     = 250;
	
	m_pCIPCMedia          = NULL;
	m_pCIPCcontrol        = NULL;
	m_pCIPCInput          = NULL;

	m_pLastError          = NULL;

	m_bNoCompression          = false;
	m_bDeliverAlways          = FALSE;
	m_bCapturing              = false;
	m_bRendering              = false;
	m_bHeaderRequested        = false;
	m_bLastSampleSyncPoint    = false;
	m_bConfirmReceivedSamples = false;
	m_bMergeSamples           = false;

	ReadRegistryKeys();
	ZeroMemory(&m_MediaType, sizeof(AM_MEDIA_TYPE));

	m_Prov.AddRef();
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::ReadRegistryKeys()
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_strKey, _T(""));

	m_bStereo           = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_STEREO       , AU_DEFAULT_STEREO);
	m_strFrq            = wkp.GetString(AU_AUDIO_SETTINGS, AU_SAMPLE_FRQ   , AU_DEFAULT_SAMPLE_FRQ);
	m_b2Bytes           = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_TWO_BYTES    , AU_DEFAULT_TWO_BYTES);
	m_nStereoChannel    = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_CHANNEL      , AU_DEFAULT_CHANNEL);
	m_nDwellTime        = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_HOLD_TIME    , AU_DEFAULT_DWELLTIME);
	m_nThreshold        = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_THRESHOLD    , AU_DEFAULT_THRESHOLD);
	m_nSequence         = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_SAMPLE_LENGTH, AU_DEFAULT_SEQUENCE);
	m_nInputChannel     = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_INPUT_CHANNEL, CB_ERR);
	m_nQuality          = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_AUDIO_QUALITY, CB_ERR);
	m_nVolume           = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_OUTPUT_VOLUME, 1000);
	m_nBalance          = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_OUTPUT_BALANCE, 0);

	m_bGenerateAlarm    = wkp.GetInt(AU_CIPC_SETTINGS, AU_GENERATE_ALARM, FALSE);

	m_bRecordingLevel   = wkp.GetInt(   AU_DEBUG_SETTINGS, AU_RECORDING_LEVEL, FALSE);
	m_strOutputFileName = wkp.GetString(AU_DEBUG_SETTINGS, AU_OUT_FILE_NAME  , NULL);

	m_sCapture		= wkp.GetString(AU_DIRECT_SHOW, GetString(CLSID_AudioInputDeviceCategory), NULL);
	m_sCompressor	= wkp.GetString(AU_DIRECT_SHOW, GetString(CLSID_AudioCompressorCategory), AU_DEFAULT_CODEC);
	m_sRenderer		= wkp.GetString(AU_DIRECT_SHOW, GetString(CLSID_AudioRendererCategory), AU_DEFAULT_OUTPUT);
	m_sAudioDevice	= wkp.GetString(AU_DIRECT_SHOW, GetString(KSCATEGORY_AUDIO_DEVICE), NULL);

	WK_TRACE(_T("---- Debug Settings ---------------------------\n"));
	if (theApp.IsReadOnly())
	{
		m_bStandalone = FALSE;
	}
	else
	{
		theApp.m_bAudioDebug = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("AudioDebug"), 0);
		m_bStandalone = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, AU_STAND_ALONE, 0);
		theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, EXTENDEDMENU, 0);
		theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, AUDIOOFFSETFORREPLAY, DEFAULT_OFFSET_REPLAY);
		m_bMergeSamples = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, MERGE_SAMPLES, 1)? true : false;
	}
#ifdef _DEBUG
	m_bStandalone = TRUE; 
#endif
	if (!theApp.m_sParam.IsEmpty())
	{
		m_bStandalone = 2; 
	}
	theApp.m_bTraceStateFlags                  = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("TraceStateFlags"));
	theApp.m_bTraceOnRequestStartMediaEncoding = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("TraceOnRequestStartMediaEncoding"));
	theApp.m_bTraceOnRequestStopMediaEncoding  = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("TraceOnRequestStopMediaEncoding"));
	theApp.m_bTraceOnIndicateMediaData         = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("TraceOnIndicateMediaData"));
	theApp.m_bTraceOnIndicateMediaData         = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("TraceOnIndicateMediaData"));
	theApp.m_bTraceOnRequestMediaDecoding      = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("TraceOnRequestMediaDecoding"));
	theApp.m_bTraceOnRequestStopMediaDecoding  = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("TraceOnRequestStopMediaDecoding"));
	theApp.m_bTraceOnRequestSetValue           = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("TraceOnRequestSetValue"));
	theApp.m_bTraceOnRequestValues             = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("TraceOnRequestValues"));
	theApp.m_bTraceFilterMessages              = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("TraceFilterMesages"));
	theApp.m_bDoIndicateMediaData              = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("DoIndicateMediaData"));
	theApp.m_bFreePushSourceBuffers            = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("FreePushSourceBuffers"), 1);
	WK_TRACE(_T("-----------------------------------------------\n"));

	ActivateDetector(m_bGenerateAlarm ? true : false);
}
/////////////////////////////////////////////////////////////////////////////
CAudioUnitDlg::~CAudioUnitDlg()
{
	ActivateDetector(false);
	WK_DELETE(m_pLastError);
	if (m_MediaSamples.GetCount())
	{
		WK_TRACE_ERROR(_T("Deleting posted and unhandled MediaSamples from list\n"));
	}
	m_MediaSamples.SafeDeleteAll();
	
	if (m_ReceivedSamples.GetCount())
	{
		WK_TRACE_ERROR(_T("Deleting received and unhandled MediaSamples from list\n"));
	}
	m_ReceivedSamples.SafeDeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioUnitDlg)
	DDX_Control(pDX, IDC_SLIDER_VOLUME, m_cVolume);
	DDX_Control(pDX, IDC_PRGR_REC_LEVEL_PEAK, m_cRecPeakLevel);
	DDX_Control(pDX, IDC_SLIDER_RECORDING_LEVEL, m_cRecordingLevel);
	DDX_Control(pDX, IDC_COMBO_AUDIO_DEVICE, m_cAudioDevices);
	DDX_Control(pDX, IDC_COMBO_QUALITY, m_cQuality);
	DDX_Control(pDX, IDC_LIST_FILTERS, m_ListFilters);
	DDX_Control(pDX, IDC_LIST_INPUT_PINS, m_cInputPinList);
	DDX_Control(pDX, IDC_COMBO3, m_cRenderer);
	DDX_Control(pDX, IDC_PRGR_REC_LEVEL, m_cRecLevel);
	DDX_Control(pDX, IDC_SLIDER_THRESHOLD, m_cThreshold);
	DDX_Control(pDX, IDC_COMBO2, m_cCompressors);
	DDX_Control(pDX, IDC_COMBO1, m_cCapture);
	DDX_Check(pDX, IDC_CK_STEREO, m_bStereo);
	DDX_CBString(pDX, IDC_COMBO_FRQ, m_strFrq);
	DDX_Check(pDX, IDC_CK_2BYTES, m_b2Bytes);
	DDX_Radio(pDX, IDC_RD_LEFT, m_nStereoChannel);
	DDX_Text(pDX, IDC_EDT_HOLD_TIME, m_nDwellTime);
	DDV_MinMaxInt(pDX, m_nDwellTime, 1, 60);
	DDX_Text(pDX, IDC_EDT_THRESHOLD, m_nThreshold);
	DDV_MinMaxInt(pDX, m_nThreshold, 0, 100);
	DDX_Check(pDX, IDC_CK_GENERATE_ALARM, m_bGenerateAlarm);
	DDX_CBIndex(pDX, IDC_COMBO_SEQUENCE, m_nSequence);
	DDX_Check(pDX, IDC_CK_RECORDING_LEVEL, m_bRecordingLevel);
	DDX_Check(pDX, IDC_CK_LISTEN, m_bListen);
	DDX_Check(pDX, IDC_CK_DELIVER_ALWAYS, m_bDeliverAlways);
	DDX_Control(pDX, IDC_INIT_AUDIO_RECORD, m_btnInitAudioRec);
	DDX_Control(pDX, IDC_PAUSE, m_btnPause);
	DDX_Control(pDX, IDC_START, m_btnStart);
	DDX_Control(pDX, IDC_STOP, m_btnStop);
	//}}AFX_DATA_MAP

	if (m_nBytesPerSecond == -1)
	{
		DDX_Text(pDX, IDC_EDT_BYTES_PER_SECOND, _T("--"), 3);
	}
	else
	{
		DDX_Text(pDX, IDC_EDT_BYTES_PER_SECOND, m_nBytesPerSecond);
	}
}

BEGIN_MESSAGE_MAP(CAudioUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CAudioUnitDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CK_STEREO, OnCkStereo)
	ON_BN_CLICKED(IDC_CK_2BYTES, OnChange)
	ON_COMMAND(IDC_BTN_ENCODER_PROPERTY, OnBtnEncoderProperty)
	ON_BN_CLICKED(IDC_INIT_AUDIO_RECORD, OnInitAudioRecord)
	ON_COMMAND(IDC_START, OnStart)
	ON_COMMAND(IDC_STOP, OnStop)
	ON_COMMAND(IDC_PAUSE, OnPause)
	ON_WM_TIMER()
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_SETTINGS, OnSettings)
	ON_BN_CLICKED(ID_APPLY_NOW, OnApplyNow)
	ON_BN_CLICKED(IDC_CK_GENERATE_ALARM, OnCkGenerateAlarm)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_THRESHOLD, OnReleasedcaptureSliderThreshold)
	ON_EN_KILLFOCUS(IDC_EDT_THRESHOLD, OnKillfocusEdtThreshold)
	ON_BN_CLICKED(IDC_CK_RECORDING_LEVEL, OnCkRecordingLevel)
	ON_NOTIFY(NM_CLICK, IDC_LIST_INPUT_PINS, OnClickListInputPins)
	ON_BN_CLICKED(IDC_PLAYING, OnInitAudioPlay)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_DBG_ADD_GRAPH_TO_ROT, OnDbgAddGraphToRot)
	ON_COMMAND(ID_DBG_RELEASE_FILTERS, OnDbgReleaseFilters)
	ON_CBN_SELCHANGE(IDC_COMBO_QUALITY, OnSelchangeComboQuality)
	ON_CBN_SELCHANGE(IDC_COMBO_FRQ, OnSelchangeComboFrq)
	ON_COMMAND(IDC_BTN_RENDERER_PROPERTY, OnBtnRendererProperty)
	ON_COMMAND(IDC_BTN_SOURCE_PROPERTY, OnBtnSourceProperty)
	ON_COMMAND(IDC_BTN_AUDIO_DEVICE_PROPERTY, OnBtnAudioDeviceProperty)
	ON_UPDATE_COMMAND_UI(ID_DBG_ADD_GRAPH_TO_ROT, OnUpdateDbgAddGraphToRot)
	ON_COMMAND(ID_DBG_REMOVE_GRAPH_TO_ROT, OnDbgRemoveGraphToRot)
	ON_UPDATE_COMMAND_UI(ID_DBG_REMOVE_GRAPH_TO_ROT, OnUpdateDbgRemoveGraphToRot)
	ON_UPDATE_COMMAND_UI(ID_DBG_RELEASE_FILTERS, OnUpdateDbgReleaseFilters)
	ON_COMMAND(ID_SETTINGS_VOLUME, OnSettingsVolume)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS_VOLUME, OnUpdateSettingsVolume)
	ON_UPDATE_COMMAND_UI(IDC_BTN_AUDIO_DEVICE_PROPERTY, OnUpdateBtnAudioDeviceProperty)
	ON_UPDATE_COMMAND_UI(IDC_BTN_RENDERER_PROPERTY, OnUpdateBtnRendererProperty)
	ON_UPDATE_COMMAND_UI(IDC_BTN_SOURCE_PROPERTY, OnUpdateBtnSourceProperty)
	ON_UPDATE_COMMAND_UI(IDC_BTN_ENCODER_PROPERTY, OnUpdateBtnEncoderProperty)
	ON_UPDATE_COMMAND_UI(IDC_START, OnUpdateStart)
	ON_UPDATE_COMMAND_UI(IDC_STOP, OnUpdateStop)
	ON_UPDATE_COMMAND_UI(IDC_PAUSE, OnUpdatePause)
	ON_UPDATE_COMMAND_UI(IDC_INIT_AUDIO_RECORD, OnUpdateInitAudioRecord)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_RECORDING_LEVEL, OnReleasedcaptureSliderRecordingLevel)
	ON_BN_CLICKED(IDC_RD_LEFT, OnRdLeft)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CK_LISTEN, OnCkListen)
	ON_BN_CLICKED(IDC_CK_DELIVER_ALWAYS, OnCkDeliverAlways)
	ON_WM_SYSCOMMAND()
	ON_EN_CHANGE(IDC_EDT_HOLD_TIME, OnChange)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnChange)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnChange)
	ON_CBN_SELCHANGE(IDC_COMBO_SEQUENCE, OnChange)
	ON_BN_CLICKED(IDC_RD_RIGHT, OnRdLeft)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VOLUME, OnReleasedcaptureSliderVolume)
	ON_BN_CLICKED(IDC_SLIDER_VOLUME, SetOutputParam)
	ON_MESSAGE(WM_DEVICECHANGE, OnDeviceChange)
	ON_MESSAGE(WM_INPUTLIST_SETCHECK, OnInputListSetCheck)

	ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphNotify)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_ENTERMENULOOP, OnEnterMenuLoop)
	ON_MESSAGE(WM_TRACE_TEXTA, OnTraceTextA)
	ON_MESSAGE(WK_WINTHREAD_STOP_THREAD, OnWinthreadStopThread)
	ON_MESSAGE(PSM_TRIGGER_UNIT, OnTriggerUnit)
	
	ON_MESSAGE(WM_REQUESTSTARTMEDIAENCODING, OnRequestStartMediaEncoding)
	ON_MESSAGE(WM_REQUESTSTOPMEDIAENCODING, OnRequestStopMediaEncoding)
	ON_MESSAGE(WM_REQUESTHARDWARE, OnRequestHardware)
	ON_MESSAGE(WM_REQUESTAUDIOQUALITIES, OnRequestAudioQualities)
	ON_MESSAGE(WM_REQUESTSETSAMPLEFREQUENCY, OnRequestSetSampleFrequency)
	ON_MESSAGE(WM_REQUESTSTOPMEDIADECODING, OnRequestStopMediaDecoding)
	ON_MESSAGE(WM_DECODEMEDIADATA, OnDecodeMediaData)
	ON_MESSAGE(WM_SET_DLG_ITEM_INT, OnSetDlgItemInt)
	ON_MESSAGE(WM_LANGUAGE_CHANGED, OnLanguageChanged)	
	ON_MESSAGE(WM_INPUTMIXER_VALUE, OnInputMixerValue)	
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioUnitDlg message handlers
BOOL CAudioUnitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(GetAppnameFromId(theApp.GetApplicationId()));

	RegisterDeviceDetect(m_hWnd, GetString(AM_KSCATEGORY_AUDIO));

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon[0], TRUE);			// Set big icon
	SetIcon(m_hIcon[0], FALSE);		// Set small icon
	SetButtonIcon(IDC_START, IDC_START);
	SetButtonIcon(IDC_PAUSE, IDC_PAUSE);
	SetButtonIcon(IDC_STOP , IDC_STOP );
	SetButtonIcon(IDC_RECORDING, IDC_STOP);
	SetButtonIcon(IDC_PLAYING  , IDC_STOP);
		
	HRESULT hr;
	HR_REPORT(CoInitialize(NULL));

#ifdef _USE_AS_MEDIAPLAYER
	if (!theApp.m_sParam.IsEmpty())
	{
		CRect rcSource, rcStart, rcCancel, rc;
		CWnd *pGrpSource = GetDlgItem(IDC_GRP_SOURCE);
		CWnd *pCancel	 = GetDlgItem(IDCANCEL);
		pGrpSource->GetWindowRect(&rcSource);
		ScreenToClient(&rcSource);
		m_btnStart.GetWindowRect(&rcStart);

		SetMenu(NULL);

		m_cVolume.SetRange(0, 1000);
		m_nVolume = 1000;
		m_cVolume.SetPos(m_nVolume);
		m_cThreshold.SetRange(0, 1000);
		m_cThreshold.SetPos(0);

		CWnd *pWnd = GetWindow(GW_CHILD);
		while (pWnd)
		{
			pWnd->ShowWindow(SW_HIDE);
			pWnd = pWnd->GetWindow(GW_HWNDNEXT);
		}
		int nFind = theApp.m_sParam.ReverseFind(_T('\\'));
		if (nFind == -1) nFind = 0;
		else			 nFind++;
		pGrpSource->SetWindowText(theApp.m_sParam.Mid(nFind));
		rc.left   = rcSource.left + rcStart.Width();
		rc.right  = rc.left + rcStart.Width();
		rc.top    = rcSource.top + rcStart.Height();
		rc.bottom = rc.top + rcStart.Height();
		m_btnStart.MoveWindow(&rc, FALSE);
		int nOffsetX = MulDiv(rcStart.Width(), 12, 10);
		int nOffsetY = MulDiv(rcStart.Height(), 12, 10);
		rc.OffsetRect(nOffsetX, 0);
		m_btnPause.MoveWindow(&rc, FALSE);
		rc.OffsetRect(nOffsetX, 0);
		m_btnStop.MoveWindow(&rc, FALSE);
		pCancel->GetWindowRect(&rcCancel);
		rc.left += nOffsetX;
		rc.right  = rc.left + rcCancel.Width();
		pCancel->MoveWindow(&rc, FALSE);

		rc.left   = rcSource.left + rcStart.Width();
		rc.right  = rcSource.right - rcStart.Width();
		rc.OffsetRect(0, nOffsetY);
		m_cVolume.MoveWindow(&rc, FALSE);
		rc.OffsetRect(0, nOffsetY);
		m_cThreshold.MoveWindow(&rc, FALSE);

		rcSource.bottom = rc.bottom + nOffsetY;
		pGrpSource->MoveWindow(&rcSource, FALSE);

		pCancel->ShowWindow(SW_SHOW);
		pGrpSource->ShowWindow(SW_SHOW);
		m_btnStart.ShowWindow(SW_SHOW);
		m_btnPause.ShowWindow(SW_SHOW);
		m_btnStop.ShowWindow(SW_SHOW);
		m_cVolume.ShowWindow(SW_SHOW);
		m_cThreshold.ShowWindow(SW_SHOW);
		GetWindowRect(&rc);
		rc.bottom = rc.top + rcSource.Height() + GetSystemMetrics(SM_CYCAPTION);
		SetWindowPos(NULL, 0, 0, rc.Width(), rcStart.Height() + rcSource.Height() + GetSystemMetrics(SM_CYCAPTION), SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
//		ShowWindow(SW_MINIMIZE);
		PostMessage(WM_COMMAND, IDC_PLAYING);
		return 0;
	}
#endif // _USE_AS_MEDIAPLAYER

	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (pSysDevEnum)
	{
		pSysDevEnum->AddRef();
		hr = EnumFiltersWithMonikerToList(pSysDevEnum, &CLSID_AudioInputDeviceCategory, &m_cCapture);
		if (hr == S_FALSE)
		{
			ClearFilterListWithMoniker(m_cCapture);
		}
		else
		{
			SetComboBoxSelection(m_cCapture, m_sCapture);
		}

	#ifndef DEBUG_WITOUT_COMPRESSORS
		pSysDevEnum->AddRef();
		hr = EnumFiltersWithMonikerToList(pSysDevEnum, &CLSID_AudioCompressorCategory, &m_cCompressors);
		if (hr == S_FALSE)
		{
			ClearFilterListWithMoniker(m_cCompressors);
		}
		else
		{
			if (AU_DEFAULT_CODEC == m_sCompressor)
			{
				TCHAR *szDefault[] = 
				{									// Nr  Byte per Sec
					AU_DEFAULT_CODEC,				// 1   1000 - 2000 (mpeg)
					_T("WMAudio"),					// 2   1000
					_T("Windows Media Audio V2"),	// 3   1000
					_T("DSP Group TrueSpeech"),		// 4   1067
					_T("GSM 6.10"),					// 5   1625
					_T("IMA ADPCM"),				// 6   4055
					_T("Microsoft ADPCM"),			// 7   4096
					_T("PCM"),						// 8   8000
					_T("CCITT"),					// 9   8000
					NULL
				};
				for (int i=0; szDefault[i]!=NULL; i++)
				{
					m_sCompressor = szDefault[i];
					if (SetComboBoxSelection(m_cCompressors, m_sCompressor))
					{
						break;
					}
				}
			}
			else
			{
				SetComboBoxSelection(m_cCompressors, m_sCompressor);
			}
		}
	#endif

		pSysDevEnum->AddRef();
		EnumRenderers(pSysDevEnum);
#ifdef _DEBUG
		pSysDevEnum->AddRef();
		hr = EnumFiltersWithMonikerToList(pSysDevEnum, &KSCATEGORY_AUDIO_DEVICE, &m_cAudioDevices);
		if (hr == S_FALSE)
		{
			ClearFilterListWithMoniker(m_cAudioDevices);
		}
		else
		{
			SetComboBoxSelection(m_cAudioDevices, m_sAudioDevice);
		}
#else
		m_cAudioDevices.ShowWindow(SW_HIDE);
#endif
		RELEASE_OBJECT(pSysDevEnum);
	}
	
	m_bStereo |= UPDATE_DLG_CTRL_VALUE;
	OnCkStereo();

	m_cInputPinList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	CRect rc;
	m_cInputPinList.GetClientRect(&rc);
	m_cInputPinList.InsertColumn(0, _T("Select"), LVCFMT_LEFT, rc.right-GetSystemMetrics(SM_CXVSCROLL));

	m_cRecLevel.SetRange32(0, 100);
	m_cRecLevel.SetPos(0);
	m_cRecPeakLevel.SetRange32(0, 100);
	m_cRecPeakLevel.SetPos(0);
	
	m_cThreshold.SetRange(0, 100);
	m_cRecordingLevel.SetRange(0, 1000);
	m_cVolume.SetRange(0, 1000);
	m_cVolume.SetPos(m_nVolume);
	
	OnKillfocusEdtThreshold();
	OnCkRecordingLevel();


	CWK_Profile wkpBase;
	int nConnect = theApp.ReadAndInitIntKey(wkpBase, DVRT_DEBUG_KEY, CONNECT2AUDIOUNITS, 0);
	if (nConnect)
	{
		BOOL asMaster = theApp.GetInstanceCount() == 1 ? TRUE : FALSE;
		m_pCIPCMedia = new CIPCMediaAudioUnit(_T("SMAudioUnitMedia"), asMaster, this);
		m_bCapturing = true;
		m_bDeliverAlways = TRUE;
	}
	else
	{
		CString str;
		str.Format(SM_AudioUnitMedia, theApp.GetInstanceCount());
		m_pCIPCMedia = new CIPCMediaAudioUnit(str, FALSE, this);
	}


/*
	if (m_sCapture.IsEmpty())
	{
		OnApplyNow();
	}
*/
	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		OnAbout();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::SetButtonIcon(int nBtnID, int nIconID)
{
   SendDlgItemMessage(nBtnID, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(nIconID), IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
	CWnd *pWnd = GetDlgItem(nBtnID);
	if (pWnd && theApp.GetMainTreadID() == GetCurrentThreadId())
	{
		pWnd->UpdateWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CAudioUnitDlg::ControlStartStopBtn(bool bStarted)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pMC)
	{
		OAFilterState oafs;
		m_pMC->GetState(0, &oafs);
		
		m_btnStart.EnableWindow(oafs != State_Running);
		m_btnStop.EnableWindow( oafs != State_Stopped);
		m_btnPause.EnableWindow(oafs != State_Paused);
	}
	else
	{
		m_btnStart.EnableWindow(!bStarted);
		m_btnStop.EnableWindow(  bStarted);
		m_btnPause.EnableWindow( bStarted);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon[0]);
	}
	else
	{
		CDialog::OnPaint();
	}
}
/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAudioUnitDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon[0];
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnAbout() 
{
	COemGuiApi::AboutDialog(this);
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::SetAUIcon(WORD wColor, BOOL bType)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	static WORD wCurColor = ICON_NOCOLOR;
	static BOOL bCurType	 = FALSE;

	if ((wColor != wCurColor) || (bType != bCurType))
	{
		wCurColor = wColor;
		HICON hIcon = m_hIcon[0];
		if ((wColor >= ICON_MAIN) && (wColor <= ICON_RECORDING))
		{
			hIcon = m_hIcon[wColor-1];
		}
		if (m_bStandalone)
		{
			::PostMessage(m_hWnd, WM_SETICON,	bType, (LPARAM)hIcon);
		}
		else
		{
			// Icon ins Systemtray
			NOTIFYICONDATA tnd;

			CString sTip = COemGuiApi::GetApplicationName(theApp.GetApplicationId());

			tnd.cbSize				= sizeof(NOTIFYICONDATA);
			tnd.hWnd				= m_hWnd;
			tnd.uID					= 1;
			tnd.uFlags				= NIF_MESSAGE|NIF_ICON|NIF_TIP;
			tnd.uCallbackMessage	= WM_TRAYCLICKED;
			tnd.hIcon				= hIcon;

			lstrcpyn(tnd.szTip, sTip, min(64, sTip.GetLength()+1));

			Shell_NotifyIcon(NIM_MODIFY, &tnd);
			if (::IsWindowVisible(m_hWnd))
			{
				::PostMessage(m_hWnd, WM_SETICON,	bType, (LPARAM)hIcon);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
int CAudioUnitDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_bStandalone == TRUE)
	{	
		SetWindowPos(&CWnd::wndTop, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
	}
	else
	{
		// Icon ins Systemtray
		NOTIFYICONDATA tnd;

		CString sTip = COemGuiApi::GetApplicationName(theApp.GetApplicationId());

		tnd.cbSize = sizeof(NOTIFYICONDATA);
		tnd.hWnd	  = m_hWnd;
		tnd.uID    = 1;
		tnd.hIcon  = m_hIcon[0];
		tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
		tnd.uCallbackMessage = WM_TRAYCLICKED;
		lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
		Shell_NotifyIcon(NIM_ADD, &tnd);
	}
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;
			int    nEnable = MF_BYCOMMAND;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MAINFRAME);
			CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_strKey, _T(""));
			DWORD dwExtended  = wkp.GetInt(   AU_DEBUG_SETTINGS, _T("ExtendedMenu"), 0);
			if (dwExtended == 1)
			{
				pM = menu.GetSubMenu(1);
			}
			else
			{
				pM = menu.GetSubMenu(0);
			}
			
			COemGuiApi::DoUpdatePopupMenu(AfxGetMainWnd(), pM);
			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnTraceTextA(WPARAM pszModule, LPARAM pszMsg)
{
	if (pszModule && pszMsg)
	{
		CWK_String sModule((LPCSTR)pszModule), sMsg((LPCSTR)pszMsg);
#ifdef _DEBUG

		TRACE(_T("%s:%s\n"), LPCTSTR(sModule), LPCTSTR(sMsg));

#else // !_DEBUG

		if (theApp.m_bTraceFilterMessages)
		{
			WK_TRACE(_T("%s:%s\n"), LPCTSTR(sModule), LPCTSTR(sMsg));
		}

		free((void*)pszMsg); // may lead to assertions, if mixed debug and release versions are used !!

#endif

	}
	else if (pszModule)	// Request whether the message is handled by the application
	{
		LRESULT *pResult = (LRESULT*)pszModule;
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_strKey, _T(""));
		*pResult = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("TraceFilters"));
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnDestroy() 
{
#ifdef	_USE_AS_MEDIAPLAYER
	if (m_nPosUpdatetimer)
	{
		KillTimer(m_nPosUpdatetimer);
		m_nPosUpdatetimer = 0;
	}
#endif // _USE_AS_MEDIAPLAYER
//	if (m_pCIPCMedia  ) m_pCIPCMedia->DoDisconnect();
//	if (m_pCIPCcontrol) m_pCIPCcontrol->DoDisconnect();
//	if (m_pCIPCInput  ) m_pCIPCInput->DoDisconnect();
	WK_DELETE(m_pCIPCMedia);
	WK_DELETE(m_pCIPCcontrol);
	WK_DELETE(m_pCIPCInput);

	OnStop();

	ClearAudioStreamConfigCapsList(m_cQuality);
	ReleaseFilters();
	ClearFilterListWithMoniker(m_cCapture);
	ClearFilterListWithMoniker(m_cCompressors);
	ClearFilterListWithMoniker(m_cRenderer);
	ClearFilterListWithMoniker(m_cAudioDevices);

	CoUninitialize();
	KillDwellTimer();
	
	if (m_bStandalone != TRUE)
	{
		NOTIFYICONDATA tnd;

		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID			= 1;
		tnd.uFlags		= 0;

		Shell_NotifyIcon(NIM_DELETE, &tnd);
		PostQuitMessage(0);
	}

	CDialog::OnDestroy();
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnOK() 
{
	DestroyWindow();
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnCancel() 
{
	if	(m_bStandalone)
	{
		DestroyWindow();
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnGraphNotify(WPARAM wParam,  LPARAM lParam)
{
	LONG evCode, evParam1, evParam2;
	HRESULT hr=S_OK;
	if (m_pME)
	{
		while(SUCCEEDED(m_pME->GetEvent(&evCode, (LONG_PTR *) &evParam1, (LONG_PTR *) &evParam2, 0)))
		{
			CString str;
			switch (evCode)
			{
				TRACE_NAME_OF(EC_USERABORT);
				TRACE_NAME_OF(EC_ERRORABORT);
				TRACE_NAME_OF(EC_TIME);
				TRACE_NAME_OF(EC_REPAINT);
				TRACE_NAME_OF(EC_STREAM_ERROR_STOPPED);
				TRACE_NAME_OF(EC_STREAM_ERROR_STILLPLAYING);
				TRACE_NAME_OF(EC_ERROR_STILLPLAYING);
				TRACE_NAME_OF(EC_PALETTE_CHANGED);
				TRACE_NAME_OF(EC_VIDEO_SIZE_CHANGED);
				TRACE_NAME_OF(EC_QUALITY_CHANGE);
				TRACE_NAME_OF(EC_SHUTTING_DOWN);
				TRACE_NAME_OF(EC_CLOCK_CHANGED);
				TRACE_NAME_OF(EC_PAUSED);
				TRACE_NAME_OF(EC_OPENING_FILE);
				TRACE_NAME_OF(EC_BUFFERING_DATA);
				TRACE_NAME_OF(EC_FULLSCREEN_LOST);
				TRACE_NAME_OF(EC_ACTIVATE);
				TRACE_NAME_OF(EC_NEED_RESTART);
				TRACE_NAME_OF(EC_WINDOW_DESTROYED);
				TRACE_NAME_OF(EC_DISPLAY_CHANGED);
				TRACE_NAME_OF(EC_STARVATION);
				TRACE_NAME_OF(EC_OLE_EVENT);
				TRACE_NAME_OF(EC_NOTIFY_WINDOW);
				TRACE_NAME_OF(EC_STREAM_CONTROL_STOPPED);
				TRACE_NAME_OF(EC_STREAM_CONTROL_STARTED);
				TRACE_NAME_OF(EC_END_OF_SEGMENT);
				TRACE_NAME_OF(EC_SEGMENT_STARTED);
				TRACE_NAME_OF(EC_LENGTH_CHANGED);
				TRACE_NAME_OF(EC_DEVICE_LOST);
				TRACE_NAME_OF(EC_STEP_COMPLETE);
				TRACE_NAME_OF(EC_TIMECODE_AVAILABLE);
				TRACE_NAME_OF(EC_EXTDEVICE_MODE_CHANGE);
				TRACE_NAME_OF(EC_STATE_CHANGE);
				TRACE_NAME_OF(EC_GRAPH_CHANGED);
				TRACE_NAME_OF(EC_CLOCK_UNSET);
				TRACE_NAME_OF(EC_VMR_RENDERDEVICE_SET);
				TRACE_NAME_OF(EC_VMR_SURFACE_FLIPPED);
				TRACE_NAME_OF(EC_VMR_RECONNECTION_FAILED);
				case EC_COMPLETE:
				{
					TRACE(_T("EC_COMPLETE\n"));
#ifdef _USE_AS_MEDIAPLAYER
					if (!theApp.m_sParam.IsEmpty())
					{
						PostMessage(WM_COMMAND, IDC_STOP);
						PostMessage(WM_COMMAND, IDCANCEL);
					}
#endif // _USE_AS_MEDIAPLAYER
				}break;

				case EC_ACTIVATE_TRANSPORT:
				{
					m_nDwellTimer = SetTimer(TIMER_HOLDTIME, 1000, NULL);
					m_nDwellTimeCount = 0;
					if (!m_bRecordingLevel && m_pSplitterInterface)
					{
						m_pSplitterInterface->SetMediaSampleCheckFnc(CheckMediaSample, (LPARAM)this);
					}
					SetAUIcon(ICON_RECORDING);
					SetButtonIcon(IDC_RECORDING, IDC_RECORDING);
					if (m_bGenerateAlarm && m_pCIPCInput)
					{
						m_pCIPCInput->SetAudioAlarm(TRUE);
					}
				} 
				continue;	// User define Event: do not invoke FreeEventParams(..)!
				break;
				case EC_RECEIVE_MEDIA_SAMPLE:
				{
					IMediaSample *pSample    = (IMediaSample*)evParam1;
					long          nPackageNo = (long)evParam2;
					ReceiveMediaSample(pSample, nPackageNo, (long)this);
					pSample->Release();
				}
				continue;	// User define Event: do not invoke FreeEventParams(..)!
				break;
				default:
//					str.Format(_T("Event(%d, %x), %x, %x\r\n"), evCode, evCode, evParam1, evParam2);
//					WK_TRACE(str);
					break;
			}
			// Free event parameters to prevent memory leaks
			hr = m_pME->FreeEventParams(evCode, evParam1, evParam2);
		}
		
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnCkStereo() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	BOOL bSave = TRUE;
	if (m_bStereo & UPDATE_DLG_CTRL_VALUE)
	{
		m_bStereo &= ~UPDATE_DLG_CTRL_VALUE;
	}
	CDataExchange dx(this, bSave);
	DDX_Check(&dx, IDC_CK_STEREO, m_bStereo);

	CWnd *pWnd = GetDlgItem(IDC_RD_RIGHT);
	if (m_bStereo == 0)
	{
		m_nStereoChannel = 0;
		dx.m_bSaveAndValidate = FALSE;
		DDX_Radio(&dx, IDC_RD_LEFT, m_nStereoChannel);
		if (pWnd) pWnd->EnableWindow(m_bStereo);
	}
	else
	{
		if (pWnd) pWnd->EnableWindow(m_bStereo);
	}
	dx.m_bSaveAndValidate = TRUE;
	OnChange();
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnRdLeft() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	BOOL bSave = TRUE;
	if (m_nStereoChannel & UPDATE_DLG_CTRL_VALUE)
	{
		bSave = FALSE;
		m_nStereoChannel &= ~UPDATE_DLG_CTRL_VALUE;
	}
	CDataExchange dx(this, bSave);
	DDX_Radio(&dx, IDC_RD_LEFT, m_nStereoChannel);
	if (m_pSplitterInterface)
	{
		HRESULT hr;
		HR_REPORT(m_pSplitterInterface->SetPinNumber(0, m_nStereoChannel+1));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnChange() 
{
	m_btnInitAudioRec.EnableWindow();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnReportError(WPARAM wParam, LPARAM lParam)
{
	ReportError((ErrorStruct*)wParam, lParam ? true : false);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::ReportError(ErrorStruct*ps, bool bMsgBox)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	ASSERT(ps != NULL);
	ASSERT(ps != m_pLastError);
	if (FAILED(ps->m_hr))
	{
		CString str, sError, sDescription;
		GetErrorStrings(ps->m_hr, sError, sDescription);
#ifdef _UNICODE
		CWK_String sFile(ps->m_sFile);
		str.Format(_T("%s(%d): DirectShow Error %s(%x)\n%s\n"), LPCWSTR(sFile), ps->m_nLine, sError, ps->m_hr, sDescription);
#else
		str.Format(_T("%s(%d): DirectShow Error %s(%x)\n%s\n"), ps->m_sFile, ps->m_nLine, sError, ps->m_hr, sDescription);
#endif		
#ifdef _DEBUG
		TRACE(str);
#else
		WK_TRACE(str);
#endif		
		if (m_bStandalone && bMsgBox)
		{
			AfxMessageBox(str, MB_ICONERROR|MB_OK);
		}
	}
	WK_DELETE(m_pLastError);
	m_pLastError = ps;
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::IndicateLastMediaError()
{
	if (m_pLastError)
	{
		DWORD dwParam1 = 0, dwParam2 = 0;
		WORD  wCmd = 0;
		CIPCMediaAudioUnit *pMedia = m_pCIPCMedia;
		if (m_pCIPCcontrol) pMedia = m_pCIPCcontrol;
		if (pMedia)
		{
			dwParam1 = pMedia->GetReplyID().GetID();
			wCmd     = (WORD)pMedia->GetLastCmd();
		}
		dwParam2 = MAKELONG(0x02|0x08, wCmd); // show MsgBox, no virtual alarm
		CString str, sError, sDescription;
		GetErrorStrings(m_pLastError->m_hr, sError, sDescription);
		str = sError + _T("\n") + sDescription;
		CWK_RunTimeError wkrt(theApp.GetApplicationId(), REL_MESSAGE, RTE_EXTERNAL_DEVICE_FAILURE, str, dwParam1, dwParam2);
		wkrt.Send();
		WK_DELETE(m_pLastError);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::InitGraphBuilder()
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pGB == NULL)
	{
		HRESULT hr;
		// Create the  graph builder.
		HR_EXCEPTION(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGB));
		// Create the capture graph builder.
		HR_EXCEPTION(CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **)&m_pCaptureGB));
		// Associate the filter graph with the capture graph builder
		HR_EXCEPTION(m_pCaptureGB->SetFiltergraph(m_pGB));						
		// Query the Media Control Interface
		HR_EXCEPTION(m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC));
		// Query the Media Event Interface
		HR_EXCEPTION(m_pGB->QueryInterface(IID_IMediaEventEx, (void **)&m_pME));

		HR_REPORT(m_pME->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPHNOTIFY, 0));
		IObjectWithSite* pObjectWithSite = NULL;
		hr = m_pGB->QueryInterface(IID_IObjectWithSite, (void**)&pObjectWithSite);
		if (SUCCEEDED(hr))
		{
			// Use the IObjectWithSite pointer to specify our key provider object.
			// The filter graph manager will use this pointer to call
			// QueryService to do the unlocking.
			// If the unlocking succeeds, then we can build our graph.
			hr = pObjectWithSite->SetSite((IUnknown *) (IServiceProvider *) &m_Prov);
			pObjectWithSite->Release();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnInitAudioRecord() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	HRESULT          hr        = NOERROR;
	IMoniker        *pMoniker  = NULL;
	IPin            *pPinOut   = NULL;
	IPin            *pPinIn    = NULL;
	IFileSinkFilter *pFileSink = NULL;
	IAMStreamConfig *pCfg      = NULL;
	IAMBufferNegotiation *pBN  = NULL;							// Suggest the allocator properties
	ALLOCATOR_PROPERTIES ap;
	
	OnStop();
	ReleaseFilters(false);
	UpdateData();
	try
	{
		WK_TRACE(_T("Initialize Media Encoder %x\n"), GetCurrentThreadId());
		InitGraphBuilder();
		// Create the capture device
		pMoniker = (IMoniker*)m_cCapture.GetItemData(m_cCapture.GetCurSel());
		if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
		HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pCapture, pMoniker, FN_OF_CAPTURE_DEVICE));
/*		
		IBaseFilter *pFilter = NULL;
		pMoniker = (IMoniker*)m_cAudioDevices.GetItemData(m_cAudioDevices.GetCurSel());
		if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
		HR_EXCEPTION(AddFilterToGraph(m_pGB, &pFilter, pMoniker, L"Audio Device"));
		GetPin(pFilter, PINDIR_INPUT, 0, &pPinIn);
		if (pPinIn)
		{
			IMiniport *pPort;
			pPinIn->QueryInterface(IMiniport)
		}
*/

		HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pSplitter, CLSID_WaveSplitter, FN_OF_WAVE_SPLITTER));

		HR_EXCEPTION(GetPin(m_pCapture, PINDIR_OUTPUT, 0, &pPinOut));// Query pins of the capture device

#ifdef _DEBUG
		IAMPhysicalPinInfo *pPPI;
		hr = pPinOut->QueryInterface(IID_IAMPhysicalPinInfo, (void **)&pPPI);
		if (SUCCEEDED(hr))
		{
			pPPI->Release();
		}
#endif
		
		int nBytesPerSample = m_b2Bytes + 1;							// Default: 2 Byte
		int nChannels       = m_bStereo + 1;							// Default: Mono
		hr = pPinOut->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
		if (SUCCEEDED(hr))
		{
			AM_MEDIA_TYPE *pmt={0};

			hr = pCfg->GetFormat(&pmt);									// Read current media type/format
			if (SUCCEEDED(hr))
			{
				ASSERT(pmt->formattype == FORMAT_WaveFormatEx);
				WAVEFORMATEX *pWF = (WAVEFORMATEX *) pmt->pbFormat;

				pWF->nChannels       = (WORD) nChannels;
				pWF->nSamplesPerSec  = _ttoi(m_strFrq);
				pWF->nAvgBytesPerSec = pWF->nSamplesPerSec * pWF->nChannels * nBytesPerSample;
				pWF->wBitsPerSample  = (WORD) (nBytesPerSample * 8);
				pWF->nBlockAlign     = (WORD) (nBytesPerSample * nChannels);
				HR_REPORT(pCfg->SetFormat(pmt));							// Set the new formattype for the output pin

				DeleteMediaType(pmt);
			}
			RELEASE_OBJECT(pCfg);
		}
		else
		{
			ReportError(HRESULT_ERROR_AT_POS(hr));
		}

		hr = pPinOut->QueryInterface(IID_IAMBufferNegotiation, (void**)&pBN);
		if (SUCCEEDED(hr))
		{
			int nLength = 5 - m_nSequence;
			ap.cbAlign  = 1;
			ap.cbPrefix = 0;
			ap.cBuffers = 16;
			ap.cbBuffer = _ttoi(m_strFrq) * nChannels / nLength;
			HR_REPORT(pBN->SuggestAllocatorProperties(&ap));		// Suggestion must be done before connection
			nLength = 1000 / (nLength+1);
			nLength = m_nDwellTime * 1000 / nLength;
			m_AverageLevel.SetNoOfAvgValues(nLength);
			m_AverageLevel.RemoveAll();
		}
		else
		{
			ReportError(HRESULT_ERROR_AT_POS(hr));
		}

		hr = ConnectFilters(m_pGB, m_pCapture, 0, m_pSplitter, 0);
		if (FAILED(hr)) throw HRESULT_ERROR_AT_POS(hr);

		if (pBN)																	// Request the allocator properties
		{																			// after connection
			hr = pBN->GetAllocatorProperties(&ap);
			if (SUCCEEDED(hr))
			{
				int nLength = _ttoi(m_strFrq) * nChannels / ap.cbBuffer;
				m_nSequence = 5 - nLength;
				if      (m_nSequence < AU_SEQUENCE_62_5) m_nSequence = AU_SEQUENCE_62_5;
				else if (m_nSequence > AU_SEQUENCE_1000) m_nSequence = AU_SEQUENCE_1000;
				SendDlgItemMessage(IDC_COMBO_SEQUENCE, CB_SETCURSEL, m_nSequence, 0);
				m_nSequenceLength = GetDlgItemInt(IDC_COMBO_SEQUENCE);
				if (!IsBetween(m_nSequenceLength, 50, 1000))
				{
					m_nSequenceLength = 250;
				}
			}
			else
			{
				ReportError(HRESULT_ERROR_AT_POS(hr), false);
			}
		}
		RELEASE_OBJECT(pBN);
		RELEASE_OBJECT(pPinOut);
		RELEASE_OBJECT(pPinIn);

		PinsToList();

		hr = GetPin(m_pSplitter  , PINDIR_INPUT , 0, &pPinIn);
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		hr = pPinIn->QueryInterface(IID_IWaveSplitter, (void **)&m_pSplitterInterface);
		if (!m_pSplitterInterface) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
		if (m_bRecordingLevel)
		{
			m_pSplitterInterface->SetMediaSampleCheckFnc(CheckMediaSample, (long)this);
		}
		m_pSplitterInterface->SetPinNumber(0, m_nStereoChannel+1);
		RELEASE_OBJECT(pPinIn);
		
		// Create the encoder device
#ifdef DEBUG_WITOUT_COMPRESSORS
		// fest eingestellter MP3 encoder
		HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pEncode, _T("6A08CF80-0E18-11CF-A24D-0020AFD79767"), FN_OF_ENCODER));
#else
		pMoniker = (IMoniker*)m_cCompressors.GetItemData(m_cCompressors.GetCurSel());
		if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
		HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pEncode, pMoniker, FN_OF_ENCODER));
#endif
		
		// Query pins of the wave splitter device and the encoder device
		HR_EXCEPTION(ConnectFilters(m_pGB, m_pSplitter, 0, m_pEncode, 0));
																					// create the file dump device
		HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pDump, CLSID_Dump, FN_OF_SAMPLE_DUMP));
		HR_EXCEPTION(m_pDump->QueryInterface(IID_IFileSinkFilter, (void **)&pFileSink));
		if (m_strOutputFileName.IsEmpty())
		{
			pFileSink->SetFileName(NULL, NULL);							// Set no filename
		}
		else
		{
			BSTR bstr = m_strOutputFileName.AllocSysString();
			pFileSink->SetFileName(bstr, NULL);							// Set a filename
			SysFreeString(bstr);
		}

		HR_EXCEPTION(GetPin(m_pEncode, PINDIR_OUTPUT, 0, &pPinOut));// Query pins of the encoder device and the dump device
		HR_EXCEPTION(GetPin(m_pDump  , PINDIR_INPUT , 0, &pPinIn ));
		IDump *pDump = NULL;
		HR_EXCEPTION(pPinIn->QueryInterface(IID_IDump, (void **)&pDump));
		if (m_strOutputFileName.IsEmpty())
		{																			// set the function to receive IMediaSample
			hr = pDump->SetReceiveFunction(ReceiveMediaSample, (long)this);
		}
		RELEASE_OBJECT(pDump);

		hr = pPinOut->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
		if (SUCCEEDED(hr))
		{
			AddAudioStreamConfigCapsToList(m_pEncode, 0, m_cQuality);
			AM_MEDIA_TYPE *pmt={0};
			hr = pCfg->GetFormat(&pmt);									// Read current media type/format
			if (SUCCEEDED(hr))
			{
				if (pmt->formattype != FORMAT_WaveFormatEx) throw HRESULT_ERROR_AT_POS(ERROR_UNRECOGNIZED_MEDIA);
				CDataExchange dx(this, false);
				if (IsBetween(m_nQuality, 0, m_cQuality.GetCount()-1))
				{
					DDX_CBIndex(&dx, IDC_COMBO_QUALITY, m_nQuality);
					hr = pCfg->SetFormat((AM_MEDIA_TYPE*)m_cQuality.GetItemData(m_nQuality));
					DeleteMediaType(pmt);
					hr = pCfg->GetFormat(&pmt);							// Read current media type/format
				}
				WAVEFORMATEX  *pWF = (WAVEFORMATEX *) pmt->pbFormat;
				m_nBytesPerSecond = pWF->nAvgBytesPerSec;				// request the average bytes per second
				DDX_Text(&dx, IDC_EDT_BYTES_PER_SECOND, m_nBytesPerSecond);
				DeleteMediaType(pmt);
			}
			RELEASE_OBJECT(pCfg);
		}
		HR_EXCEPTION(m_pGB->Connect(pPinOut, pPinIn));				// Connect the encoder device to the dump device

		if (theApp.GetAudioDebug())
		{
			AddGraphFiltersToList(m_pGB, &m_ListFilters);
		}
	}
   catch(ErrorStruct *ps)
	{
		ReportError(ps);
		ReleaseFilters(false);
	}

	RELEASE_OBJECT(pFileSink);
	RELEASE_OBJECT(pPinOut);
	RELEASE_OBJECT(pPinIn);
	RELEASE_OBJECT(pBN);
	
	m_btnInitAudioRec.EnableWindow(hr != NOERROR);
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnInitAudioPlay() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	HRESULT     hr        = NOERROR;
	IPin        *pPinTemp = NULL;
	IBaseFilter *pFilter  = NULL;
#ifdef _DEBUG
	IFileSourceFilter *pFSF = NULL;
#endif

	try
	{
		if (m_MediaType.pbFormat == NULL
#ifdef _USE_AS_MEDIAPLAYER
			&& theApp.m_sParam.IsEmpty()
#endif // _USE_AS_MEDIAPLAYER
			)
		{
			throw HRESULT_ERROR_AT_POS(ERROR_INVALID_MEDIA);
		}
		AddGraphToRot(FALSE);
		InitGraphBuilder();
#ifdef _USE_AS_MEDIAPLAYER
		if (!theApp.m_sParam.IsEmpty())
		{
			LPCOLESTR sFile = theApp.m_sParam;
/*
			HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pMediaSampleSource, CLSID_AsyncReader, L"File"));
			HR_EXCEPTION(m_pMediaSampleSource->QueryInterface(IID_IFileSourceFilter, (void**) &pFSF));
			pFSF->Load(sFile, NULL);
			HR_EXCEPTION(GetPin(m_pMediaSampleSource, PINDIR_OUTPUT, 0, &pPinTemp));
			hr = m_pGB->Render(pPinTemp);
*/			
			hr = m_pGB->RenderFile(sFile, NULL);

			if (theApp.GetAudioDebug())
			{
				AddGraphFiltersToList(m_pGB, &m_ListFilters);
			}
			if (SUCCEEDED(hr))
			{
				PostMessage(WM_COMMAND, IDC_START);
			}
			else
			{
				PostMessage(WM_COMMAND, ID_APP_EXIT);
			}
		}
		else
#endif // _USE_AS_MEDIAPLAYER
		if (m_pMediaSampleSource==NULL)
		{
			// Create the PushSource Filter
			HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pMediaSampleSource, CLSID_PushSource, L"PushSource"));

			HR_EXCEPTION(m_pMediaSampleSource->QueryInterface(IID_IPushSource, (void**) &m_pIPushSource));
			RELEASE_OBJECT(m_pRender);
			IMoniker*pMoniker = (IMoniker*)m_cRenderer.GetItemData(m_cRenderer.GetCurSel());
			if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
			HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pRender, pMoniker, NULL));
			
			PushSourceParams psp;
			CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_strKey, _T(""));
			ZERO_INIT(psp);
			psp.m_nOffsetInMS         = wkp.GetInt(AU_DEBUG_SETTINGS, AUDIOOFFSETFORREPLAY, DEFAULT_OFFSET_REPLAY);
			psp.m_bCapture            = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("Capture"), 0);
			psp.m_bGenerateTimeStamps = TRUE;
			psp.m_bInternalRM         = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("InternalRateMatching"), FALSE);
			psp.m_bLiveSource         = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("LiveSource"), FALSE);
			psp.m_bPreview            = FALSE;
			psp.m_bStill			  = FALSE;
			psp.m_lMiscFlags          = 0;//AM_FILTER_MISC_FLAGS_IS_SOURCE;
			psp.m_bSortSamples        = theApp.ReadAndInitIntKey(wkp, AU_DEBUG_SETTINGS, _T("SortSamples"), FALSE); // TODO! RKE: SortSamples nur, wenn bei Sample 0 zurückgesetzt wird!
//			psp.m_dwFlags			  = PS_FLAG_SAVE_ONLY_FIRST_TIME;

			m_pIPushSource->SetParams(&psp);
			HR_EXCEPTION(m_pIPushSource->SetMediaType(&m_MediaType));
			HR_EXCEPTION(m_pIPushSource->SetBufferType(BUFFER_ATTACH, ReleaseBufferFunction, (long)this, &m_AP));
			HR_EXCEPTION(GetPin(m_pMediaSampleSource, PINDIR_OUTPUT, 0, &pPinTemp));
			
			hr = m_pCaptureGB->RenderStream(NULL, &MEDIATYPE_Audio, pPinTemp, NULL, m_pRender);
			if (FAILED(hr))
			{
				HR_REPORT(hr);
				CString sSearchName; 
				GetGUIDString(sSearchName, &m_MediaType.subtype, TRUE);
				HR_EXCEPTION(AddFilterToGraph(m_pGB, &pFilter, CLSID_CQzFilterClassManager, sSearchName, AFTG_MATCH_PARTIAL|AFTG_MATCH_CASE, L"Decoder"));
				HR_EXCEPTION(ConnectFilters(m_pGB, m_pMediaSampleSource, 0, pFilter, 0));
				HR_EXCEPTION(ConnectFilters(m_pGB, pFilter, 0, m_pRender, 0));
			}

			if (theApp.GetAudioDebug())
			{
				AddGraphFiltersToList(m_pGB, &m_ListFilters);
			}
/*
			IReferenceClock *pClock = NULL;
			if (SUCCEEDED(m_pRender->QueryInterface(IID_IReferenceClock, (void**)&pClock)))
			{
				IMediaFilter *pMediaFilter = NULL;
				if (SUCCEEDED(m_pGB->QueryInterface(IID_IMediaFilter, (void**)&pMediaFilter)))
				{
					HR_REPORT(pMediaFilter->SetSyncSource(pClock));
					RELEASE_OBJECT(pMediaFilter);
				}
				RELEASE_OBJECT(pClock);
			}
*/
		}
	}
	catch(ErrorStruct *ps)
	{
		if (m_MediaType.pbFormat != NULL)
		{
			FreeMediaType(m_MediaType);
		}
		ReportError(ps);
		ReleasePlayFilters();
	}
	RELEASE_OBJECT(pFilter);
	RELEASE_OBJECT(pPinTemp);

#ifdef _DEBUG
	RELEASE_OBJECT(pFSF);
#endif

	if (m_pRender)
	{
		m_cVolume.EnableWindow(TRUE);
		SetOutputParam();
	}
	else if (m_pGB)
	{
		IBasicAudio *pBA = NULL;
		m_pGB->QueryInterface(IID_IBasicAudio, (void**)&pBA);
		if (pBA)
		{
			m_cVolume.EnableWindow(TRUE);
			SetOutputParam();
			pBA->Release();
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::ReleaseFilters(bool bAll/*=true*/)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	AddGraphToRot(FALSE);
	if (m_pGB)
	{
		if (m_pCapture)  m_pGB->RemoveFilter(m_pCapture);
		if (m_pSplitter) m_pGB->RemoveFilter(m_pSplitter);
		if (m_pEncode)   m_pGB->RemoveFilter(m_pEncode);
		if (m_pDump)     m_pGB->RemoveFilter(m_pDump);

		if (bAll)
		{
			if (m_pRender)   m_pGB->RemoveFilter(m_pRender);
			if (m_pMediaSampleSource) m_pGB->RemoveFilter(m_pMediaSampleSource);
		}
	}
	PinsToList();

	RELEASE_OBJECT(m_pCapture);
	RELEASE_OBJECT(m_pSplitterInterface);
	RELEASE_OBJECT(m_pSplitter);
	RELEASE_OBJECT(m_pEncode);
	RELEASE_OBJECT(m_pDump);

	if (bAll)
	{
		RELEASE_OBJECT(m_pRender);
		RELEASE_OBJECT(m_pIPushSource);
		RELEASE_OBJECT(m_pMediaSampleSource);

		if (m_pME)
		{
			m_pME->SetNotifyWindow(NULL, 0, 0);
		}
		
		RELEASE_OBJECT(m_pMC);
		RELEASE_OBJECT(m_pME);

		RELEASE_OBJECT(m_pGB);
		RELEASE_OBJECT(m_pCaptureGB);
		m_ListFilters.ResetContent();
		m_cVolume.EnableWindow(FALSE);
	}
	else if (theApp.GetAudioDebug())
	{
		AddGraphFiltersToList(m_pGB, &m_ListFilters);
	}
	
	m_btnInitAudioRec.EnableWindow(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::ReleasePlayFilters()
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pRender) m_pGB->RemoveFilter(m_pRender);
	RELEASE_OBJECT(m_pRender);
	if (m_pMediaSampleSource)											// Filter entfernen
	{
		ASSERT(m_pGB != NULL);
		m_pGB->RemoveFilter(m_pMediaSampleSource);
		RELEASE_OBJECT(m_pIPushSource);
		RELEASE_OBJECT(m_pMediaSampleSource);
		HRESULT       hr;
		IEnumFilters *pEnum = NULL;
		IBaseFilter  *pFilter = NULL;
		ULONG         cFetched;
		CPtrList      Filters;

		hr = m_pGB->EnumFilters(&pEnum);	// Finde Decoder Filter
		if (SUCCEEDED(hr))
		{
			int nSize = wcslen(FN_OF_CAPTURERS);
			while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
			{
				FILTER_INFO FilterInfo;
				hr = pFilter->QueryFilterInfo(&FilterInfo);
				if (SUCCEEDED(hr))
				{
					if (wcsncmp(FilterInfo.achName, FN_OF_CAPTURERS, nSize) != 0)
					{	// if it is not a capturer filters name, it must be a decoder filter
						Filters.AddTail(pFilter);
						pFilter = NULL;
					}
					FilterInfo.pGraph->Release();
				}       
				RELEASE_OBJECT(pFilter);
			}
			pEnum->Release();
		}
		while (Filters.GetCount())
		{
			pFilter = (IBaseFilter*)Filters.RemoveHead();
			m_pGB->RemoveFilter(pFilter);
			RELEASE_OBJECT(pFilter);
		}
	}
	if (theApp.GetAudioDebug())
	{
		AddGraphFiltersToList(m_pGB, &m_ListFilters);
	}
	m_cVolume.EnableWindow(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
HRESULT WINAPI CAudioUnitDlg::CheckMediaSample(IMediaSample *pSample, AM_MEDIA_TYPE*pmt, long nPinNo, long lParam)
{
	if (!pmt) return E_POINTER;
	CAudioUnitDlg *pThis   = (CAudioUnitDlg*)lParam;
	WAVEFORMATEX  *pWFin   = (WAVEFORMATEX *) pmt->pbFormat;
	BYTE          *pSource = NULL;
#ifdef PEAK_DETECTION
	long           nSum    = 0;
#else
	double         dSum    = 0.0;
#endif
	pSample->GetPointer(&pSource);
	if (pWFin && pSource)
	{
		if (pWFin->wFormatTag == WAVE_FORMAT_PCM) // WAVE PCM
		{
			long lLength  = pSample->GetSize();
			int  i, nType = pWFin->nBlockAlign / pWFin->nChannels;
			if (nType == 2)
			{
				for (i= (nPinNo-1) * nType; i<lLength; i+=pWFin->nBlockAlign)
				{
#ifdef PEAK_DETECTION
					short nLevel = (short)abs(*((short*)&pSource[i]));
					if (nLevel > nSum)	// Peak detection
					{
						nSum = nLevel;
					}
#else
					long nLevel = (long)*((short*)&pSource[i]);
					dSum += (double)(nLevel * nLevel);
//					dSum += (double)abs(*((short*)&pSource[i]));
#endif

				}
#ifdef PEAK_DETECTION
				nSum = MulDiv(nSum, 100, 32767);
#else
				dSum = sqrt(dSum) / (lLength);
//				dSum = 0.003051 * dSum / lLength;
#endif
			}
			else if (nType == 1)
			{
				for (i= (nPinNo-1); i<lLength; i+=pWFin->nBlockAlign)
				{
#ifdef PEAK_DETECTION
					signed char nLevel = (signed char)abs((pSource[i]-127));
					if (nLevel > nSum)	// Peak detection
					{
						nSum = nLevel;
					}
#else
					signed char nLevel = (signed char)(pSource[i]-127);
					dSum += ((double)nLevel * (double)nLevel);
#endif
				}
#ifdef PEAK_DETECTION
				nSum = MulDiv(nSum, 100, 127);
#else
				dSum = 0.7874015748031496 * sqrt(dSum) / (lLength - 1);
#endif
			}
		}
	}
#ifdef PEAK_DETECTION
	pThis->m_nRecLevel = nSum;
	pThis->m_AverageLevel.AddValue(pThis->m_nRecLevel);
	pThis->m_nAverageLevel = (int) pThis->m_AverageLevel.GetAverage();
	if (pThis->m_bRecordingLevel == BST_INDETERMINATE)
	{
		DWORD dwValue = MAKELONG(nSum, pThis->m_nAverageLevel);
		if (pThis->m_pCIPCcontrol)
		{
			pThis->m_pCIPCcontrol->DoConfirmValues(pThis->m_pCIPCcontrol->GetSecID(AUDIO_INPUT), MEDIA_SET_VALUE|MEDIA_VOLUME_LEVEL, MEDIA_IDR_INPUT_PINS, dwValue);
		}
		else if (pThis->m_pCIPCMedia)
		{
			pThis->m_pCIPCMedia->DoConfirmValues(pThis->m_pCIPCMedia->GetReplyID(), MEDIA_SET_VALUE|MEDIA_VOLUME_LEVEL, MEDIA_IDR_INPUT_PINS, dwValue);
		}
	}
#else
	pThis->m_nRecLevel = (int)dSum;
	pThis->m_AverageLevel.AddValue(pThis->m_nRecLevel);
#endif
	if (pThis->m_bRecordingLevel != BST_UNCHECKED)
	{
		pThis->PostMessage(WM_TIMER, EVENT_REC_LEVEL);
	}
	return NOERROR;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT WINAPI CAudioUnitDlg::ReceiveMediaSample(IMediaSample *pSample, long nPackageNo, long lParam)
{
	CAudioUnitDlg *pThis = (CAudioUnitDlg*)lParam;
	return pThis->ReceiveMediaSample(pSample, nPackageNo);
}

HRESULT CAudioUnitDlg::ReceiveMediaSample(IMediaSample *pSample, long nPackageNo)
{
	CIPCMediaAudioUnit *pME = NULL;
	if (m_pCIPCMedia && m_pCIPCMedia->IsConnected())
	{
		pME = m_pCIPCMedia;
	}
	else if (m_pCIPCcontrol && m_pCIPCcontrol->IsConnected())
	{
		pME = m_pCIPCcontrol;
	}
	else if (m_pCIPCMedia)
	{
		pME = m_pCIPCMedia;
	}
	
	if (pME || m_bListen)
	{
		if (pME == NULL) return E_POINTER;
		
		PushSourceProperties msp;
		bool bSyncPoint = (pSample->IsSyncPoint() == S_OK) ? TRUE : FALSE;
		
		pSample->GetPointer(&msp.pBuffer);
		CSecID id = pME->GetEncodingID();
		CIPCMediaSampleRecord rec(pME, id);
		rec.SetData(nPackageNo, pSample->GetActualDataLength(), msp.pBuffer);
		TRACE(_T("PNo: %d:"), nPackageNo);
		
		CSystemTime st;														// set the system time
		st.GetLocalTime();
		rec.SetMediaTime(st);
		
		pSample->GetTime(&msp.rtTimeStart, &msp.rtTimeEnd);		// set the reference time
		rec.SetReferenceTimes(msp.rtTimeStart, msp.rtTimeEnd);

		// prevents delivering more than one sample with synchronization-point in a sequel of samples
		if (m_bLastSampleSyncPoint && bSyncPoint)
		{
			pSample->SetSyncPoint(FALSE);
		}
		m_bLastSampleSyncPoint = bSyncPoint;
	
		rec.SetMediaInfo(pSample->IsPreroll() == S_OK, pSample->IsDiscontinuity() == S_OK, pSample->IsSyncPoint() == S_OK);

//		TRACE(_T("pNr:%d, SP:%x\n"), nPackageNo, pSample->IsSyncPoint());
		if (   nPackageNo == 0											// Package 0: quite clear
			 || pSample->IsSyncPoint() == S_OK						// Sync Point: naturally
			 || (nPackageNo & 0x0000001f)	== 0x0000001f        // every 32nd sample, if need be
			 || m_bListen == TRUE)                             // for local replay start
		{
			TRACE(_T("LH %d, %d"), pSample->IsSyncPoint() == S_OK ? 1:0, m_bListen);
			AM_MEDIA_TYPE *pmt = NULL;
			ALLOCATOR_PROPERTIES ap = {0};
			HRESULT hres = pSample->GetMediaType(&pmt);
			IDump *pDump = NULL;
			m_pDump->QueryInterface(IID_IDump, (void **)&pDump);
			if (pDump)
			{
				pDump->GetAllocatorProperties(&ap);
				pDump->Release();
			}
			rec.SetSyncPoint(true);
			if (SUCCEEDED(hres) && pmt)
			{
				rec.SetLongHeaderData(pmt->majortype,
					pmt->subtype,
					pmt->formattype,
					pmt->lSampleSize,
					pmt->cbFormat,
					pmt->pbFormat,
					TRUE,
			      ap.cBuffers, ap.cbBuffer, ap.cbAlign, ap.cbPrefix);
				DeleteMediaType(pmt);
			}
			if (m_bListen == TRUE)	m_bListen = 2;
		}
		TRACE(_T("\n"));
		if (m_bMergeSamples)
		{
			m_ReceivedSamples.Lock();
			m_ReceivedSamples.AddTail(new CIPCMediaSampleRecord(rec));
			REFERENCE_TIME rtTotal = m_ReceivedSamples.GetTotalReferenceTime();
			m_ReceivedSamples.Unlock();
			int nTotal = UNITS_100NS_TO_MILLISECONDS(rtTotal);
			if (nTotal > m_nSequenceLength)
			{
				FlushReceivedSamples();
			}
		}
		else
		{
			rec.CreateBubble();
			if (pME)
			{
				pME->DoIndicateMediaData(rec, id);
			}
			if (m_bListen)
			{
				OnDecodeMediaData(rec, 0);
			}
		}
	}
	return NOERROR;
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::FlushReceivedSamples()
{
	m_ReceivedSamples.Lock();
	CIPCMediaSampleRecord *pMSR = m_ReceivedSamples.MergeSampleRecords();
//	TRACE(_T("Flushing %d Samples with %d ms\n"), m_ReceivedSamples.GetCount(), UNITS_100NS_TO_MILLISECONDS(m_ReceivedSamples.GetTotalReferenceTime()));
	m_ReceivedSamples.DeleteAll();
	m_ReceivedSamples.Unlock();
	if (pMSR)
	{
		CIPCMediaAudioUnit *pME = NULL;
		if (m_pCIPCMedia && m_pCIPCMedia->IsConnected())
		{
			pME = m_pCIPCMedia;
		}
		else if (m_pCIPCcontrol && m_pCIPCcontrol->IsConnected())
		{
			pME = m_pCIPCcontrol;
		}
		if (pME)
		{
			pME->DoIndicateMediaData(*pMSR, pME->GetEncodingID());
		}
		if (m_bListen)
		{
			OnDecodeMediaData(*pMSR, 0);
		}
		WK_DELETE(pMSR);
	}
}
/////////////////////////////////////////////////////////////////////////////
HRESULT  WINAPI CAudioUnitDlg::ReleaseBufferFunction(void*pData, long lParam)
{
	CAudioUnitDlg *pThis = (CAudioUnitDlg*)lParam;
	
#ifdef TEST_PERFORMANCE_BUFFER_RELEASE
	POSITION posOld, pos = perfmon.GetHeadPosition();
	int nCount=0;
	while (pos)
	{
		posOld = pos;
		CDebugPM *pDPM = (CDebugPM *)perfmon.GetNext(pos);
		if (pDPM->m_dwBuffer == (DWORD)pData)
		{
			TRACE(_T("too long %d\n"), nCount);
			perfmon.RemoveAt(posOld);
			delete pDPM;
			break;
		}
		nCount++;
	}
#endif

	WK_DELETE(pData);
	pThis->PostMessage(WM_SET_DLG_ITEM_INT, IDC_TXT_NO_OF_SAMP, --pThis->m_nNoOfSamples);

	if (pThis->m_bConfirmReceivedSamples)
	{ 
		if (pThis->m_pCIPCcontrol)
		{
			pThis->m_pCIPCcontrol->DoConfirmValues(pThis->m_pCIPCMedia->GetReplyID(), MEDIA_SET_VALUE|CIPC_MEDIA_REQUEST_DECODING, 0, pThis->m_nNoOfSamples);
		}
		if (pThis->m_pCIPCMedia)
		{
			pThis->m_pCIPCMedia->DoConfirmValues(pThis->m_pCIPCMedia->GetReplyID(), MEDIA_SET_VALUE|CIPC_MEDIA_REQUEST_DECODING, 0, pThis->m_nNoOfSamples);
		}
	} 

	return NOERROR;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnStart() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pMC)
	{
		m_ReceivedSamples.Lock();
		if (m_ReceivedSamples.GetCount())
		{
			m_ReceivedSamples.DeleteAll();
			WK_TRACE(_T("Deleting ReceivedSamples in OnStart()\n"));
		}
		m_ReceivedSamples.Unlock();

		UpdateThreshold();
		HRESULT hr;

#ifdef	_USE_AS_MEDIAPLAYER
		if (!theApp.m_sParam.IsEmpty())
		{
			IMediaPosition *pMP = NULL;
			m_pGB->QueryInterface(IID_IMediaPosition, (void **)&pMP);
			if (pMP)
			{
				REFTIME refPos, refDuration;
				pMP->get_Duration(&refDuration);
				refPos = (REFTIME)(refDuration * m_cThreshold.GetPos() / 1000);
				pMP->put_CurrentPosition(refPos);
				pMP->Release();
				m_nPosUpdatetimer = SetTimer(TIMER_UPDATE, 1000, NULL);
			}
		}
#endif // _USE_AS_MEDIAPLAYER
		hr = m_pMC->Run();
		if (FAILED(hr))
		{
			ReportError(HRESULT_ERROR_AT_POS(hr));
			IndicateLastMediaError();
		}
		else
		{
			ControlStartStopBtn(true);
			SetAUIcon(ICON_RUNNING);
			DoConfirmMediaState();
		}
		if (m_pSplitter)
		{
			m_pSplitter->SetSyncSource(NULL);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnStop() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pMC)
	{
#ifdef	_USE_AS_MEDIAPLAYER
		if (m_nPosUpdatetimer)
		{
			KillTimer(m_nPosUpdatetimer);
			m_nPosUpdatetimer = 0;
		}
#endif // _USE_AS_MEDIAPLAYER
		HRESULT hr;
		unsigned int nThreadID;
		HANDLE hThread = (HANDLE) _beginthreadex(NULL, 0, StopThread, (void*)this, 0, &nThreadID);
		DWORD dwResult = WaitForSingleObject(hThread, 4000);
		
		GetExitCodeThread(hThread, (DWORD*)&hr);
		CloseHandle(hThread);

		if (dwResult == WAIT_TIMEOUT)
		{
			ReleaseFilters();
			hr = E_ABORT;
		}

		if (FAILED(hr))
		{
			ReportError(HRESULT_ERROR_AT_POS(hr));
		}
		else
		{
			ControlStartStopBtn(false);
			SetAUIcon(ICON_MAIN);
			DoConfirmMediaState();
			m_AverageLevel.RemoveAll();
		}
		m_ReceivedSamples.Lock();
		if (m_ReceivedSamples.GetCount())
		{
			m_ReceivedSamples.DeleteAll();
			WK_TRACE(_T("Deleting ReceivedSamples in OnStop()\n"));
		}
		m_ReceivedSamples.Unlock();
	}
	m_bLastSampleSyncPoint = false;
}
//////////////////////////////////////////////////////////////////////////
unsigned int WINAPI CAudioUnitDlg::StopThread(void *pParam)
{
	CAudioUnitDlg *pThis = (CAudioUnitDlg*)pParam;
	if (pThis->m_pMC)
	{	
		pThis->m_bLastSampleSyncPoint = false;
		UINT nResult = pThis->m_pMC->Stop();
		return nResult;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnPause() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pMC)
	{
		HRESULT hr;
#ifdef	_USE_AS_MEDIAPLAYER
		if (m_nPosUpdatetimer)
		{
			KillTimer(m_nPosUpdatetimer);
			m_nPosUpdatetimer = 0;
		}
#endif // _USE_AS_MEDIAPLAYER
		hr = m_pMC->Pause();
		if (FAILED(hr))
		{
			ReportError(HRESULT_ERROR_AT_POS(hr));
		}
		else
		{
			ControlStartStopBtn(false);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnBtnEncoderProperty() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pEncode)
	{
		HRESULT hr;
		HR_MSG_BOX(ShowFilterPropertyPage(m_pEncode, m_hWnd));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnBtnSourceProperty() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pCapture)
	{
		HRESULT hr;
		HR_MSG_BOX(ShowFilterPropertyPage(m_pCapture, m_hWnd));
	}
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnBtnRendererProperty() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pRender)
	{
		HRESULT hr;
		HR_MSG_BOX(ShowFilterPropertyPage(m_pRender, m_hWnd));
	}
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnBtnAudioDeviceProperty() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pMediaSampleSource)
	{
		HRESULT hr;
		HR_MSG_BOX(ShowFilterPropertyPage(m_pMediaSampleSource, m_hWnd));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnTimer(UINT nIDEvent) 
{
	if (m_nDwellTimer == nIDEvent)
	{
		int nLowerTresholdVal = 1;
		if (m_nThreshold == 0)
		{
			KillDwellTimer();
			return;
		}
		
		if (m_nAverageLevel > m_nLowerThreshold)
		{
			m_nDwellTimeCount = 0;
			return;
		}
		if (++m_nDwellTimeCount < m_nDwellTime)
		{
			return;
		}

		KillDwellTimer();
		if (m_pSplitterInterface)
		{
			SetAUIcon(ICON_RUNNING);
			SetButtonIcon(IDC_RECORDING, IDC_STOP);
			if (!m_bDeliverAlways)
			{
				if (m_ReceivedSamples.GetCount())
				{
					WK_TRACE(_T("FlushReceivedSamples in Dwell Timer Event\n"));
				}
				FlushReceivedSamples();
			}
			if (!m_bRecordingLevel)
			{
				m_pSplitterInterface->SetMediaSampleCheckFnc(NULL, (LPARAM)this);
			}
			UpdateThreshold();
			if (m_bGenerateAlarm && m_pCIPCInput)
			{
				m_pCIPCInput->SetAudioAlarm(FALSE);
			}
		}
	}
	else if (m_nResetTimer == nIDEvent)
	{
		if (m_pCIPCMedia)
		{
			if (m_bGenerateAlarm && m_pCIPCInput) 
			{
				if (m_pCIPCInput->DoReset() && m_pCIPCMedia->DoReset())
				{
					OnRequestReset();
				}
			}
			else if (m_pCIPCMedia->DoReset())
			{
				OnRequestReset();
			}
		}
	}
	else if (EVENT_REC_LEVEL == nIDEvent)
	{
		m_cRecPeakLevel.SetPos(m_nRecLevel);
		m_cRecLevel.SetPos(m_nAverageLevel);
	}
#ifdef	_USE_AS_MEDIAPLAYER
	else if (m_nPosUpdatetimer == nIDEvent)
	{
		IMediaPosition *pMP = NULL;
		m_pGB->QueryInterface(IID_IMediaPosition, (void **)&pMP);
		if (pMP)
		{
			REFTIME refPos, refDuration;
			pMP->get_CurrentPosition(&refPos);
			pMP->get_Duration(&refDuration);
			m_cThreshold.SetPos((int)(refPos * 1000 / refDuration));
			pMP->Release();
		}
	}
#endif // _USE_AS_MEDIAPLAYER
	CDialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnAppExit() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnSettings() 
{
   if (!m_bStandalone)
	{
		SetWindowPos(&CWnd::wndTop, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		SetForegroundWindow(); 
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::KillDwellTimer()
{
	if (m_nDwellTimer)
	{
		KillTimer(m_nDwellTimer);
		m_nDwellTimer = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnApplyNow() 
{
	if (!theApp.IsReadOnly())
	{
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, m_strKey, "");

		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_STEREO       , m_bStereo);
		wkp.WriteString(AU_AUDIO_SETTINGS, AU_SAMPLE_FRQ   , m_strFrq);
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_TWO_BYTES    , m_b2Bytes);
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_CHANNEL      , m_nStereoChannel);
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_HOLD_TIME    , m_nDwellTime);
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_THRESHOLD    , m_nThreshold);
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_SAMPLE_LENGTH, m_nSequence);
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_INPUT_CHANNEL, m_nInputChannel);
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_AUDIO_QUALITY, m_nQuality);
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_OUTPUT_VOLUME, m_nVolume);
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_OUTPUT_BALANCE, m_nBalance);
		
		wkp.WriteInt(AU_CIPC_SETTINGS, AU_GENERATE_ALARM, m_bGenerateAlarm);

		m_cCapture.GetLBText(m_cCapture.GetCurSel(), m_sCapture);
		wkp.WriteString(AU_DIRECT_SHOW, GetString(CLSID_AudioInputDeviceCategory), m_sCapture);
		m_cCompressors.GetLBText(m_cCompressors.GetCurSel(), m_sCompressor);
		wkp.WriteString(AU_DIRECT_SHOW, GetString(CLSID_AudioCompressorCategory) , m_sCompressor);
		m_cRenderer.GetLBText(m_cRenderer.GetCurSel(), m_sRenderer);
		wkp.WriteString(AU_DIRECT_SHOW, GetString(CLSID_AudioRendererCategory)   , m_sRenderer);
		m_cAudioDevices.GetLBText(m_cAudioDevices.GetCurSel(), m_sAudioDevice);
		wkp.WriteString(AU_DIRECT_SHOW, GetString(KSCATEGORY_AUDIO_DEVICE)       , m_sAudioDevice);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CAudioUnitDlg::PinsToList()
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	int                 n=0, nCount, nIn = CB_ERR, nSel = CB_ERR;
	HRESULT             hr = 0;
	BOOL                bEnable;
	IAMAudioInputMixer *pAMaim = NULL;
	IPin               *pPinIn = NULL;
	DWORD               dwSel=0;
	BOOL                bFirst = FALSE;

	nCount = m_cInputPinList.GetItemCount();
	for (n=0; n<nCount; n++)
	{
		pAMaim = (IAMAudioInputMixer*)m_cInputPinList.GetItemData(n);
		RELEASE_OBJECT(pAMaim);
	}
	m_cInputPinList.DeleteAllItems();

	if (!m_pCapture) return nSel;
	CString sKey, sSection, sDevice;
	sKey = AUDIO_UNIT;
	sKey = WK_PROFILE_ROOT + sKey + _T("\\") + SEC_MEDIA_TYPE_INPUT;
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, sKey, "");
	m_cCapture.GetLBText(m_cCapture.GetCurSel(), sDevice);

	while (SUCCEEDED(hr))
	{
		hr = GetPin(m_pCapture, PINDIR_INPUT, n++, &pPinIn);		// Query pins of the capture and the wave splitter device
		if (SUCCEEDED(hr))
		{
			PIN_INFO pi;
			hr = pPinIn->QueryPinInfo(&pi);								// Query Pin Info
			CString str;
			if (SUCCEEDED(hr))
			{
				str = CString(pi.achName);
				sSection = sDevice + _T("\\") + str;
				nIn = m_cInputPinList.InsertItem(n, str);
			}
			else
			{
				ReportError(HRESULT_ERROR_AT_POS(hr));
			}
			RELEASE_OBJECT(pi.pFilter);

			hr = pPinIn->QueryInterface(IID_IAMAudioInputMixer, (void**)&pAMaim);
			if (SUCCEEDED(hr))												// Query Input Mixer
			{
				bEnable = FALSE;
				m_cInputPinList.SetItemData(nIn, (DWORD)pAMaim);
				if (wkp.GetInt(sSection, AU_STEREO, -1) == -1 && !theApp.IsReadOnly())		// not created?
				{
					BOOL bStereo = FALSE;
					double dVal;
					hr = pAMaim->get_Pan(&dVal);
					if (SUCCEEDED(hr))
					{
						bStereo = TRUE; 
					}
					wkp.WriteInt(sSection, AU_STEREO , bStereo);		// create
					wkp.WriteInt(sSection, AU_CHANNEL, nIn);
					if (_tcsncmp(str, AU_DEFAULT_INPUT, 2) == 0)
					{
						pAMaim->put_Enable(TRUE);
					}
					bFirst = TRUE;
				}
				hr = pAMaim->get_Enable(&bEnable);						// the selected Pin
				if (SUCCEEDED(hr))
				{
					if (bEnable)
					{
						m_cInputPinList.SetCheck(nIn);
						dwSel |= (1<<nIn);
					}
				}
				else
				{
					ReportError(HRESULT_ERROR_AT_POS(hr), false);
				}
				pAMaim = NULL;
			}
			else
			{
				ReportError(HRESULT_ERROR_AT_POS(hr));
			}
/*
			else
			{
				IKsPin* pKsPin = NULL;
				KSMULTIPLE_ITEM *pmi = NULL;
				HRESULT hr = pPinIn->QueryInterface(IID_IKsPin, (void **)&pKsPin);
				if (SUCCEEDED(hr))
				{
					hr = pKsPin->KsQueryMediums(&pmi);
					pKsPin->Release();
				}
				else
				{
					ReportError(HRESULT_ERROR_AT_POS(hr));
				}
				if (SUCCEEDED(hr) && (pmi != NULL) && pmi->Count)
				{
					REGPINMEDIUM *pMedium = (REGPINMEDIUM*)(pmi + 1);

					for (ULONG i = 0; i < pmi->Count; i++, pMedium++) 
					{
					}
					CoTaskMemFree(pmi);
				}
			}
*/
			RELEASE_OBJECT(pAMaim);
			hr = 0;
		}
		RELEASE_OBJECT(pPinIn);
	};

	if (dwSel & (1<<m_nInputChannel))
	{
		nSel = m_cInputPinList.SetItemState(m_nInputChannel, LVIS_SELECTED, LVIS_SELECTED);
	}
	else if (dwSel)
	{
		nCount = m_cInputPinList.GetItemCount();
		for (n=0; n<nCount; n++)
		{
			if (dwSel & (1<<n))
			{
				m_nInputChannel = n;
				pAMaim = (IAMAudioInputMixer*)m_cInputPinList.GetItemData(m_nInputChannel);
				if (pAMaim) hr = pAMaim->put_Enable(TRUE);
				m_cInputPinList.SetCheck(m_nInputChannel);
			}
		}
	}
	else // dwSel == 0
	{
		nCount = m_cInputPinList.GetItemCount();
		if (nCount)
		{
			m_nInputChannel = 0;
			pAMaim = (IAMAudioInputMixer*)m_cInputPinList.GetItemData(m_nInputChannel);
			if (pAMaim) hr = pAMaim->put_Enable(TRUE);
			m_cInputPinList.SetCheck(m_nInputChannel);
		}
	}
	if (IsBetween(m_nInputChannel, 0, m_cInputPinList.GetItemCount()))
	{
		m_cInputPinList.EnsureVisible(m_nInputChannel, FALSE);
	}
	long lValue = m_nInputChannel;
	OnClickListInputPins(NULL, &lValue);
	
	if (bFirst && m_nInputChannel != -1 && !theApp.IsReadOnly())
	{
		CString str = m_cInputPinList.GetItemText(m_nInputChannel, 0);
		sSection = sDevice + _T("\\") + str;
//		wkp.WriteInt(sSection, AU_ACTIVE, 1);				// activate left channel
		wkp.WriteString(sSection, AU_INPUT_NAME, str);  // Set the name
		sSection  = WK_PROFILE_ROOT;
		sSection += AUDIO_UNIT;
		CWK_Profile wkpI(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, sSection, "");
		sDevice = AU_CHANNEL;
		sDevice += (_TCHAR)((int)'0' + theApp.GetInstanceCount());
		wkpI.WriteInt(SEC_MEDIA_TYPE_INPUT, sDevice, m_cInputPinList.GetItemCount());// No of Channels

		CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, m_strKey, "");
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_INPUT_CHANNEL, m_nInputChannel);
	}

	return nSel;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnClickListInputPins(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HRESULT             hr = 0;
	LVHITTESTINFO       lvHti;
	IAMAudioInputMixer *pAMaim = NULL;
	int                 n=0, nCount;
	BOOL                bEnable;
	GetCursorPos(&lvHti.pt);

	if (pNMHDR && (pNMHDR->hwndFrom == m_cInputPinList.m_hWnd))
	{
		m_cInputPinList.ScreenToClient(&lvHti.pt);
		m_cInputPinList.HitTest(&lvHti);
	}
	else
	{
		lvHti.iItem = *pResult;
		if (IsBetween(lvHti.iItem, 0, m_cInputPinList.GetItemCount()-1))
		{
			lvHti.flags = LVHT_ONITEMLABEL|LVHT_ONITEMSTATEICON;
		}
		else
		{
			lvHti.flags = 0;
		}
	}

	if (lvHti.flags & (LVHT_ONITEMSTATEICON))
	{
		pAMaim = (IAMAudioInputMixer*)m_cInputPinList.GetItemData(lvHti.iItem);
		if (pAMaim)
		{
			bEnable = TRUE;
			hr = pAMaim->put_Enable(bEnable);
			if (SUCCEEDED(hr))
			{
				nCount = m_cInputPinList.GetItemCount();
				for (n=0; n<nCount; n++)
				{
					pAMaim = (IAMAudioInputMixer*)m_cInputPinList.GetItemData(n);
					hr = pAMaim->get_Enable(&bEnable);					// the selected Pin
					if (pNMHDR) PostMessage(WM_USER, n, bEnable);
					else        m_cInputPinList.SetCheck(n, bEnable);
				}
			}
			m_nInputChannel = lvHti.iItem;
			m_cInputPinList.EnsureVisible(m_nInputChannel, FALSE);
			m_cInputPinList.SetSelectionMark(m_nInputChannel);
		}
	}
	if (lvHti.flags & (LVHT_ONITEMSTATEICON|LVHT_ONITEMLABEL|LVHT_ONITEMICON))
	{
		pAMaim = (IAMAudioInputMixer*)m_cInputPinList.GetItemData(lvHti.iItem);
		if (pAMaim)
		{
			double dLevel;
			BOOL bStereo;
			hr = pAMaim->get_MixLevel(&dLevel);
			if (SUCCEEDED(hr))
			{
				m_cRecordingLevel.EnableWindow();
				m_cRecordingLevel.SetPos((int)((1-dLevel)*1000));
			}
			else
			{
				m_cRecordingLevel.EnableWindow(FALSE);
			}
			hr = pAMaim->get_Pan(&dLevel);
			if (SUCCEEDED(hr))
			{
				bStereo = TRUE;
			}
			else
			{
				bStereo = FALSE;
			}
			GetDlgItem(IDC_CK_STEREO)->EnableWindow(bStereo);
			GetDlgItem(IDC_RD_RIGHT)->EnableWindow(bStereo);
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnReleasedcaptureSliderThreshold(NMHDR* pNMHDR, LRESULT* pResult) 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	m_nThreshold = m_cThreshold.GetPos();
	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDT_THRESHOLD, m_nThreshold);
	UpdateThreshold();
	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnReleasedcaptureSliderRecordingLevel(NMHDR* pNMHDR, LRESULT* pResult) 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	int nSel = m_cInputPinList.GetSelectionMark();
	if (nSel != -1)
	{
		IAMAudioInputMixer*pAMaim = (IAMAudioInputMixer*)m_cInputPinList.GetItemData(nSel);
		double dLevel = (1000-m_cRecordingLevel.GetPos()) / 1000.0;
		pAMaim->put_MixLevel(dLevel);
	}

	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnReleasedcaptureSliderVolume(NMHDR* pNMHDR, LRESULT* pResult) 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	m_nVolume = m_cVolume.GetPos();
	SetOutputParam();
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnKillfocusEdtThreshold() 
{
	CDataExchange dx(this, TRUE);
	DDX_Text(&dx, IDC_EDT_THRESHOLD, m_nThreshold);
	m_cThreshold.SetPos(m_nThreshold);
	UpdateThreshold();
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnCkRecordingLevel() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
//	OnChange();
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_RECORDING_LEVEL, m_bRecordingLevel);
	if (m_pSplitterInterface)
	{
		m_pSplitterInterface->SetMediaSampleCheckFnc(m_bRecordingLevel ? CheckMediaSample : NULL, (LPARAM)this);
	}
	m_cRecPeakLevel.ShowWindow(m_bRecordingLevel ? SW_SHOW: SW_HIDE);
	m_cRecLevel.ShowWindow(m_bRecordingLevel ? SW_SHOW: SW_HIDE);
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnCkGenerateAlarm() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_GENERATE_ALARM, m_bGenerateAlarm);
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnCkListen() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_LISTEN, m_bListen);
	if (m_bListen)
	{

	}
	else 
	{
		if (m_ReceivedSamples.GetCount())
		{
			WK_TRACE(_T("FlushReceivedSamples in OnCkListen\n"));
		}
		FlushReceivedSamples();
		if (m_bRendering)
		{
			m_bRendering = FALSE;
			DoConfirmMediaState();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnSelchangeComboFrq() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	CDataExchange dx(this, TRUE);
	CString strFrq;
	DDX_CBString(&dx, IDC_COMBO_FRQ, strFrq);
	if (m_strFrq != strFrq)
	{
		m_strFrq = strFrq;
		m_nQuality = CB_ERR;
		dx.m_bSaveAndValidate = FALSE;
		DDX_CBIndex(&dx, IDC_COMBO_QUALITY, m_nQuality);
		if (m_pCIPCcontrol == NULL)
		{
			OnChange();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnSelchangeComboQuality() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	int nOld = m_nQuality;
	m_nQuality = m_cQuality.GetCurSel();
	if (m_pEncode && (m_nQuality != nOld))
	{
		IPin *pPinOut = NULL;
		HRESULT hr = GetPin(m_pEncode, PINDIR_OUTPUT, 0, &pPinOut);		// Query pins of the encoder device and the dump device
		if (pPinOut)
		{
			IAMStreamConfig *pCfg = NULL;
			hr = pPinOut->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
			if (SUCCEEDED(hr))
			{
				hr = pCfg->SetFormat((AM_MEDIA_TYPE*)m_cQuality.GetItemData(m_nQuality));
				if (SUCCEEDED(hr))
				{
					AM_MEDIA_TYPE *pmt={0};
					hr = pCfg->GetFormat(&pmt);								// Read current media type/format
					if (SUCCEEDED(hr))
					{
						CDataExchange dx(this, FALSE);
						WAVEFORMATEX  *pWF = (WAVEFORMATEX *) pmt->pbFormat;
						m_nBytesPerSecond = pWF->nAvgBytesPerSec;			// request the average bytes per second
						DDX_Text(&dx, IDC_EDT_BYTES_PER_SECOND, m_nBytesPerSecond);
					}
					DeleteMediaType(pmt);
				}
				else
				{
					OnChange();
				}
				RELEASE_OBJECT(pCfg);
			}
			RELEASE_OBJECT(pPinOut);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
long CAudioUnitDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, (WPARAM)theApp.GetApplicationId(), 0);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::SetNewEncodingSegent()
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pDump)
	{
		IPin *pPin = NULL;
		GetPin(m_pDump, PINDIR_INPUT, 0, &pPin);
		if (pPin)
		{
			pPin->NewSegment(0, 0, 0);
			RELEASE_OBJECT(pPin);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::UpdateThreshold()
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pSplitterInterface)
	{ 
//		WK_TRACE(_T("UpdateThreshold()\n"));
		ASSERT(GetCurrentThreadId() == theApp.GetMainTreadID());
		if (m_nThreshold >= 15)
		{
			m_nLowerThreshold = m_nThreshold - 10; // 10 % unter dem Einschaltwert
		}
		else if (m_nThreshold >= 2)
		{
			m_nLowerThreshold = m_nThreshold / 2;
		}
		m_pSplitterInterface->SetThreshold((float)m_nThreshold);

		if (m_bDeliverAlways)
		{
			m_pSplitterInterface->SetDeliver(WAVESPLIT_ALWAYS);
		}
		else
		{
			bool bOnlyDetection = m_bNoCompression || !m_bCapturing;
			m_pSplitterInterface->SetDeliver(bOnlyDetection ? WAVESPLIT_NEVER : WAVESPLIT_IF_ACTIVE);
		}
		CDataExchange dx(this, FALSE);
		DDX_Check(&dx, IDC_CK_DELIVER_ALWAYS, m_bDeliverAlways);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CDialog::OnLButtonDown(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnDeviceChange(WPARAM wParam, LPARAM lParam)
{
	if (lParam)
	{
		DEV_BROADCAST_HDR *pDBH = (DEV_BROADCAST_HDR*)lParam;
		if (pDBH->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE_EX)
		{
			CDevIntEx diex((DEV_BROADCAST_DEVICEINTERFACE_EX*)pDBH);
			if (    ((wParam == DBT_DEVICEARRIVAL) || (wParam == DBT_DEVICEREMOVECOMPLETE))
				  && (diex.GetGuidClass() == AM_KSCATEGORY_AUDIO))
			{
				int nOldCaptureCount  = m_cCapture.GetCount(),
					 nOldRendererCount = m_cRenderer.GetCount();

				ClearFilterListWithMoniker(m_cCapture);
				EnumFiltersWithMonikerToList(NULL, &CLSID_AudioInputDeviceCategory, &m_cCapture);

				if (!SetComboBoxSelection(m_cCapture, m_sCapture))
				{
					if (m_pCapture)
					{
						OnStop();
						ReleaseFilters(false);
						if (m_pCIPCMedia)
						{
							m_pCIPCMedia->DoIndicateError(CIPC_MEDIA_INDICATE_DATA, m_pCIPCMedia->GetSecID(AUDIO_INPUT), CIPC_ERROR_NO_HARDWARE, 0);
						}
					}
				}
				
				ClearFilterListWithMoniker(m_cRenderer);
				if (!EnumRenderers(NULL))
				{
					if (m_pRender)
					{
						OnStop();
						ReleasePlayFilters();
						if (m_pCIPCMedia)
						{
							m_pCIPCMedia->DoIndicateError(CIPC_MEDIA_INDICATE_DATA, m_pCIPCMedia->GetSecID(AUDIO_OUTPUT), CIPC_ERROR_NO_HARDWARE, 0);
						}
					}
				}

				WPARAM wUpdate = 0;
				CWK_Profile wkp;
				if (nOldCaptureCount != m_cCapture.GetCount())
				{
					wkp.WriteInt(AUDIO_UNIT, AU_CAPTURERS, m_cCapture.GetCount());
					if (m_pCIPCcontrol)
					{
						OnRequestHardware(m_pCIPCcontrol->GetSecID(AUDIO_INPUT).GetID() , MAKELONG(MEDIA_IDR_AUDIOINPUTDEVICES, USE_CIPCCONTROL));
					}
				}
				else if (nOldRendererCount != m_cRenderer.GetCount())
				{
					wkp.WriteInt(AUDIO_UNIT, AU_RENDERERS, m_cRenderer.GetCount());
					if (m_pCIPCcontrol)
					{
						OnRequestHardware(m_pCIPCcontrol->GetSecID(AUDIO_OUTPUT).GetID(), MAKELONG(MEDIA_IDR_AUDIO_RENDERERS  , USE_CIPCCONTROL));
					}
				}
				wkp.FlushKey(AUDIO_UNIT);
			}
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnInputListSetCheck(WPARAM wParam, LPARAM lParam)
{
	m_cInputPinList.SetCheck(wParam, lParam);
	return 0;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnWinthreadStopThread(WPARAM wParam, LPARAM lParam)
{
	if (wParam == TRUE)
	{
		CString str = SM_AudioUnitMedia;
		str.Format(SM_AudioUnitMedia, theApp.GetInstanceCount());
		str += _T("Control");
		m_pCIPCcontrol = new CIPCMediaAudioUnit(str, FALSE, this);
		return wParam;
	}
	else if (wParam == FALSE)
	{
		WK_DELETE(m_pCIPCcontrol);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnTriggerUnit(WPARAM wParam, LPARAM lParam)
{
	OnStop();
	OnStart();
	WK_TRACE(_T("OnTriggerUnit(%d, %d)"), wParam, lParam);
	return 0;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnSetDlgItemInt(WPARAM wID, LPARAM lValue)
{
	SetDlgItemInt(wID, lValue);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnRequestStartMediaEncoding(WPARAM dwFlags, LPARAM )
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	bool bStart      = (dwFlags & MEDIA_ENC_ONLY_INITIALIZE) ? false : true;
	bool bShowState  = (dwFlags & MEDIA_ENC_DONT_SHOW_STATE) ? false : true;
	bool bOldCapturing = m_bCapturing;

	if (m_btnInitAudioRec.IsWindowEnabled())
	{
		OnInitAudioRecord();
	}

	if (!(dwFlags & MEDIA_ENC_DONT_CHANGE_STATES))
	{
		m_bDeliverAlways = (dwFlags & MEDIA_ENC_DELIVER_ALWAYS)  ? TRUE  : FALSE;
		m_bNoCompression = (dwFlags & MEDIA_ENC_NO_COMPRESSION)  ? true  : false;
	}

	if (dwFlags & MEDIA_ENC_NEW_SEGMENT)
	{
		SetNewEncodingSegent();
	}
	
	if (bShowState && bStart)
	{
		m_bCapturing = true;
	}
	
	if (bStart && m_btnStart.IsWindowEnabled())
	{
		OnStart();
	}
	else
	{
		UpdateThreshold();
		if (bOldCapturing != m_bCapturing)
		{
			DoConfirmMediaState(CIPC_MEDIA_REQUESTSTART_ENCODING);
		}
	}

	if (dwFlags & MEDIA_ENC_ONLY_INITIALIZE)
	{
		DoConfirmMediaState(CIPC_MEDIA_REQUESTSTART_ENCODING);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnRequestStopMediaEncoding(WPARAM dwFlags, LPARAM)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	bool bDelete = false;
	if (dwFlags == 1)
	{
		ReleaseFilters();
		bDelete = true;
	}
	else
	{
		if (m_bGenerateAlarm && m_pCIPCInput && m_pCIPCInput->IsAudioDetectorActive())
		{
			m_bDeliverAlways = FALSE;
			UpdateThreshold();
			bDelete = true;
		}
		else if (m_btnStop.IsWindowEnabled() && !m_bRendering)
		{
			m_bCapturing = false;
			StopThread(this);
			ControlStartStopBtn(false);
			SetAUIcon(ICON_MAIN);
			DoConfirmMediaState();
			m_AverageLevel.RemoveAll();
			bDelete = true;
		}
	}

	m_ReceivedSamples.Lock();
	if (bDelete && m_ReceivedSamples.GetCount())
	{
		m_ReceivedSamples.DeleteAll();
	}
	m_ReceivedSamples.Unlock();

	return 0;
}
//////////////////////////////////////////////////////////////////////////
BOOL CAudioUnitDlg::InitDecodeMediaData()
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	WK_TRACE(_T("InitDecodeMediaData\n"));

	if (m_btnStop.IsWindowEnabled())
	{
		OnStop();
	}

	ReleasePlayFilters();

	OnInitAudioPlay();

	if (m_pIPushSource)
	{
		CString sFormat, sMajor, sSub;
		GetGUIDString(sMajor , &m_MediaType.majortype , TRUE);
		GetGUIDString(sSub   , &m_MediaType.subtype   , TRUE);
		GetGUIDString(sFormat, &m_MediaType.formattype, TRUE);
		sFormat = sMajor + _T(", ") + sSub + _T(", ") + sFormat;
		SetDlgItemText(IDC_TXT_MEDIA_TYPE, sFormat);
		WK_TRACE(_T(": %s\n"), sFormat);
		OnStart();
	}
	else
	{
		IndicateLastMediaError();
	}

	return m_pIPushSource != NULL;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnRequestStopMediaDecoding(WPARAM dwFlags, LPARAM)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	TRACE(_T("OnRequestStopMediaDecoding\n"));
	m_bRendering = false;
	SetButtonIcon(IDC_PLAYING, IDC_STOP);

/*
	if (m_btnStop.IsWindowEnabled())
	{
		OnStop();
		if (   m_bCapturing 
			|| (m_bGenerateAlarm && m_pCIPCInput && m_pCIPCInput->IsAudioDetectorActive()))
		{
			OnStart();
		}
	}
	DoConfirmMediaState(CIPC_MEDIA_REQUESTSTOP_DECODING);
*/
	if (theApp.m_bFreePushSourceBuffers)
	{
		if (m_pIPushSource)
		{
			HRESULT hr = m_pIPushSource->SetBuffer(NULL);
			if (hr != 0)
			{
				WK_TRACE(_T("Removed Buffers: %d\n"), hr);
			}
		}
	}
	if (   m_bGenerateAlarm && m_pCIPCInput && m_pCIPCInput->IsAudioDetectorActive() 
		&& !m_bCapturing)
	{
		m_bDeliverAlways = FALSE;
		UpdateThreshold();
	}
	else if (m_btnStop.IsWindowEnabled() && !m_bCapturing)
	{
		OnStop();
		return 0;
	}
	DoConfirmMediaState(CIPC_MEDIA_REQUESTSTOP_DECODING);

	return 0;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnRequestHardware(WPARAM wParam, LPARAM lParam)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	CIPCMediaAudioUnit *pCIPC = NULL;
	if      (HIWORD(lParam) == USE_CIPCCONTROL) pCIPC = m_pCIPCcontrol;
	else if (HIWORD(lParam) == USE_CIPCMEDIA  ) pCIPC = m_pCIPCMedia;

	if (pCIPC)
	{
		int   nCompressors = m_cCompressors.GetCount(),
				nInputDevs   = m_cCapture.GetCount(),
				nRenderers   = m_cRenderer.GetCount(),
				iError       = 0;
		DWORD dwFlags, *pdwSelect = &dwFlags, dwIDRequest = LOWORD(lParam);
		CSecID secID((DWORD)wParam);

		CIPCExtraMemory *pEM = NULL;
		CComboBox       *pCB = NULL;
		
		CIPCMediaRecord::MediaRecordFlags Flags;
		if (dwIDRequest == 0)
		{
			Flags.Reset();
			if (nInputDevs > 0) Flags.IsInput  = true;
			if (nRenderers > 0) Flags.IsOutput = true;
			if (Flags  == 0)
			{
				iError    = CIPC_ERROR_NO_HARDWARE;
			}
			else
			{
				Flags.IsOkay               = true;
				Flags.IsAudio              = true;
				GetStateFlags(Flags);
			}
			dwFlags = Flags;
		}

		WORD wGroupID = secID.GetGroupID();
		ASSERT(wGroupID == pCIPC->GetSecID(0).GetGroupID());

		switch (dwIDRequest)
		{
			case MEDIA_IDR_AUDIOCOMPRESSORS:
			if (nCompressors)
			{
				pCB = &m_cCompressors;
			}break;
			
			case MEDIA_IDR_AUDIOINPUTDEVICES:
			if (nInputDevs)
			{
				pCB = &m_cCapture;
			}break;

			case MEDIA_IDR_AUDIO_RENDERERS:
			if (nRenderers)
			{
				pCB = &m_cRenderer;
				CString sKey, sName, sDevice, sChannels;
				sKey = AUDIO_UNIT;
				sKey = WK_PROFILE_ROOT + sKey + _T("\\");
				CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, sKey, "");
				m_cRenderer.GetLBText(m_cRenderer.GetCurSel(), sName);
				
				sChannels.Format(_T("%s%d"), AU_CHANNEL, theApp.GetInstanceCount());
				int nChannels = wkp.GetInt(SEC_MEDIA_TYPE_OUTPUT, sChannels, -1);
				if (nChannels == -1 && !theApp.IsReadOnly())
				{
					wkp.WriteInt(SEC_MEDIA_TYPE_OUTPUT, sChannels, m_cRenderer.GetCount());
				}
				
				sDevice.Format(_T("%s\\%s\\Audio"), SEC_MEDIA_TYPE_OUTPUT , sName);
				sName = wkp.GetString(sDevice, AU_OUTPUT_NAME, NULL);
				if (sName.IsEmpty() && !theApp.IsReadOnly())
				{
					wkp.WriteInt(sDevice, AU_CHANNEL, 0);
					wkp.WriteInt(sDevice, AU_STEREO, 0);
					wkp.WriteInt(sDevice, AU_ACTIVE, 0);
				}
			}break;
			case MEDIA_IDR_INPUT_PINS:
			if (m_pCapture == NULL)
			{
				OnInitAudioRecord();
			}
			if (m_cInputPinList.GetItemCount())
			{
				int          i, nSel = -1, nCount = m_cInputPinList.GetItemCount();
				CStringArray sa;
				CString      str;
				pEM = new CIPCExtraMemory;
				for (i=0; i<nCount; i++)
				{
					str = m_cInputPinList.GetItemText(i, 0);
					sa.Add(str);
					if (m_cInputPinList.GetCheck(i)) nSel = i;
				}
				*pdwSelect = nSel;
				pEM->Create(pCIPC, sa);
			}break;
			case MEDIA_IDR_TEST_CONFIG:
				iError = TestConfig(dwIDRequest) ? 0 : CIPC_ERROR_NO_HARDWARE;
				break;
			case 0:	// 
			{
				pEM = new CIPCExtraMemory;
				CStringArray   sa;
				sa.Add(m_sCapture);    // InputDevice
				sa.Add(m_sCompressor); // Compressor
				sa.Add(m_sRenderer);   // Renderer
				pEM->Create(pCIPC, sa);
			}break;
		}

		if (pCB)
		{
			CStringArray   sa;
			CString        s;
			pEM = new CIPCExtraMemory;
			int i, nCount = pCB->GetCount();
			for (i=0; i<nCount; i++) 
			{
				pCB->GetLBText(i, s);
				sa.Add(s);
			}
			*pdwSelect = pCB->GetCurSel();
			pEM->Create(pCIPC, sa);
		}

		pCIPC->DoConfirmHardware(secID, iError, dwFlags, dwIDRequest, pEM);
		WK_DELETE(pEM);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnRequestAudioQualities(WPARAM wParam, LPARAM lParam)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	CIPCMediaAudioUnit *pCIPC = NULL;
	WORD wUsage                 = LOWORD(lParam);
	BOOL bReleaseFilters        = HIWORD(lParam) & 1 ? true : false;
	if      (wUsage == USE_CIPCCONTROL) pCIPC = m_pCIPCcontrol;
	else if (wUsage == USE_CIPCMEDIA  ) pCIPC = m_pCIPCMedia;

	if (pCIPC)
	{
		if (bReleaseFilters)
		{
			ReleaseFilters();
		}
		CIPCExtraMemory *pReturnData = NULL;
		if (m_pEncode == NULL)
		{
			OnInitAudioRecord();
		}
		int i, nCount = m_cQuality.GetCount();
		if (nCount > 0)
		{
			pReturnData = new CIPCExtraMemory;
			CStringArray sa;
			CString str;
			for (i=0; i<nCount; i++)
			{
				m_cQuality.GetLBText(i, str);
				sa.Add(str);
			}
			pReturnData->Create(pCIPC, sa);
		}
		pCIPC->DoConfirmValues(CSecID((DWORD)wParam), MEDIA_GET_VALUE|MEDIA_AUDIO_QUALITYS, MEDIA_IDR_AUDIOCOMPRESSORS, m_cQuality.GetCurSel(), pReturnData);
		WK_DELETE(pReturnData);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnRequestSetSampleFrequency(WPARAM wParam, LPARAM lParam)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	CIPCMediaAudioUnit *pCIPC = NULL;
	if      (lParam == USE_CIPCCONTROL) pCIPC = m_pCIPCcontrol;
	else if (lParam == USE_CIPCMEDIA  ) pCIPC = m_pCIPCMedia;

	WPARAM nSel =SendDlgItemMessage(IDC_COMBO_FRQ, CB_GETCURSEL, 0, 0);
	if (pCIPC && nSel != wParam)
	{
		SendDlgItemMessage(IDC_COMBO_FRQ, CB_SETCURSEL, wParam, 0);
		SendMessage(WM_COMMAND, MAKELONG(IDC_COMBO_FRQ, CBN_SELCHANGE), (LPARAM)::GetDlgItem(m_hWnd, IDC_COMBO_FRQ));
		OnInitAudioRecord();
		OnRequestValues(pCIPC, pCIPC->GetSecID(AUDIO_INPUT), MEDIA_GET_VALUE|MEDIA_AUDIO_QUALITYS, MEDIA_IDR_AUDIOCOMPRESSORS, 0, NULL);
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnDecodeMediaData(const CIPCMediaSampleRecord&rec, DWORD dwFlags)
{
	CIPCMediaSampleRecord *pRec = new CIPCMediaSampleRecord(rec);
	m_MediaSamples.SafeAddTail(pRec);
	PostMessage(WM_DECODEMEDIADATA, (LPARAM)pRec, dwFlags);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnDecodeMediaData(WPARAM wParam, LPARAM lParam)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	

	DWORD dwFlags = (DWORD)lParam;
	m_MediaSamples.Lock();
	CIPCMediaSampleRecord *pRec = (CIPCMediaSampleRecord*)wParam;
	POSITION pos = m_MediaSamples.Find(pRec);
	if (pos)
	{
		m_MediaSamples.RemoveAt(pos);
	}
	m_MediaSamples.Unlock();

	m_bConfirmReceivedSamples = (dwFlags & MEDIA_DEC_CONFIRM_RECEIVING) ? true : false;
	
	HRESULT hr = NOERROR;
	int nPNo = pRec->GetPackageNumber();
	bool bOldRendering = m_bRendering;
	TEST_TRACE(_T("RNo:%d, Sz:%d, LH:%d, SP:%d\n"), nPNo, pRec->GetActualLength(), pRec->IsLongHeader(), pRec->GetIsSyncPoint());

	if (pRec->IsLongHeader())		// Bei LongHeader evtl. neu initialisieren
	{
		long lBuffer = m_AP.cbBuffer;
		ALLOCATOR_PROPERTIES ap;
		pRec->GetAllocatorProperties(ap.cBuffers, ap.cbBuffer, ap.cbAlign, ap.cbPrefix);
		bool bChanged =   (m_MediaType.majortype            != pRec->GetMajorType())  
							|| (m_MediaType.subtype              != pRec->GetSubType())
							|| (m_MediaType.formattype           != pRec->GetFormatType())
							|| (m_MediaType.bFixedSizeSamples    != pRec->GetFixedSizeSamples())
							|| (m_MediaType.bTemporalCompression != pRec->GetTemporalCompression())
							|| (m_MediaType.lSampleSize          != pRec->GetSampleSize())
							|| (m_MediaType.cbFormat             != pRec->GetFormatSize())
							|| (lBuffer                          != ap.cbBuffer);

		if (bChanged)
		{
			FreeMediaType(m_MediaType);
			m_MediaType.pbFormat = NULL;
			m_MediaType.majortype            = pRec->GetMajorType();
			m_MediaType.subtype              = pRec->GetSubType();
			m_MediaType.formattype           = pRec->GetFormatType();
			m_MediaType.bFixedSizeSamples    = pRec->GetFixedSizeSamples();
			m_MediaType.bTemporalCompression = pRec->GetTemporalCompression();
			m_MediaType.lSampleSize          = pRec->GetSampleSize();
			m_MediaType.cbFormat             = pRec->GetFormatSize();
			m_MediaType.pbFormat = (PBYTE)CoTaskMemAlloc(m_MediaType.cbFormat);
			m_MediaType.pUnk = NULL;
			memcpy(m_MediaType.pbFormat, pRec->GetFormat(), m_MediaType.cbFormat);

			m_AP = ap;
			m_AP.cBuffers = 1;

			WK_TRACE(_T("Initialize Media Decoder\n"));
			InitDecodeMediaData();
		}
		m_bHeaderRequested = false;
	}
	else if (m_pIPushSource == NULL)								// Packetnummer > 0 und nicht initialisiert ?
	{
		if (m_MediaType.pbFormat)
		{
			WK_TRACE(_T("ReInitialize Media Decoder\n"), nPNo);
			InitDecodeMediaData();
		}
		else if (m_pCIPCMedia && !m_bHeaderRequested)
		{
			WK_TRACE(_T("No Format to initialize available\n"), nPNo);
			m_pCIPCMedia->DoRequestValues(m_pCIPCMedia->GetReplyID(), MEDIA_GET_VALUE|MEDIA_ENC_NEW_SEGMENT, m_pCIPCMedia->GetSecID(AUDIO_OUTPUT).GetID());
			m_bHeaderRequested = true;
		}
	}
	
	if (pRec->ContainsData()) 
	{
		m_bRendering = true;
		PushSourceProperties msp;
		pRec->GetReferenceTimes(&msp.rtTimeStart, &msp.rtTimeEnd);
		pRec->GetMediaTimes(&msp.llTimeStart, &msp.llTimeEnd);
	#ifdef _DEBUG
		CSystemTime st;
		pRec->GetMediaTime(st);
	#endif
		msp.bIsPreroll       = pRec->GetIsPreroll();					// Media properties
		msp.bIsDiscontinuity = pRec->GetIsDiscontinuty();
		msp.bIsSyncPoint     = pRec->GetIsSyncPoint();
		msp.nLength          = pRec->GetActualLength();				// Length
		msp.pBuffer          = pRec->DetachData();					// Detached data is freed in ReleaseBufferFunction
		msp.nSampleNumber    = pRec->GetPackageNumber();

		if (m_pIPushSource)													// neuen Puffer setzen
		{
#ifdef TEST_PERFORMANCE_BUFFER_RELEASE
			CString str;
			str.Format(_T("Buffer %x"), msp.pBuffer);
			perfmon.AddTail((void*) new CDebugPM(str, (DWORD)msp.pBuffer));
#endif
			SetDlgItemInt(IDC_TXT_NO_OF_SAMP, ++m_nNoOfSamples);
			
			hr = m_pIPushSource->SetBuffer(&msp);

			if (FAILED(hr))
			{
				ReportError(HRESULT_ERROR_AT_POS(hr));
			}

			if (m_btnStart.IsWindowEnabled())
			{
				WK_TRACE(_T("Starting again\n"));
				OnStart();
			}
		}
	}
	if (bOldRendering != m_bRendering)
	{
		DoConfirmMediaState();
	}
	
	WK_DELETE(pRec);
	return 0;
}
//////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CAudioUnitDlg
 Function   : OnRequestSetValue
 Description: Sets Values of the Unit

 Parameters:   
  secID       : (E): Security ID of the Unit  (CSecID)
  sKey        : (E): Key number (Cmd ID)  (const CString&)
  sValue      : (E): Valuestring  (const CString&)
  dwServerData: (E): unused  (DWORD)

 Result type:  (void)
 created: September, 10 2003
 <TITLE OnRequestSetValue>
*********************************************************************************************/
void CAudioUnitDlg::OnRequestSetValue(CSecID secID, const CString &sKey, const CString &sValue, DWORD dwServerData)
{
	if (sKey == _T("ReleaseFilters"))
	{
		PostMessage(WM_COMMAND, ID_DBG_RELEASE_FILTERS);
	}
	else
	{
		int nFind = sKey.Find(AU_DIRECT_SHOW);
		if (nFind != -1 && !theApp.IsReadOnly())
		{
			CString strKey;
			strKey.Format(AUDIO_UNIT_DEVICE, theApp.GetInstanceCount());
			strKey = WK_PROFILE_ROOT + strKey;
			CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, strKey, "");
			strKey = sKey.Mid(nFind+_tcslen(AU_DIRECT_SHOW)+1);
			switch (_ttoi(strKey))
			{
				case MEDIA_IDR_AUDIOINPUTDEVICES:
					wkp.WriteString(AU_DIRECT_SHOW, GetString(CLSID_AudioInputDeviceCategory), sValue);
					break;
				case MEDIA_IDR_AUDIOCOMPRESSORS:
					wkp.WriteString(AU_DIRECT_SHOW, GetString(CLSID_AudioCompressorCategory) , sValue);
					break;
				case MEDIA_IDR_AUDIO_RENDERERS:
					wkp.WriteString(AU_DIRECT_SHOW, GetString(CLSID_AudioRendererCategory)   , sValue);
					break;
			}
		}
	}
}
/*********************************************************************************************
 Class      : CAudioUnitDlg
 Function   : OnRequestValues
 Description: Exchange of AudioUnit Values via CIPC

 Parameters:   
  pCIPC  : (EA): CIPC Object  (CIPCMediaAudioUnit*)
  mediaID: (E): Security ID of the Unit  (CSecID)
  dwCmd  : (E): Command Value  (DWORD)
                Flags: (MEDIA_SET_VALUE, MEDIA_GET_VALUE, MEDIA_ERROR_OCCURRED)
  dwID   : (E): ID for the Command  (DWORD)
  dwValue: (EA): Value to exchange  (DWORD)
  pData  : (E): Extra data to exchange  (const CIPCExtraMemory*)
	
  Legend:
   dwCmd Flags
	* dwID
	  - MEDIA_COMMAND_VALUE(dwCmd)
	
  Values:
	MEDIA_SET_VALUE : (I)
	MEDIA_GET_VALUE : (O)
	* MEDIA_IDR_INPUT_PINS
	  - MEDIA_ENABLE					(IO)
	  - MEDIA_MONO						(IO)
	  - MEDIA_RECORDING_LEVEL		(IO)
	  - MEDIA_BALANCE					(IO)
	  - MEDIA_LOUDNESS				(IO)
	  - MEDIA_TREBLE					(IO)
	  - MEDIA_BASS						(IO)
	  - MEDIA_START_VOLUME_LEVEL	(IO)
	  - MEDIA_THRESHOLD				(IO)
	  - MEDIA_STEREO_CHANNEL		(IO)
	  - MEDIA_INPUT_PIN_NAME		(O)

	* MEDIA_IDR_AUDIO_RENDERERS
	  - MEDIA_VOLUME					(IO)
	  - MEDIA_BALANCE					(IO)

	* MEDIA_IDR_AUDIOCOMPRESSORS	(O)
	  - MEDIA_AUDIO_QUALITYS		(O) ListBoxTexts

	dwCmd only Value without Flag
	CB_GETCURSEL
	  - MEDIA_IDR_AUDIOINPUTDEVICES
	  - MEDIA_IDR_INPUT_PINS

   CB_SETCURSEL
	  - MEDIA_SAMPLE_FREQUENCY

	CB_SELECTSTRING
	  - MEDIA_IDR_AUDIOINPUTDEVICES		// Select Audio Input Device
	  - MEDIA_IDR_INPUT_PINS				// Select Input Channel

 Result type:  (void)
 created: September, 10 2003
 <TITLE OnRequestValues>
*********************************************************************************************/
void CAudioUnitDlg::OnRequestValues(CIPCMediaAudioUnit *pCIPC, CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory *pData)
{
	DWORD dwCommand = MEDIA_COMMAND_VALUE(dwCmd);
	HRESULT hr = NOERROR;
	CIPCExtraMemory *pReturnData = NULL;
	if (dwCmd & MEDIA_SET_VALUE)
	{
		if (dwID == MEDIA_IDR_INPUT_PINS)
		{
			switch (dwCommand) 
			{
				case MEDIA_START_VOLUME_LEVEL:
					if (dwValue == TRUE) m_bRecordingLevel = BST_INDETERMINATE;
					else                 m_bRecordingLevel = BST_UNCHECKED;
					SendDlgItemMessage(IDC_CK_RECORDING_LEVEL, BM_SETCHECK, m_bRecordingLevel);
					if (m_pSplitterInterface)
					{
						m_pSplitterInterface->SetMediaSampleCheckFnc(m_bRecordingLevel ? CheckMediaSample : NULL, (LPARAM)this);
						m_cRecPeakLevel.ShowWindow(m_bRecordingLevel ? SW_SHOW : SW_HIDE);
						m_cRecLevel.ShowWindow(m_bRecordingLevel ? SW_SHOW : SW_HIDE);
					}
				break;
				case MEDIA_START_LOCAL_REPLAY:
					::PostMessage(::GetDlgItem(m_hWnd, IDC_CK_LISTEN), BM_SETCHECK, dwValue != 0 ? TRUE : FALSE, 0);
					PostMessage(WM_COMMAND, MAKELONG(IDC_CK_LISTEN, BN_CLICKED), (LPARAM)m_hWnd);
					break;
				case MEDIA_THRESHOLD:
				{
					NMHDR nmhdr = {m_cThreshold.m_hWnd, IDC_SLIDER_THRESHOLD, NM_RELEASEDCAPTURE};
					if (dwValue == -1)
					{
						CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_strKey, _T(""));
						m_nThreshold = wkp.GetInt(AU_AUDIO_SETTINGS, AU_THRESHOLD, AU_DEFAULT_THRESHOLD);
					}
					else
					{
						m_nThreshold = dwValue;
						if (dwCmd & MEDIA_SAVE_IN_REGISTRY)
						{
							CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_strKey, _T(""));
							wkp.WriteInt(AU_AUDIO_SETTINGS, AU_THRESHOLD, m_nThreshold);
						}
					}
					m_cThreshold.SetPos(m_nThreshold);
					SendMessage(WM_NOTIFY, nmhdr.idFrom, (LPARAM)&nmhdr);
				} break;
				case MEDIA_STEREO_CHANNEL:
				{
					if (m_pCapture)
					{
						if (m_nStereoChannel != (int)dwValue)
						{
							m_nStereoChannel = dwValue | UPDATE_DLG_CTRL_VALUE; // OnRdLeft();
							PostMessage(WM_COMMAND, MAKELONG(IDC_RD_LEFT, BN_CLICKED));
						}
					}
					else
					{
						m_nStereoChannel = dwValue;
					}
				}break;
				default: hr = VFW_E_ENUM_OUT_OF_RANGE; break; 
			}

			if (hr == VFW_E_ENUM_OUT_OF_RANGE)
			{
				hr = S_OK;
				DWORD *pdwValues = new DWORD[5];
				pdwValues[0] = USE_CIPCMEDIA;
				if (pCIPC == m_pCIPCcontrol)
				{
					pdwValues[0] = USE_CIPCCONTROL; 
				}
				pdwValues[1] = mediaID.GetID();
				pdwValues[2] = dwCmd;
				pdwValues[3] = dwID;
				pdwValues[4] = dwValue;
				PostMessage(WM_INPUTMIXER_VALUE, (WPARAM)pdwValues, TRUE);
				return;
			}
		}
		else if (dwID == MEDIA_IDR_AUDIO_RENDERERS)
		{
			switch (dwCommand) 
			{
				case MEDIA_VOLUME:
				{
					m_nVolume = dwValue;
					m_cVolume.PostMessage(TBM_SETPOS, TRUE, dwValue);
					PostMessage(WM_COMMAND, MAKELONG(IDC_SLIDER_VOLUME, BN_CLICKED), (LPARAM)m_hWnd); 
					if (dwCmd & MEDIA_SAVE_IN_REGISTRY)
					{
						CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_strKey, _T(""));
						wkp.WriteInt(AU_AUDIO_SETTINGS, AU_OUTPUT_VOLUME, m_nVolume);
					}
				} break;
				case MEDIA_BALANCE:
				{
					m_nBalance = dwValue;
					PostMessage(WM_COMMAND, MAKELONG(IDC_SLIDER_VOLUME, BN_CLICKED), (LPARAM)m_hWnd); 
				} break;
				default: hr = VFW_E_ENUM_OUT_OF_RANGE; break; 
			}
		}
	}
	else if (dwCmd & MEDIA_GET_VALUE)
	{
		if (dwID == MEDIA_IDR_INPUT_PINS)
		{
			switch (dwCommand) 
			{
				case MEDIA_INPUT_PIN_NAME:
				{
					CString sSection, sChannel, sDeviceName, str;
					if (dwValue == 0xffffffff)
					{
						dwValue = MAKELONG(m_nInputChannel, m_nStereoChannel);
					}
					else if (LOWORD(dwValue) == 0xffff)
					{
						dwValue = MAKELONG(m_nInputChannel, HIWORD(dwValue));
					}
					if (IsBetween(LOWORD(dwValue), 0, m_cInputPinList.GetItemCount()-1))
					{
						sChannel    = m_cInputPinList.GetItemText(LOWORD(dwValue), 0);
						m_cCapture.GetLBText(m_cCapture.GetCurSel(), sDeviceName);
						sSection.Format(_T("%s\\%s\\%s\\%s"), AUDIO_UNIT, SEC_MEDIA_TYPE_INPUT, sDeviceName, sChannel);
						CWK_Profile wkp;
						int nActive = wkp.GetInt(sSection, AU_ACTIVE, 0);
						if (nActive)
						{
							str = wkp.GetString(sSection, AU_INPUT_NAME, NULL);
							if (!str.IsEmpty())
							{
								int nSemi = str.Find(_T(";"));
								if (HIWORD(dwValue) == 0xffff)					// gesamten String liefern ?
								{
									if (nActive == 1)									// nur links aktiv
									{
										if (nSemi != -1) str = str.Left(nSemi);// links liefern
										dwValue = MAKELONG(LOWORD(dwValue), 1);// und kennzeichnen
									}
									else if (nActive == 2)							// nur rechts aktiv
									{
										if (nSemi != -1) str = str.Mid(nSemi+1);// rechts liefern
										dwValue = MAKELONG(LOWORD(dwValue), 2);// und kennzeichnen
									}
									else
									{
										dwValue = MAKELONG(LOWORD(dwValue), 3);// und kennzeichnen
									}
								}
								else if (HIWORD(dwValue) == 1 && nActive & 1)// nur links liefern und links aktiv
								{
									if (nSemi != -1) str = str.Left(nSemi);	// links liefern
								}
								else if (HIWORD(dwValue) == 2 && nActive & 2)// nur rechts liefern und rechts aktiv
								{
									if (nSemi != -1) str = str.Mid(nSemi+1);	// rechts liefern
								}
								else
								{
									str.Empty();										// nichts liefern
									dwValue = MAKELONG(LOWORD(dwValue), 0);
								}
							}
						}
					}
					if (str.IsEmpty())
					{
						hr = S_FALSE;
						dwCmd |= MEDIA_ERROR_OCCURRED;
					}
					else
					{
						pReturnData = new CIPCExtraMemory;
						pReturnData->Create(pCIPC, str);
					}
				} break;
				case MEDIA_STEREO_CHANNEL:
				{
					dwValue = m_nStereoChannel;
				}break;
				case MEDIA_VOLUME_LEVEL:
					if (m_bRecordingLevel != BST_UNCHECKED) dwValue = 1;
					else									dwValue = 0;
				break;
				case MEDIA_THRESHOLD:
					dwValue = (DWORD)m_nThreshold;
					break;
				default: hr = VFW_E_ENUM_OUT_OF_RANGE; break; 
			}
			
			if (hr == VFW_E_ENUM_OUT_OF_RANGE)
			{
				hr = S_OK;
				DWORD *pdwValues = new DWORD[5];
				pdwValues[0] = USE_CIPCMEDIA;
				if (pCIPC == m_pCIPCcontrol)
				{
					pdwValues[0] = USE_CIPCCONTROL; 
				}
				pdwValues[1] = mediaID.GetID();
				pdwValues[2] = dwCmd;
				pdwValues[3] = dwID;
				pdwValues[4] = dwValue;
				PostMessage(WM_INPUTMIXER_VALUE, (WPARAM)pdwValues, FALSE);
				return;
			}
		}
		else if (dwID == MEDIA_IDR_AUDIO_RENDERERS)
		{
			switch (dwCommand) 
			{
				case MEDIA_VOLUME:
				{
					dwValue = m_nVolume;
				} break;
				case MEDIA_BALANCE:
				{
					dwValue = m_nBalance;
				} break;
				default: hr = VFW_E_ENUM_OUT_OF_RANGE; break; 
			}
		}
		else if (dwID == MEDIA_IDR_AUDIOCOMPRESSORS)
		{
			if (dwCommand == MEDIA_AUDIO_QUALITYS)
			{
				DWORD dwCIPC = USE_CIPCMEDIA;
				if (pCIPC == m_pCIPCcontrol)
				{
					dwCIPC = USE_CIPCCONTROL; 
				}																
				PostMessage(WM_REQUESTAUDIOQUALITIES, mediaID.GetID(), dwCIPC);
			}			
		}
		else if (dwID == 0)
		{
			if (dwCommand == MEDIA_STATE)
			{
				CIPCMediaRecord::MediaRecordFlags Flags;
				Flags.Reset();
				GetStateFlags(Flags);
				dwValue = Flags;
				dwCmd  = dwCommand|MEDIA_SET_VALUE;
				dwID    = CIPC_MEDIA_REQUEST_VALUES;
			}
		}
	}
	else if (dwCommand == CB_SELECTSTRING)
	{
		if (dwID == MEDIA_IDR_AUDIOINPUTDEVICES)
		{
			int nSel = (int)dwValue;										// Index 
			int nOld = m_cCapture.GetCurSel();
			if (pData)
			{
				CString sValue = pData->GetString();					// String
				nSel = m_cCapture.FindStringExact(-1, sValue);
				if (nSel == CB_ERR) nSel = (int)dwValue;				// bei Fehler den Index nehmen
			}
			nSel = m_cCapture.SetCurSel(nSel);							// Selektieren
			if (nSel &= nOld)
			{
				::EnableWindow(m_btnInitAudioRec.m_hWnd, TRUE);		// neu initialisieren
				PostMessage(WM_REQUESTSTARTMEDIAENCODING, MEDIA_ENC_ONLY_INITIALIZE|MEDIA_ENC_DONT_CHANGE_STATES);

				WORD wCIPC = USE_CIPCMEDIA;
				if (pCIPC == m_pCIPCcontrol)
				{
					wCIPC = USE_CIPCCONTROL; 
				}																	// neue Infos abfragen
				PostMessage(WM_REQUESTHARDWARE, mediaID.GetID(), MAKELONG(MEDIA_IDR_INPUT_PINS, wCIPC));
			}
			return; // No Confirmation
		}
		else if (dwID == MEDIA_IDR_INPUT_PINS)
		{
			int nSel = (int)dwValue;
			if (pData)
			{
				CString sValue = pData->GetString();
				LVFINDINFO info;
				info.flags = LVFI_STRING;
				info.psz   = sValue;
				dwValue = m_cInputPinList.FindItem(&info);
				if (dwValue != CB_ERR)
				{
					nSel = dwValue;
				}
			}
			if (IsBetween(nSel, 0, m_cInputPinList.GetItemCount()-1))
			{
				m_nInputChannel = dwValue;
				OnClickListInputPins(NULL, (long*)&dwValue);
				dwValue = m_nInputChannel;
				pCIPC->OnRequestValues(mediaID, MEDIA_GET_VALUE|MEDIA_RECORDING_LEVEL, MEDIA_IDR_INPUT_PINS, 0, NULL);
			}
			else
			{
				hr = E_INVALIDARG;
			}
		}
		else if (dwID == MEDIA_IDR_AUDIOCOMPRESSORS)
		{
			int nSel = (int)dwValue;
			if (pData)
			{
				CString sValue = pData->GetString();
				nSel = m_cCompressors.FindStringExact(-1, sValue);
			}
			m_cCompressors.SetCurSel(nSel);
			DWORD dwCIPC = USE_CIPCMEDIA;
			if (pCIPC == m_pCIPCcontrol)
			{
				dwCIPC = USE_CIPCCONTROL;
			}
			dwCIPC |= 0x00010000;	// Release Capture Filters
				
			PostMessage(WM_REQUESTAUDIOQUALITIES, mediaID.GetID(), dwCIPC);
		}
		else
		{
			hr = VFW_E_ENUM_OUT_OF_RANGE;
		}
	}
	else if (dwCommand == CB_GETCURSEL)
	{
		if (dwID == MEDIA_IDR_AUDIOINPUTDEVICES)
		{
			dwValue = m_cCapture.GetCurSel();
		}
		else if (dwID == MEDIA_IDR_INPUT_PINS)
		{
			dwValue = m_nInputChannel;
		}
		else
		{
			hr = VFW_E_ENUM_OUT_OF_RANGE;
		}
	}
	else if (dwCommand == CB_SETCURSEL)
	{
		if (dwID == MEDIA_SAMPLE_FREQUENCY)
		{
			PostMessage(WM_REQUESTSETSAMPLEFREQUENCY, dwValue, USE_CIPCCONTROL);
		}
	}
	else
	{
		hr = VFW_E_ENUM_OUT_OF_RANGE;
	}
	if (FAILED(hr))
	{
		WK_TRACE(_T("ERROR ! Cmd: %x, ID:%d\n"), dwCmd, dwID, dwValue);
		dwCmd |= MEDIA_ERROR_OCCURRED;
		dwValue = (DWORD)hr;
		PostMessage(WM_REPORT_ERROR, (WPARAM)HRESULT_ERROR_AT_POS(hr), 0);
	}
	pCIPC->DoConfirmValues(mediaID, dwCmd, dwID, dwValue, pReturnData);
	WK_DELETE(pReturnData);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnInputMixerValue(WPARAM wParam, LPARAM lParam)
{
	DWORD *pdwValues = (DWORD*)wParam;
	ASSERT(pdwValues != 0);
	HRESULT hr = S_OK;
	CIPCMedia* pCIPC = NULL;
	if (pdwValues[0] == USE_CIPCMEDIA)
	{
		pCIPC = m_pCIPCMedia;
	}
	else
	{
		pCIPC = m_pCIPCcontrol;
	}
	CSecID mediaID(pdwValues[1]);
	DWORD dwCmd		= pdwValues[2];
	DWORD dwID		= pdwValues[3];
	DWORD dwValue	= pdwValues[4];
	DWORD dwCommand = MEDIA_COMMAND_VALUE(dwCmd);
	delete[] pdwValues;

	if (pCIPC == NULL)
	{
		return 0;
	}

	IAMAudioInputMixer *pAMaim = NULL;
	if (IsBetween(m_nInputChannel, 0, m_cInputPinList.GetItemCount()-1))
	{
		ASSERT(m_cInputPinList.GetCheck(m_nInputChannel));
		pAMaim = (IAMAudioInputMixer*)m_cInputPinList.GetItemData(m_nInputChannel);
		if (!pAMaim)
		{
			hr = VFW_E_NO_INTERFACE;
		}
	}
	else
	{
		hr = VFW_E_WRONG_STATE;
	}
				
	if (pAMaim)
	{
		hr = S_OK;
		if (lParam) // Set
		{
			switch (dwCommand) 
			{
				case MEDIA_ENABLE:
					hr = pAMaim->put_Enable((BOOL)dwValue);
					break;
				case MEDIA_MONO:
					hr = pAMaim->put_Mono((BOOL)dwValue);
					break;
				case MEDIA_RECORDING_LEVEL:
				{
					double dValue;
					if (dwValue == 0xffffffff) dValue = AMF_AUTOMATICGAIN;
					else                       dValue = 0.001 * dwValue;
					hr = pAMaim->put_MixLevel(dValue);
					m_cRecordingLevel.SetPos((int)((1-dValue)*1000));
				}	break;
				case MEDIA_BALANCE:
				{
					double dValue = 0.001 * dwValue;
					hr = pAMaim->put_Pan(dValue);
				}	break;
				case MEDIA_LOUDNESS:
					hr = pAMaim->put_Loudness((BOOL)dwValue);
					break;
				case MEDIA_TREBLE:
				{
					double dRange;
					hr = pAMaim->get_TrebleRange(&dRange);
					if (SUCCEEDED(hr))
					{
						double dValue = 0.001 * dwValue * dRange;
						hr = pAMaim->put_Treble(dValue);
					}
				}break;
				case MEDIA_BASS:
				{
					double dRange;
					hr = pAMaim->get_BassRange(&dRange);
					if (SUCCEEDED(hr))
					{
						double dValue = 0.001 * dwValue * dRange;
						hr = pAMaim->put_Bass(dValue);
					}
				}break;
				default: hr = VFW_E_ENUM_OUT_OF_RANGE; break; 
			}
		}
		else
		{
			switch (dwCommand) 
			{
				case MEDIA_ENABLE:
					hr = pAMaim->get_Enable((BOOL*)&dwValue);
					break;
				case MEDIA_MONO:
					hr = pAMaim->get_Mono((BOOL*)&dwValue);
					break;
				case MEDIA_LOUDNESS:
					hr = pAMaim->get_Loudness((BOOL*)&dwValue);
					break;
				case MEDIA_RECORDING_LEVEL:
				{
					double dValue = 0;
					hr = pAMaim->get_MixLevel(&dValue);
					dwValue = (DWORD)(long)(dValue * 1000.0);
				}	break;
				case MEDIA_BALANCE:
				{
					double dValue = 0;
					hr = pAMaim->get_Pan(&dValue);
					dwValue = (DWORD)(long)(dValue * 1000.0);
				}	break;
				case MEDIA_TREBLE:
				{
					double dRange;
					hr = pAMaim->get_TrebleRange(&dRange);
					if (SUCCEEDED(hr))
					{
						double dValue = 0;
						hr = pAMaim->get_Treble(&dValue);
						dwValue = (DWORD)(long)(dValue * 1000.0 / dRange);
					}
				}break;
				case MEDIA_BASS:
				{
					double dRange;
					hr = pAMaim->get_BassRange(&dRange);
					if (SUCCEEDED(hr))
					{
						double dValue = 0;
						hr = pAMaim->get_Bass(&dValue);
						dwValue = (DWORD)(long)(dValue * 1000.0 / dRange);
					}
				}break;
				default: hr = VFW_E_ENUM_OUT_OF_RANGE; break; 
			}
		}
	}
	if (FAILED(hr))
	{
		WK_TRACE(_T("ERROR ! Cmd: %x, ID:%d\n"), dwCmd, dwID, dwValue);
		dwCmd |= MEDIA_ERROR_OCCURRED;
		dwValue = (DWORD)hr;
		
		ReportError(HRESULT_ERROR_AT_POS(hr), false);
	}
	pCIPC->DoConfirmValues(mediaID, dwCmd, dwID, dwValue, NULL);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::AddGraphToRot(BOOL bAdd)
{
	if (m_dwRegisterROT != 0)
	{
		RemoveGraphFromRot(m_dwRegisterROT);
		m_dwRegisterROT = 0;
	}
	if (bAdd)
	{
		::AddGraphToRot(m_pGB, &m_dwRegisterROT);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnDbgAddGraphToRot() 
{
	AddGraphToRot(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnUpdateDbgAddGraphToRot(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((m_dwRegisterROT == 0));
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnDbgRemoveGraphToRot() 
{
	AddGraphToRot(FALSE);
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnUpdateDbgRemoveGraphToRot(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((m_dwRegisterROT != 0));
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnDbgReleaseFilters() 
{
	OnStop();
	ReleaseFilters();

	if (m_bGenerateAlarm && m_pCIPCInput)
	{
		UpdateAudioDetector(m_pCIPCInput->IsAudioDetectorActive());
	}
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnUpdateDbgReleaseFilters(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pGB != NULL);
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnSettingsVolume() 
{
	ShellExecute(m_hWnd, _T("open"), _T("SNDVOL32.EXE"), NULL, NULL, SW_SHOW);
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnUpdateSettingsVolume(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();	
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnUpdateBtnAudioDeviceProperty(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pMediaSampleSource && SupportsPropertyPage(m_pMediaSampleSource));
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnUpdateBtnRendererProperty(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pRender && SupportsPropertyPage(m_pRender));
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnUpdateBtnSourceProperty(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pCapture && SupportsPropertyPage(m_pCapture));
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnUpdateBtnEncoderProperty(CCmdUI* pCmdUI) 
{
/*
	if (m_pEncode)
	{
		IAMStreamConfig  *pDlg;
		HRESULT hr = m_pEncode->QueryInterface(IID_IAMStreamConfig , (void**)&pDlg);
		if (SUCCEEDED(hr))
		{
			pDlg->Release();
		}
	}
*/
	pCmdUI->Enable(m_pEncode && SupportsPropertyPage(m_pEncode));
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnUpdateStart(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_btnStart.IsWindowEnabled());
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnUpdateStop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_btnStop.IsWindowEnabled());
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnUpdatePause(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_btnPause.IsWindowEnabled());
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnUpdateInitAudioRecord(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_btnInitAudioRec.IsWindowEnabled());
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::ActivateDetector(bool bActivate)
{
	if (bActivate)
	{
		if (m_pCIPCInput == NULL)
		{
			CString str;
			str.Format(SM_AudioUnitInput, theApp.GetInstanceCount());
			m_pCIPCInput = new CIPCInputAudioUnit(this, str);
		}
	}
	else
	{
		WK_DELETE(m_pCIPCInput);
	}
}
/////////////////////////////////////////////////////////////////////////////

BOOL CAudioUnitDlg::TestConfig(DWORD dwRequestID)
{
	// TODO! RKE: TestConfig muss auch wieder decodieren
	OnInitAudioRecord();
	if (m_pDump) return TRUE;
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAudioUnitDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnRequestReset()
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	WK_TRACE(_T("CAudioUnitDlg::OnRequestReset()\n"));
	if (m_nResetTimer)
	{
		KillTimer(m_nResetTimer);
		m_nResetTimer = 0;
	}

	OnStop();
	ReleaseFilters();
	
	m_bNoCompression          = false;
	m_bDeliverAlways          = FALSE;
	m_bCapturing              = false;
	m_bRendering              = false;
	m_bHeaderRequested        = false;
	m_bLastSampleSyncPoint    = false;
	m_bConfirmReceivedSamples = false;
	m_bMergeSamples           = true;

	ReadRegistryKeys();
	UpdateData(FALSE);

	SetComboBoxSelection(m_cCapture, m_sCapture);
	SetComboBoxSelection(m_cCompressors, m_sCompressor);
	SetComboBoxSelection(m_cRenderer, m_sRenderer);
	m_cQuality.SetCurSel(m_nQuality);

	if (m_bGenerateAlarm && m_pCIPCInput)
	{
		UpdateAudioDetector(m_pCIPCInput->IsAudioDetectorActive());
	}
	
	if (m_pCIPCInput && m_pCIPCInput->DoReset())
	{
		m_pCIPCInput->ConfirmReset();
	}

	if (m_pCIPCMedia && m_pCIPCMedia->DoReset())
	{
		m_pCIPCMedia->ConfirmReset();
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnEnterMenuLoop(WPARAM wParam, LPARAM lParam)
{
	COemGuiApi::DoUpdatePopupMenu(AfxGetMainWnd(), GetMenu());
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnRequestDisconnect(CIPCMediaAudioUnit*pMAU)
{
	if (pMAU == m_pCIPCcontrol)
	{
		PostMessage(WK_WINTHREAD_STOP_THREAD, FALSE, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAudioUnitDlg::SetComboBoxSelection(CComboBox& cCombo, CString &sSel)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	BOOL bFound = FALSE;
	int nFind = cCombo.FindStringExact(-1, sSel);
	if (nFind == CB_ERR)
	{
		nFind = cCombo.FindString(-1, sSel);
		if (nFind == CB_ERR)
		{
			CString s, sFind = sSel;
			sFind.MakeLower();
			int nCount = cCombo.GetCount();
			for (nFind=0; nFind<nCount; nFind++)
			{
				cCombo.GetLBText(nFind, s);
				s.MakeLower();
				if (s.Find(sFind) != -1)
				{
					bFound = TRUE;
					break;
				}
			}
			if (!bFound)
			{
				nFind = 0;
			}
		}
		else
		{
			bFound = TRUE;
		}
	}
	else
	{
		bFound = TRUE;
	}
	
	nFind = cCombo.SetCurSel(nFind);
	if (nFind != CB_ERR)
	{
		cCombo.GetLBText(nFind, sSel);
	}
	else
	{
		sSel.Empty();
	}

	return bFound;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::GetStateFlags(CIPCMediaRecord::MediaRecordFlags &Flags)
{
	if (m_pCapture)
	{
		Flags.IsCaptureInitialized = true;
		Flags.IsCapturing = m_bCapturing;
	}
	if (m_pRender)
	{
		Flags.IsRenderInitialized  = true;
		Flags.IsRendering = m_bRendering;
	}
	Flags.IsEnabled = !::IsWindowEnabled(m_btnStart.m_hWnd);

	if (m_cCapture.GetCount())  Flags.IsInput  = true;
	if (m_cRenderer.GetCount()) Flags.IsOutput = true;

	Flags.IsOkay = Flags.IsInput || Flags.IsOutput;
	Flags.IsAudio = true;

	if (theApp.m_bTraceStateFlags)
	{
		WK_TRACE(_T("Capture:%d, Init:%d; Render:%d, Init:%d\n"), Flags.IsCapturing, Flags.IsCaptureInitialized, Flags.IsRendering, Flags.IsRenderInitialized);
	}
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
/*
BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
	if (CWnd::FromHandlePermanent(hwnd) != NULL)
	{
		return 1;
	}
	_TCHAR szClassName[128];
	CDataExchange dx((CWnd*)lParam, FALSE);
	GetClassName(hwnd, szClassName, 128);
	long lStyle = GetWindowLong(hwnd, GWL_STYLE);
	if (strcmp(szClassName, _T("Button")) == 0)
	{
		CSkinButton *pBtn = new CSkinButton();
		DDX_Control(&dx, ::GetDlgCtrlID(hwnd), *pBtn);
//		pBtn->SetSurface(SurfaceColorChangeBrushed);
		pBtn->SetBaseColor(SKIN_COLOR_GOLD_METALLIC);
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::MakeSkin()
{
	EnumChildWindows(m_hWnd, EnumChildProc, (LPARAM)this);
}
*/
#endif
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	WK_TRACE(_T("OnShowWindow(%d, %d)\n"), bShow, nStatus); 
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::DoConfirmMediaState(DWORD dwID)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pCIPCMedia)
	{
		CIPCMediaRecord::MediaRecordFlags Flags;
		Flags.Reset();
		GetStateFlags(Flags);
		if (dwID == 0)
		{
			dwID = m_pCIPCMedia->GetLastCmd();
		}
		if (m_dwOldMediaState != Flags)
		{
			if (theApp.m_bTraceStateFlags)
			{
				WK_TRACE(_T("DoConfirmMediaState=> SecID:%08x, ID:%d, F:%x\n"), m_pCIPCMedia->GetReplyID().GetID(), dwID, (DWORD)Flags);
			}
			m_pCIPCMedia->DoConfirmValues(m_pCIPCMedia->GetReplyID(), MEDIA_SET_VALUE|MEDIA_STATE, dwID, Flags);
			if (m_pCIPCcontrol)
			{
				m_pCIPCcontrol->DoConfirmValues(m_pCIPCcontrol->GetReplyID(), MEDIA_SET_VALUE|MEDIA_STATE, dwID, Flags);
			}
			SetButtonIcon(IDC_PLAYING, m_bRendering ? IDC_RECORDING: IDC_STOP);
		}
		m_dwOldMediaState = Flags;
	}
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::UpdateAudioDetector(bool bOn)
{
	if (bOn)
	{
		PostMessage(WM_REQUESTSTARTMEDIAENCODING, MEDIA_ENC_NO_COMPRESSION|MEDIA_ENC_DONT_SHOW_STATE);
	}
	else
	{
/*
		if (m_pCIPCInput)
		{
			m_pCIPCInput->SetAudioAlarm(FALSE);
		}
*/
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CAudioUnitDlg::EnumRenderers(ICreateDevEnum *pSysDevEnum)
{
	HRESULT hr = EnumFiltersWithMonikerToList(pSysDevEnum, &CLSID_AudioRendererCategory, &m_cRenderer);
	if (hr == S_FALSE)
	{
		ClearFilterListWithMoniker(m_cRenderer);
	}
	else
	{
		VARIANT varName={0};
		int i, nCount = m_cRenderer.GetCount();
		for (i=0; i<nCount; i++)
		{
			IMoniker *pMoniker = (IMoniker*)m_cRenderer.GetItemData(i);
			IPropertyBag *pPropBag;
			ASSERT(pMoniker);

			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
												(void **)&pPropBag);
			if (FAILED(hr)) continue;

			varName.vt = VT_BSTR;
			hr = pPropBag->Read(L"CLSID", &varName, 0);
			if (FAILED(hr)) continue;

			CString str(varName.bstrVal);
			SysFreeString(varName.bstrVal);
			if (CLSID_DSoundRender == MakeGUID(str))
			{
				varName.vt = VT_BSTR;
				hr = pPropBag->Read(L"DSGuid", &varName, 0);
				if (FAILED(hr)) continue;

				CString str(varName.bstrVal);
				SysFreeString(varName.bstrVal);
				if (GUID_NULL != MakeGUID(str))
				{
					m_cRenderer.DeleteString(i);
					i--;
					nCount--;
				}
			}
			else
			{
				m_cRenderer.DeleteString(i);
				i--;
				nCount--;
			}
		}

		BOOL bResult = SetComboBoxSelection(m_cRenderer, m_sRenderer);
		if (bResult)
		{
			IMoniker*pMoniker = (IMoniker*)m_cRenderer.GetItemData(m_cRenderer.GetCurSel());
			BOOL bExists = FALSE;
			if (pMoniker != (IMoniker*)CB_ERR)
			{
				HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGB);
				if (SUCCEEDED(hr))
				{
					hr = AddFilterToGraph(m_pGB, &m_pRender, pMoniker, NULL);
					if (SUCCEEDED(hr))
					{
						bExists = TRUE;
					}
				}
			}
			RELEASE_OBJECT(m_pRender);
			RELEASE_OBJECT(m_pGB);
			if (!bExists)
			{
				ClearFilterListWithMoniker(m_cRenderer);
				m_sRenderer.Empty();
				bResult = FALSE;
			}
		}
		return bResult;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::InitResetTimer()
{
	if (m_nResetTimer == 0)
	{
		m_nResetTimer = SetTimer(TIMER_RESET, 1000, NULL);
	}

	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_strKey, _T(""));
	m_bGenerateAlarm    = wkp.GetInt(AU_CIPC_SETTINGS, AU_GENERATE_ALARM, FALSE);
}
//////////////////////////////////////////////////////////////////////////

void CAudioUnitDlg::OnCkDeliverAlways() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_DELIVER_ALWAYS, m_bDeliverAlways);
}
//////////////////////////////////////////////////////////////////////////
void CAudioUnitDlg::SetOutputParam()
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	IBasicAudio *pBA = NULL;
	if (m_pGB)
	{
		m_pGB->QueryInterface(IID_IBasicAudio, (void**)&pBA);
		if (pBA)
		{
			long lVolume  = (m_nVolume - 1000) * 10;	// range from 0 to 1000
			long lBalance = m_nBalance * 1000;			// range from -100 to 100
#ifdef _USE_AS_MEDIAPLAYER
			if (!theApp.m_sParam.IsEmpty())
			{
				double dLog = log10((double)m_nVolume);
				dLog = -(3.0-dLog);
				lVolume = (long)(dLog * 3333.3333333333333333333333333333);
			}
#endif // _USE_AS_MEDIAPLAYER
			pBA->put_Volume(lVolume);					// range from -10000 to 0
			pBA->put_Balance(lBalance);					// range from -10000 to 10000
/*
			long lVolume;
			hr = pBA->get_Volume(&lVolume);
			dwValue = (lVolume / 10) + 1000;
			long lBalance;
			hr = pBA->get_Balance(&lBalance);
			dwValue = lBalance / 1000;
*/
			pBA->Release();
		}
	}	
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CAudioUnitDlg::OnLanguageChanged(WPARAM wParam, LPARAM lParam)
{
#if _MFC_VER >= 0x0710
	return theApp.SetLanguageParameters((UINT)wParam, (DWORD)lParam);
#else
	return 0;
#endif
}
