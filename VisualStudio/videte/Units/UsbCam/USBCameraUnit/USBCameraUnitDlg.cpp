/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCameraUnit
// FILE:		$Workfile: USBCameraUnitDlg.cpp $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCameraUnit/USBCameraUnitDlg.cpp $
// CHECKIN:		$Date: 31.10.05 14:12 $
// VERSION:		$Revision: 31 $
// LAST CHANGE:	$Modtime: 13.09.05 11:24 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "USBCameraUnit.h"
#include "USBCameraUnitDlg.h"
#include "CIPCMediaVideo.h"
#include "CIPCOutputUSBcamUnit.h"
#include "CIPCInputUSBcamUnitMDAlarm.h"

#pragma warning( push )
#pragma warning( disable : 4201 )
#include <KS.h>
#pragma warning( pop )

#include <math.h>
#include <KSMEDIA.h>
#include <qedit.h>    

#include <initguid.h>    // DEFINE_GUID to declare an EXTERN_C const.
//#include "filters\RGB2YUV\iRGB2YUV.h"
#include "filters\Sample2MemRenderer\iSample2MemRenderer.h"
#include "filters\Dump\idump.h"
#include "filters\inftee\iinftee.h"
#include "filters\MediaSource\imediasource.h"
#include "filters\PushSource\iPushSource.h"

#include "common\mfcutil.h"
#include "common\dshowutil.h"
#include "include\dmoreg.h"
#include "common\namedguid.h"

#include <vimage\cjpeg.h>    
#include ".\usbcameraunitdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
class CSampleGrabberCB : public ISampleGrabberCB 
{

public:

    // These will get set by the main thread below. We need to
    // know this in order to write out the bmp
    long Width;
    long Height;

    // Fake out any COM ref counting
    //
    STDMETHODIMP_(ULONG) AddRef() { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }

    // Fake out any COM QI'ing
    //
    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv)
    {
        CheckPointer(ppv,E_POINTER);
        
        if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ) 
        {
            *ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
            return NOERROR;
        }    

        return E_NOINTERFACE;
    }


    // We don't implement this one
    //
    STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample )
    {
        return 0;
    }


    // The sample grabber is calling us back on its deliver thread.
    // This is NOT the main app thread!
    //
    STDMETHODIMP BufferCB( double SampleTime, BYTE * pBuffer, long BufferSize )
    {
        return 0;
    }
};

CSampleGrabberCB g_CB;
#endif

struct ErrorStruct
{
	ErrorStruct(HRESULT hr, int nLine, CString sFile) 
	{
		m_hr = hr;
		m_nLine = nLine;
		m_sFile = sFile;
	}
	int         m_nLine;
	HRESULT     m_hr;
	CString     m_sFile;
};

#define HRESULT_ERROR_AT_POS(hr) new ErrorStruct(hr, __LINE__, __FILE__)
#define HR_EXCEPTION(HR) if (FAILED(hr=HR))	throw HRESULT_ERROR_AT_POS(hr);
#define HR_REPORT(HR) if (FAILED(hr=HR))	ReportError(HRESULT_ERROR_AT_POS(hr), false);
#define HR_MSG_BOX(HR) if (FAILED(hr=HR))	ReportError(HRESULT_ERROR_AT_POS(hr));


#define BTN_STATE_INITIALIZED   0x00010000
#define BTN_STATE_MOTION_DETECT 0x00020000
#define BTN_STATE_PLAY          0x00040000
#define BTN_STATE_ACTIVE	     0x00080000

#define SETTINGS_SECTION   _T("Settings")
#define DEBUG_SETTINGS	 _T("Debug")
#define STAND_ALONE		 _T("StandAlone")
#define OUT_FILE_NAME	 _T("FileName")

#define DIRECT_SHOW      _T("DirectShow")

#define VIDEO_INPUTS 	 _T("VideoInputs")
#define VIDEO_INPUT 	 _T("VideoInput")
#define OUTPUT_FORMAT    _T("OutputFormat")
#define OUTPUT_RATE		 _T("OutputRate")
#define CAM_CTRL_PAN		 _T("Pan")	
#define CAM_CTRL_TILT	 _T("Tilt")
#define CAM_CTRL_ZOOM	 _T("Zoom")
#define CAM_CTRL_FOCUS	 _T("Focus")
#define CAM_CTRL_ROLL	 _T("Roll")	

#define ENDTIME_VIDEO_JOB        1l
#define ENDTIME_MOTION_DETECTION 2l


#ifdef _DEBUG
//#define __TEST 1
  #define TRACE_NAME_OF(a) case a: TRACE(_T("%s\n"), #a); break;
#else
  #define TRACE_NAME_OF //
#endif

#ifdef TEST_WITH_SAVIC
	#define SM_OUTPUT_CAMERA SM_SAVIC_OUTPUT_CAMERAS
	#define SYNC_EVENT       _T("SaVicSyncEvent")
	#define SM_OUTPUT        SM_SAVIC_OUTPUT_RELAYS
	#define SM_MD_INPUT      SM_SAVIC_MD_INPUT
	#define SM_INPUT         SM_SAVIC_INPUT
#else
	#define SM_OUTPUT_CAMERA SM_USBCAM_OUTPUT_CAMERA	// Camera Pictures
	#define SM_MD_INPUT      SM_USBCAM_MD_INPUT			// Motion Detection
	#define SM_OUTPUT        SM_USBCAM_OUTPUT			// evtl. PTZ
	#define SM_INPUT         SM_USBCAM_INPUT			// evtl. Button
#endif

int FindGCD(int nU, int nV)
{
	int nT;
	do 
	{
		if (nU<nV)
		{
			nT = nU;
			nU = nV;
			nV = nT;
		}
		nU = nU - nV;
	} while(nU>0);
	return nV;
}

extern CUSBCameraUnitApp theApp;

CUSBCameraUnitDlg *CUSBCameraUnitDlg::gm_pThis = NULL;

/////////////////////////////////////////////////////////////////////////////
// CUSBCameraUnitDlg dialog
CUSBCameraUnitDlg::CUSBCameraUnitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUSBCameraUnitDlg::IDD, pParent)
	, m_bServer(FALSE)
{
	gm_pThis = this;

	m_sKey = _T("DVRT\\");
	m_sKey += USB_CAM_UNIT;

	m_sDeviceKey.Format(USB_CAM_UNIT_DEV, theApp.GetInstanceCount());
	m_sDeviceKey = m_sKey + m_sDeviceKey;


	//{{AFX_DATA_INIT(CUSBCameraUnitDlg)
	m_bShowPicture = FALSE;
	m_bStillImage = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon[0] = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon[1] = AfxGetApp()->LoadIcon(IDR_RUNNING);
	m_hIcon[2] = AfxGetApp()->LoadIcon(IDR_RECORDING);

	m_bStandalone = TRUE;

	m_pGraph				= NULL;
	m_pGraphBuilder	= NULL;
	m_pMediaControl	= NULL;
	m_pMediaEvent     = NULL;
	m_pVideoWindow    = NULL;
	
	ReadRegistry();

	m_bStillImage = !m_bStandalone;

	m_dwRegister          = 0;
	m_nSelectedCam        = CB_ERR;
	m_nCamIndex				 = CB_ERR;
	m_ppVSccOutputFormats = NULL;
//	m_nDevChangeTimer     = 0;
	m_bDTS                = FALSE;
	m_bIsShuttingDown     = FALSE;

	m_pInputMDAlarm       = NULL;
	m_pInput              = NULL;
	m_pOutput             = NULL;
	m_bStarted			    = FALSE;
	m_pMD					    = NULL;

#ifdef TEST_WITH_SAVIC 
	m_hSyncSemaphore	    = NULL;
#endif


#ifdef _DEBUG
	m_bColor = TRUE;
	m_pCIPCMedia   = NULL;
	ZeroMemory(&m_AP       , sizeof(ALLOCATOR_PROPERTIES));
	ZeroMemory(&m_MediaType, sizeof(AM_MEDIA_TYPE));
	m_pMediaSampleSource  = NULL;
	m_pIMediaSampleSource = NULL;
#endif
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::ReadRegistry()
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, m_sKey, "");
	CWK_Profile wkpDev(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sDeviceKey, "");
	CString strSection, strGUID;

//	strGUID.Empty();// = GetString(CLSID_VideoInputDeviceCategory);
	for (int i=0; i< MAX_CAMERAS; i++)
	{
		strSection.Format(CAMERA_N, i+1);
		m_sVideoInput[i]     = wkpDev.GetString(strSection, VIDEO_INPUT, NULL);
		m_nOutputFormat[i]   = wkpDev.GetInt(   strSection, OUTPUT_FORMAT, CB_ERR);
		m_nOutputRate[i]     = wkpDev.GetInt(   strSection, OUTPUT_RATE, CB_ERR);
		m_pCapture[i]        = NULL;
		m_pEncoder[i]		   = NULL;
		ZERO_INIT(m_sCameraState[i]);
	}

	ASSERT(sizeof(CameraStates) == sizeof(DWORD));

	strGUID				= GetString(CLSID_VideoCompressorCategory);
	m_sEncoder			= wkpDev.GetString(SETTINGS_SECTION, strGUID, NULL);
	m_nRefPictRate		= wkpDev.GetInt(SETTINGS_SECTION, REF_PICT_RATE, 50);
	m_bMotionDetection	= wkpDev.GetInt(SETTINGS_SECTION, MOTION_DETECTION, 1);
	m_bCameraDetectors	= wkpDev.GetInt(SETTINGS_SECTION, CAMERA_DETECTORS, 0);
	m_szResolution.cx	= wkpDev.GetInt(SETTINGS_SECTION, PICT_RES_X, 384);
	m_szResolution.cy	= wkpDev.GetInt(SETTINGS_SECTION, PICT_RES_Y, 288);

	m_bStandalone		= wkp.GetInt(DEBUG_SETTINGS, STAND_ALONE, m_bStandalone);
	m_strOutputFileName	= wkpDev.GetString(DEBUG_SETTINGS, OUT_FILE_NAME, NULL);
}
//////////////////////////////////////////////////////////////////////////
CUSBCameraUnitDlg::~CUSBCameraUnitDlg()
{
#ifdef _DEBUG
	WK_DELETE(m_pCIPCMedia);
#endif
	
	gm_pThis = NULL;
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUSBCameraUnitDlg)
	DDX_Control(pDX, IDC_COMBO_TEST2, m_cTest);
	DDX_Control(pDX, IDC_COMBO_FORMATS2, m_cOutputFormats2);
	DDX_Control(pDX, IDC_COMBO_CAMERA_NO, m_cCameraNo);
	DDX_Control(pDX, IDC_COMBO_TEST, m_cTestFilters);
	DDX_Control(pDX, IDC_FRM_CAM_PICTURE, m_cFrame);
	DDX_Control(pDX, IDC_COMBO_FRAMERATES, m_cFrameRates);
	DDX_Control(pDX, IDC_COMBO_ENCODERS, m_cEncoders);
	DDX_Control(pDX, IDC_BTN_UN_INIT, m_btnRelease);
	DDX_Control(pDX, IDC_BTN_INIT, m_btnInitialize);
	DDX_Control(pDX, IDC_BTN_STOP, m_btnStop);
	DDX_Control(pDX, IDC_BTN_START, m_btnStart);
	DDX_Control(pDX, IDC_COMBO_FORMATS, m_cOutputFormats);
	DDX_Control(pDX, IDC_COMBO_CAMERAS, m_cCameras);
	DDX_Check(pDX, IDC_CK_STILL_IMAGE, m_bStillImage);
	DDX_Check(pDX, IDC_BTN_SERVER, m_bServer);
	//}}AFX_DATA_MAP
}

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CUSBCameraUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CUSBCameraUnitDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_INIT, OnBtnInit)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_START, OnBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERAS, OnSelchangeVideoSource)
	ON_BN_CLICKED(IDC_BTN_UN_INIT, OnBtnUnInit)
	ON_BN_CLICKED(IDC_CK_SHOW_PICTUR, OnCkShowPicture)
	ON_COMMAND(ID_RELEASE_ALL, OnReleaseAllFilters)
	ON_COMMAND(ID_DBG_REMOVE_ROT, OnRemoveFromRot)
	ON_COMMAND(ID_DBG_ADD_TO_ROT, OnAddToRot)
	ON_UPDATE_COMMAND_UI(ID_DBG_ADD_TO_ROT, OnUpdateDbgAddToRot)
	ON_UPDATE_COMMAND_UI(ID_DBG_REMOVE_ROT, OnUpdateDbgRemoveRot)
	ON_UPDATE_COMMAND_UI(ID_RELEASE_ALL, OnUpdateReleaseAll)
	ON_COMMAND(ID_SETTINGS_ENCODER, OnSettingsEncoder)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS_ENCODER, OnUpdateSettingsEncoder)
	ON_CBN_SELCHANGE(IDC_COMBO_FORMATS, OnSelchangeComboFormats)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_APPLY_NOW, OnApplyNow)
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERA_NO, OnSelchangeComboCameraNo)
	ON_CBN_SELCHANGE(IDC_COMBO_FRAMERATES, OnSelchangeComboFramerates)
	ON_COMMAND(ID_SETTINGS_ENCODER_INFO, OnSettingsEncoderInfo)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS_ENCODER_INFO, OnUpdateSettingsEncoderInfo)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_UPDATE_COMMAND_UI(ID_APP_ABOUT, OnUpdateAppAbout)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_SETTINGS_DLG, OnSettingsDlg)
	ON_BN_CLICKED(IDC_CK_MOTION_DETECTION, OnCkMotionDetection)
	ON_COMMAND(ID_SETTINGS_MD, OnSettingsMd)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS_MD, OnUpdateSettingsMd)
	ON_COMMAND(ID_REQUEST_RESET, OnRequestReset)
	ON_UPDATE_COMMAND_UI(ID_REQUEST_RESET, OnUpdateRequestReset)
	ON_COMMAND(ID_SETTINGS_SOURCE_FILTER, OnSettingsSourceFilter)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS_SOURCE_FILTER, OnUpdateSettingsSourceFilter)
	ON_BN_CLICKED(IDC_CK_STILL_IMAGE, OnCkStillImage)
	ON_COMMAND_RANGE(ID_SETTINGS_CAMERA,ID_SETTINGS_CAMERA+MAX_CAMERAS, OnSettingsCamera)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SETTINGS_CAMERA, ID_SETTINGS_CAMERA+MAX_CAMERAS, OnUpdateSettingsCamera)
	ON_COMMAND(IDC_BTN_START, OnBtnStart)
	ON_UPDATE_COMMAND_UI(IDC_BTN_START, OnUpdateBtnStart)
	ON_COMMAND(IDC_BTN_STOP, OnBtnStop)
	ON_UPDATE_COMMAND_UI(IDC_BTN_STOP, OnUpdateBtnStop)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphNotify)
	ON_MESSAGE(WM_ENTERMENULOOP, OnEnterMenuLoop)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)
	ON_MESSAGE(WM_REQUEST_CAMERA_CONTROL, OnRequestCameraControl)
	//}}AFX_MSG_MAP
#ifdef _DEBUG
	ON_MESSAGE(WM_DECODE_MEDIA_DATA, OnDecodeMediaData)
	ON_MESSAGE(WM_CONFIRM_PICTURE_RECORD, OnConfirmPictureRecord)
	ON_BN_CLICKED(IDC_PLAYING, OnInitVideoPlay)
#endif
	ON_BN_CLICKED(IDC_BTN_SERVER, OnBnClickedBtnServer)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBCameraUnitDlg message handlers
LRESULT CALLBACK CUSBCameraUnitDlg::VideoWindowFrameProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	long lWndProc = ::GetWindowLong(hwnd, GWL_USERDATA);
	switch (uMsg)
	{
		case WM_PAINT:
			{
				CPaintDC dc(FromHandle(hwnd));
			}
			return 0;

		case  WM_ERASEBKGND:
			return 1;

		case WM_DESTROY:
			::SetWindowLong(hwnd, GWL_WNDPROC,lWndProc);
			break;
	}
	return CallWindowProc((WNDPROC)lWndProc, hwnd, uMsg, wParam, lParam); 
}
//////////////////////////////////////////////////////////////////////////
BOOL CUSBCameraUnitDlg::OnInitDialog()
{
	CString str;
	int i, nSel, nCountInit = 0;
	CDialog::OnInitDialog();
	SetWindowText(GetAppnameFromId(theApp.GetApplicationId()));
	long lWndProc = ::GetWindowLong(m_cFrame.m_hWnd,  GWL_USERDATA);
	if (lWndProc == 0)
	{
		::SetWindowLong(m_cFrame.m_hWnd, GWL_USERDATA, ::SetWindowLong(m_cFrame.m_hWnd,  GWL_WNDPROC, (long)VideoWindowFrameProc));
	}

	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		EnumFiltersWithMonikerToList(NULL, &CLSID_VideoInputDeviceCategory, NULL, &m_cCameras);
		SaveCamerasToRegistry();
		EnumFiltersWithMonikerToList(NULL, &CLSID_VideoCompressorCategory, NULL, &m_cEncoders);
		CString sText;
		sText.LoadString(IDS_NO_ENCODER);
		m_cEncoders.InsertString(0, sText);
		nSel = m_cEncoders.FindStringExact(0, m_sEncoder);
		if (nSel == CB_ERR) nSel = 0;
		m_cEncoders.SetCurSel(nSel);

#ifdef _DEBUG
		sText = _T("NoFilter");
		m_cTestFilters.InsertString(0, sText);
		m_cTestFilters.SetCurSel(0);
		EnumFiltersWithMonikerToList(NULL, &CLSID_CQzFilterClassManager, NULL, &m_cTestFilters);
#else
		m_cTestFilters.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TXT_DECODERS)->ShowWindow(SW_HIDE);
#endif
	}

	for (i=0; i<MAX_CAMERAS; i++)
	{
		str.Format(_T("%d"), i+1);
		m_cCameraNo.InsertString(i, str);
		if (!m_sVideoInput[i].IsEmpty())
		{
			nSel = m_cCameras.FindStringExact(-1, m_sVideoInput[i]);
			m_sCameraState[i].bActive = 1;
		}
		else
		{
			nSel = CB_ERR;
		}
		if (nSel != CB_ERR)
		{
			nCountInit++;
		}
		m_cCameraNo.SetItemData(i, nSel);
	}
	
	if (nCountInit == 0)
	{
		nCountInit = m_cCameras.GetCount();
		for (i=0; i<nCountInit; i++)
		{
			m_cCameras.GetLBText(i, m_sVideoInput[i]);
			if (nCountInit == 1 && m_sVideoInput[i].Find(_T("<<")) != -1 && m_sVideoInput[i].Find(_T(">>")) != -1)
			{	// No Entries
				m_sVideoInput[i].Empty();
				OnSelchangeComboCameraNo();
				break;
			}
			m_cCameraNo.SetItemData(i, i);
			m_cCameraNo.SetCurSel(i);
			OnSelchangeComboCameraNo();
		}
		OnApplyNow();
	}
	else
	{
		m_TimerEvents.SetAt(TIMER_DEV_UPDATE, TIMER_DEV_UPDATE);
		SendMessage(WM_TIMER, TIMER_DEV_UPDATE, NULL);
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon[0], TRUE);			// Set big icon
	SetIcon(m_hIcon[0], FALSE);		// Set small icon
	
	SetButtonIcon(IDC_BTN_START, IDC_START);
	SetButtonIcon(IDC_BTN_STOP , IDC_STOP);
	SetButtonIcon(IDC_RECORDING, IDC_STOP);
	SetButtonIcon(IDC_STILL    , IDC_STOP);
	m_cTest.SetCurSel(0);

	if (m_bStandalone)					// Testbetrieb
	{
		m_cCameraNo.SetCurSel(0);
		OnSelchangeComboCameraNo();
		if (m_nCamIndex != CB_ERR)
		{
			SetControlStates(m_sCameraState[m_nCamIndex]);
		}
		else
		{
			CameraStates s={0};
			SetControlStates(s);
		}
		InitMotionDetection();
#ifdef _DEBUG
		BOOL asMaster = theApp.GetInstanceCount() == 1 ? TRUE : FALSE;
//		m_pCIPCMedia = new CIPCMediaVideo(_T("SMVideoUnitMedia"), asMaster, this); 
		CString sSMName;
		sSMName = SM_TASHA_OUTPUT_CAMERAS;
//		sSMName += _T("1");
//		sSMName = _T("SMUSBcamUnitOutput");
		m_pOutput = new CIPCOutputUSBcamUnit(this, sSMName, asMaster);
#endif	
	}
	else										// Aufnahme
	{
		InitFunctions();
	}
	
	RegisterDeviceDetect(m_hWnd, GUID_DEVINTCLASS_VIDEO);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnPaint() 
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
//////////////////////////////////////////////////////////////////////////
LRESULT CUSBCameraUnitDlg::OnGraphNotify(WPARAM wParam,  LPARAM lParam)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	LONG evCode, evParam1, evParam2;
	HRESULT hr=S_OK;
	if (m_pMediaEvent)
	{
		while(SUCCEEDED(m_pMediaEvent->GetEvent(&evCode, (LONG_PTR *) &evParam1, (LONG_PTR *) &evParam2, 0)))
		{
			CString str;
			switch (evCode)
			{
				TRACE_NAME_OF(EC_COMPLETE);
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
//				TRACE_NAME_OF(EC_PAUSED);
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
//				TRACE_NAME_OF(EC_DEVICE_LOST);
				TRACE_NAME_OF(EC_STEP_COMPLETE);
				TRACE_NAME_OF(EC_TIMECODE_AVAILABLE);
				TRACE_NAME_OF(EC_EXTDEVICE_MODE_CHANGE);
				TRACE_NAME_OF(EC_STATE_CHANGE);
				TRACE_NAME_OF(EC_GRAPH_CHANGED);
				TRACE_NAME_OF(EC_CLOCK_UNSET);
				TRACE_NAME_OF(EC_VMR_RENDERDEVICE_SET);
				TRACE_NAME_OF(EC_VMR_SURFACE_FLIPPED);
				TRACE_NAME_OF(EC_VMR_RECONNECTION_FAILED);
				case EC_PAUSED:
					if (m_bShowPicture)
					{
//						m_cFrame.InvalidateRect(NULL);
					}
					break;

				case EC_DEVICE_LOST:
				{
					IUnknown*	pUnk = (IUnknown*)evParam1;
					IBaseFilter*pFilter = NULL;						// Search for BaseFilter
					int			i = MAX_CAMERAS;

					if (SUCCEEDED(pUnk->QueryInterface(IID_IBaseFilter, (void**)&pFilter)))
					{
						for (i=0; i<MAX_CAMERAS; i++)
						{
							if (m_pCapture[i] == pFilter)
							{
								break;
							}
						}
					}
					RELEASE_OBJECT(pFilter);

					if (i<MAX_CAMERAS)
					{
						OnBtnStop();
						if (evParam2 == 0)									// Device removed
						{
							// When the device becomes available again,
							// the previous state of the device filter is no longer valid.
							// The application must rebuild the graph in order to use the device.
							ReleaseFilters(i);
						}

						if (!m_bStandalone)
						{
							for (i=0; i<MAX_CAMERAS; i++)
							{
								if (m_sCameraState[i].bInitialized)
								{
									break;
								}
							}
							if (i<MAX_CAMERAS)
							{
								OnBtnStart();
							}
						}
					}

				}	break;

				case EC_RECEIVE_MEDIA_SAMPLE:
				{
					IMediaSample *pSample    = (IMediaSample*)evParam1;
//					long          nPackageNo = (long)evParam2;
//					ReceiveMediaSample(pSample, nPackageNo, (long)this);
					pSample->Release();
				} continue;	// User define Event: do not invoke FreeEventParams(..)!
				default:
					str.Format(_T("Event(%d, %x), %x, %x\r\n"), evCode, evCode, evParam1, evParam2);
					TRACE(str);
					break;
			}
			// Free event parameters to prevent memory leaks
			hr = m_pMediaEvent->FreeEventParams(evCode, evParam1, evParam2);
		}
		
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
HCURSOR CUSBCameraUnitDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon[0];
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnBtnInit() 
{
	if ((m_nSelectedCam != CB_ERR) && (m_nCamIndex != CB_ERR))
	{
		CDataExchange dx(this, false);
		if (!m_sCameraState[m_nCamIndex].bInitialized)
		{
#ifdef __TEST 
			InitCameraFilters2(m_nCamIndex);
#else
			InitCameraFilters(m_nCamIndex);
#endif
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnBtnUnInit() 
{
	if (m_nCamIndex != CB_ERR)
	{
		CDataExchange dx(this, false);
		if (m_sCameraState[m_nCamIndex].bInitialized)
		{
			ReleaseFilters(m_nCamIndex);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnBtnStart() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pMediaControl && (m_bStarted == FALSE))
	{
		HRESULT hr = m_pMediaControl->Run();
		if (SUCCEEDED(hr))
		{
			m_bStarted = TRUE;
			SetControlStates(m_sCameraState[m_nCamIndex]);
			SetVUIcon(ICON_RUNNING, FALSE);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnBtnStop() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (m_pMediaControl && (m_bStarted == TRUE))
	{
		HRESULT hr = m_pMediaControl->StopWhenReady();
		if (SUCCEEDED(hr))
		{
			m_bStarted = FALSE;
			SetControlStates(m_sCameraState[m_nCamIndex]);
			SetVUIcon(ICON_MAIN, FALSE);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnApplyNow() 
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, m_sKey, "");
	CWK_Profile wkpDev(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, m_sDeviceKey, "");

	int i, nSel;
	CString strSection, strGUID;// = GetString(CLSID_VideoInputDeviceCategory);
	for (i=0; i<MAX_CAMERAS; i++)
	{
		nSel = m_cCameraNo.GetItemData(i);
		strSection.Format(CAMERA_N, i+1);
		if (nSel != -1)
		{
			wkpDev.WriteString(strSection, VIDEO_INPUT  , m_sVideoInput[i]);
			wkpDev.WriteInt(   strSection, OUTPUT_FORMAT, m_nOutputFormat[i]);
			wkpDev.WriteInt(   strSection, OUTPUT_RATE  , m_nOutputRate[i]);
		}
		else
		{
			wkpDev.DeleteSection(strSection);
		}
	}
	
	if (m_cEncoders.GetCurSel() != CB_ERR)
	{
		m_cEncoders.GetLBText(m_cEncoders.GetCurSel(), m_sEncoder);
		strGUID = GetString(CLSID_VideoCompressorCategory);
		wkpDev.WriteString(SETTINGS_SECTION, strGUID, m_sEncoder);
	}

	wkpDev.WriteString(DEBUG_SETTINGS, OUT_FILE_NAME, m_strOutputFileName);
	wkp.WriteInt(   DEBUG_SETTINGS, STAND_ALONE, m_bStandalone);
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnSettingsCamera(UINT nID) 
{
	if ((m_nCamIndex != -1) && (m_pCapture[m_nCamIndex] != NULL))
	{
		ShowFilterPropertyPage(m_pCapture[m_nCamIndex], m_hWnd);
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnSettingsEncoder() 
{
	if ((m_nCamIndex != -1) && (m_pEncoder[m_nCamIndex] != NULL))
	{
		IAMVfwCompressDialogs *pCD = NULL;
		HRESULT hr = m_pEncoder[m_nCamIndex]->QueryInterface(IID_IAMVfwCompressDialogs, (void**)&pCD);
		if (SUCCEEDED(hr))
		{
			pCD->ShowDialog(VfwCompressDialog_Config, m_hWnd);
			RELEASE_OBJECT(pCD);
		}
		else
		{
			ShowFilterPropertyPage(m_pEncoder[m_nCamIndex], m_hWnd);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnSettingsEncoderInfo() 
{
	if ((m_nCamIndex != -1) && (m_pEncoder[m_nCamIndex] != NULL))
	{
		IAMVfwCompressDialogs *pCD = NULL;
		HRESULT hr = m_pEncoder[m_nCamIndex]->QueryInterface(IID_IAMVfwCompressDialogs, (void**)&pCD);
		if (SUCCEEDED(hr))
		{
			pCD->ShowDialog(VfwCompressDialog_About, m_hWnd);
			RELEASE_OBJECT(pCD);
		}
	}

}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnSettingsMd() 
{
	if ((m_nCamIndex != CB_ERR) && (m_sCameraState[m_nCamIndex].bMotionDetection))
	{
		m_pMD->OpenConfigDlg((WORD)m_nCamIndex);
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnSettingsSourceFilter() 
{
#ifdef _DEBUG
	if (m_pMediaSampleSource)
	{
		ShowFilterPropertyPage(m_pMediaSampleSource, m_hWnd);
	}
#endif
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnAddToRot() 
{
	if (m_dwRegister == 0)
	{
		AddToRot(m_pGraph);
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnRemoveFromRot()
{
	if (m_dwRegister)
	{
		IRunningObjectTable *pROT;
		if (SUCCEEDED(GetRunningObjectTable(0, &pROT)))
		{
		  pROT->Revoke(m_dwRegister);
		  pROT->Release();
		  m_dwRegister = 0;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnReleaseAllFilters()
{
	for (int i=0; i<MAX_CAMERAS; i++)
	{
		if (m_sCameraState[i].bInitialized)
		{
			ReleaseFilters(i);
		}
		else
		{
			RELEASE_OBJECT(m_pCapture[i]);
		}
	}
	ReleaseFilters(CB_ERR);
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnRequestReset() 
{
	OnBtnStop();
	InitMotionDetection();
	ReadRegistry();
	OnReleaseAllFilters();
	InitFunctions();
	
	if (m_pOutput)
	{
		m_pOutput->ConfirmReset();
	}
	if (m_pInput)
	{
		m_pInput->ConfirmReset();
	}
	if (m_pInputMDAlarm)
	{
		m_pInputMDAlarm->ConfirmReset();
	}
}
//////////////////////////////////////////////////////////////////////////
LRESULT CUSBCameraUnitDlg::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM, *pSettings;
			CPoint pt;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_CONTEXT_MENU);
			pM = menu.GetSubMenu(0);
			pSettings = pM->GetSubMenu(6);
			if (pSettings)
			{
				CString str, strFormat;
				pSettings->GetMenuString(ID_SETTINGS_CAMERA, strFormat, MF_BYCOMMAND);
				pSettings->DeleteMenu(ID_SETTINGS_CAMERA, MF_BYCOMMAND);
				for (int i=0; i<MAX_CAMERAS; i++)
				{
					if (m_pCapture[i])
					{
						str.Format(strFormat, i+1);
						pSettings->InsertMenu((UINT)-1, MF_BYPOSITION, ID_SETTINGS_CAMERA+i, str);
					}
				}
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
void CUSBCameraUnitDlg::OnAppAbout() 
{
	COemGuiApi::AboutDialog(this);
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnAppExit() 
{
	DestroyWindow();
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnSettingsDlg() 
{
	if (!m_bStandalone)
	{
		m_cCameraNo.SetCurSel(0);
		OnSelchangeComboCameraNo();
		SetControlStates(m_sCameraState[0]);
		ShowWindow(SW_SHOW);
	}
}
/////////////////////////////////////////////////////////////////////////////
long CUSBCameraUnitDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, theApp.GetApplicationId(), 0);
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnUpdateDbgAddToRot(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_dwRegister == 0);
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnUpdateDbgRemoveRot(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_dwRegister != 0);
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnUpdateSettingsCamera(CCmdUI* pCmdUI) 
{
	if ((m_nCamIndex != -1) && (m_pCapture[m_nCamIndex] != NULL))
	{
		pCmdUI->Enable(SupportsPropertyPage(m_pCapture[m_nCamIndex]));
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnUpdateSettingsEncoder(CCmdUI* pCmdUI) 
{
	if ((m_nCamIndex != -1) && (m_pEncoder[m_nCamIndex] != NULL))
	{
		IAMVfwCompressDialogs *pCD = NULL;
		HRESULT hr = m_pEncoder[m_nCamIndex]->QueryInterface(IID_IAMVfwCompressDialogs, (void**)&pCD);
		if (SUCCEEDED(hr))
		{
			hr = pCD->ShowDialog(VfwCompressDialog_QueryConfig, m_hWnd);
			pCmdUI->Enable(SUCCEEDED(hr));
			RELEASE_OBJECT(pCD);
		}
		else
		{
			pCmdUI->Enable(SupportsPropertyPage(m_pEncoder[m_nCamIndex]));
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnUpdateSettingsEncoderInfo(CCmdUI* pCmdUI) 
{
	if ((m_nCamIndex != -1) && (m_pEncoder[m_nCamIndex] != NULL))
	{
		IAMVfwCompressDialogs *pCD = NULL;
		HRESULT hr = m_pEncoder[m_nCamIndex]->QueryInterface(IID_IAMVfwCompressDialogs, (void**)&pCD);
		if (SUCCEEDED(hr))
		{
			hr = pCD->ShowDialog(VfwCompressDialog_QueryAbout, m_hWnd);
			pCmdUI->Enable(SUCCEEDED(hr));
			RELEASE_OBJECT(pCD);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnUpdateSettingsMd(CCmdUI* pCmdUI) 
{
	if ((m_nCamIndex != CB_ERR) && m_sCameraState[m_nCamIndex].bMotionDetection)
	{
		pCmdUI->Enable();
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnUpdateSettingsSourceFilter(CCmdUI* pCmdUI) 
{
#ifdef _DEBUG
	if (m_pMediaSampleSource)
	{
		pCmdUI->Enable(SupportsPropertyPage(m_pMediaSampleSource));
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
#else
		pCmdUI->Enable(FALSE);
#endif
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnUpdateAppAbout(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnUpdateBtnStart(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_btnStart.IsWindowEnabled());
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnUpdateBtnStop(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_btnStop.IsWindowEnabled());
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnUpdateReleaseAll(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((m_pGraph != NULL) && !m_btnStop.IsWindowEnabled());
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnUpdateRequestReset(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnCkShowPicture() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_SHOW_PICTUR, m_bShowPicture);
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnCkStillImage() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_STILL_IMAGE, m_bStillImage);
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnCkMotionDetection() 
{
	BOOL bMD;
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_MOTION_DETECTION, bMD);
	if (m_nCamIndex != CB_ERR)
	{
		m_sCameraState[m_nCamIndex].bMotionDetection = bMD;
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnSelchangeComboCameraNo() 
{
	int nCurSel = m_cCameraNo.GetCurSel();
	if ((nCurSel != CB_ERR) && (m_nCamIndex != nCurSel))
	{
		m_nCamIndex = nCurSel;
		BOOL bMD    = m_sCameraState[m_nCamIndex].bMotionDetection;
		CDataExchange dx(this, FALSE);
		DDX_Check(&dx, IDC_CK_MOTION_DETECTION, bMD);
		m_cCameras.SetCurSel(m_cCameraNo.GetItemData(nCurSel));
		OnSelchangeVideoSource();
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnSelchangeVideoSource() 
{
	int nCurSel = m_cCameras.GetCurSel();
	if ((nCurSel != CB_ERR) && (m_nSelectedCam != nCurSel) && (m_nCamIndex != CB_ERR))
	{
		m_nSelectedCam = nCurSel;
		AM_MEDIA_TYPE *pmt=NULL;
		int i;
		HRESULT   hr        = NOERROR;
		IMoniker *pMoniker  = NULL;
		IPin     *pPinOut   = NULL,
			      *pPin      = NULL;
		DeleteOutputFormats();
		try
		{
			IPropertyBag *pPropBag = NULL;
			VARIANT var;
			VariantInit(&var);
			if (m_pGraph == NULL)
			{
				InitBasicFilters();
			}

			pMoniker = (IMoniker*)m_cCameras.GetItemData(m_nSelectedCam);
			if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);

			if (m_pCapture[nCurSel] == NULL)
			{
				WCHAR szFilterName[64];
				swprintf(szFilterName, L"Capture Device (%d)", nCurSel+1);
				HR_EXCEPTION(AddFilterToGraph(m_pGraph, &m_pCapture[nCurSel], pMoniker, szFilterName));
			}
			m_sCameraState[nCurSel].nCapturePin = 0xf;
			m_sCameraState[nCurSel].nStillPin   = 0xf;
			for (i=0; ; i++)
			{
				hr = GetPin(m_pCapture[nCurSel], PINDIR_OUTPUT, i, &pPin);
				if FAILED(hr) break;

				IKsPropertySet *pPS = NULL;
				hr = pPin->QueryInterface(IID_IKsPropertySet, (void **)&pPS);
				GUID guid;
				if (SUCCEEDED(hr))
				{
					CString str;
					DWORD dwSize;
					pPS->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0,  &guid, sizeof(GUID), &dwSize);
					if (guid == PIN_CATEGORY_CAPTURE)
					{
						pPinOut = pPin;
						pPin = NULL;
						m_sCameraState[nCurSel].nCapturePin = i;
					}
					else if (guid == PIN_CATEGORY_STILL)
					{
						m_sCameraState[nCurSel].nStillPin = i;
					}
					GetGUIDString(str, &guid);
					pPS->Release();
				}
				RELEASE_OBJECT(pPin);
			}

			AddVideoStreamConfigCapsToList(m_pCapture[nCurSel], m_sCameraState[nCurSel].nStillPin, m_cOutputFormats2);
			
			if (pPinOut == NULL)
			{
				GetPin(m_pCapture[nCurSel], PINDIR_OUTPUT, 0, &pPinOut);
			}

			IAMStreamConfig *pCfg      = NULL;
			hr = pPinOut->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
			if (SUCCEEDED(hr))
			{
				VIDEO_STREAM_CONFIG_CAPS *pVSCC = NULL;
				CString  str, strWnd;
				int  iCount, iSize, nWeight, nMaxWeight = 0, nSelect = CB_ERR;

				pCfg->GetNumberOfCapabilities(&iCount, &iSize);
				m_ppVSccOutputFormats = (VIDEO_STREAM_CONFIG_CAPS**)CoTaskMemAlloc(iCount* sizeof(void*));

				for (i=0; i<iCount; i++)
				{
					nWeight = 0;
					pVSCC = (VIDEO_STREAM_CONFIG_CAPS*) CoTaskMemAlloc(iSize);
					pCfg->GetStreamCaps(i, &pmt, (BYTE*)pVSCC);

					GetGUIDString(strWnd, &pmt->subtype);
					int nFind = strWnd.Find(_T("_"));
					if (nFind != -1) strWnd = strWnd.Mid(nFind+1);
					str.Format(_T("%s (%d x %d)"), strWnd, pVSCC->InputSize.cx, pVSCC->InputSize.cy);
					if ((pVSCC->InputSize.cx == 352) && (pVSCC->InputSize.cy == 288))
					{
						str = str + _T(": CIF");
						nWeight |= 0x0000000f;
					}
					else if ((pVSCC->InputSize.cx == 176) && (pVSCC->InputSize.cy == 144))
					{
						str = str + _T(": QCIF");
						nWeight |= 0x0000000e;
					}
					else if ((pVSCC->InputSize.cx == 320) && (pVSCC->InputSize.cy == 240))
					{
						str = str + _T(": SIF");
					}
					else if ((pVSCC->InputSize.cx == 640) && (pVSCC->InputSize.cy == 480))
					{
						str = str + _T(": VGA");
					}
					else if ((pVSCC->InputSize.cx == 160) && (pVSCC->InputSize.cy == 120))
					{
						str = str + _T(": QSIF");
					}

					if         (WMMEDIASUBTYPE_I420 == pmt->subtype)  nWeight |= 0x000f0000;
					else if (  (MEDIASUBTYPE_YVYU   == pmt->subtype) 
						      ||(MEDIASUBTYPE_YUY2   == pmt->subtype)) nWeight |= 0x000f0000;
					else if (   MEDIASUBTYPE_ARGB32 == pmt->subtype)  nWeight |= 0x00050000;
					else if (   MEDIASUBTYPE_RGB32  == pmt->subtype)  nWeight |= 0x00040000;
					else if (   MEDIASUBTYPE_RGB24  == pmt->subtype)  nWeight |= 0x00030000;
					else if (   MEDIASUBTYPE_RGB565 == pmt->subtype)  nWeight |= 0x00020000;
					else if (   MEDIASUBTYPE_RGB555 == pmt->subtype)  nWeight |= 0x00010000;
					m_cOutputFormats.AddString(str);
					m_cOutputFormats.SetItemData(i, (ULONG)pmt);
					m_ppVSccOutputFormats[i] = pVSCC;
					if (nWeight > nMaxWeight)
					{
						nMaxWeight = nWeight;
						nSelect = i;
					}
				}
				RELEASE_OBJECT(pCfg);
				if (m_nOutputFormat[m_nCamIndex] != CB_ERR)
				{
					m_nOutputFormat[m_nCamIndex] = m_cOutputFormats.SetCurSel(m_nOutputFormat[m_nCamIndex]);
				}

				if (m_nOutputFormat[m_nCamIndex] == CB_ERR)
				{
					m_nOutputFormat[m_nCamIndex] = m_cOutputFormats.SetCurSel(nSelect);
				}
				OnSelchangeComboFormats();
				SetControlStates(m_sCameraState[m_nCamIndex]);
			}
		}
		catch(ErrorStruct *ps)
		{
			ReportError(ps, false);
			ReleaseFilters(nCurSel);
		}
		RELEASE_OBJECT(pPinOut);
	}
	else if (nCurSel == CB_ERR)
	{
		m_nSelectedCam = nCurSel;
		CameraStates s ={0};
		SetControlStates(s);
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnSelchangeComboFormats() 
{
	m_cFrameRates.ResetContent();
	int nSelOPF = m_cOutputFormats.GetCurSel();
	if ((nSelOPF != CB_ERR) && (m_nCamIndex != CB_ERR) && (m_pCapture[m_nCamIndex] != NULL))
	{
		HRESULT          hr        = NOERROR;
		IPin            *pPinOut   = NULL;
		int iPin = 0;
		if (m_sCameraState[m_nCamIndex].nCapturePin != 0xf) iPin = m_sCameraState[m_nCamIndex].nCapturePin;
		m_nOutputFormat[m_nCamIndex] = nSelOPF;
		hr = GetPin(m_pCapture[m_nCamIndex], PINDIR_OUTPUT, iPin, &pPinOut);
		if (SUCCEEDED(hr))
		{
			int nSelOPF = m_cOutputFormats.GetCurSel();
			IAMVideoControl *pVC = NULL;
			hr = m_pCapture[m_nCamIndex]->QueryInterface(IID_IAMVideoControl, (void**)&pVC);
			if (SUCCEEDED(hr))
			{
				long i, nSel = CB_ERR, lCount = 0;
				LONGLONG *pllFramRate = NULL;
				hr = pVC->GetFrameRateList(pPinOut, nSelOPF, m_ppVSccOutputFormats[nSelOPF]->InputSize, &lCount, &pllFramRate);
				if (SUCCEEDED(hr))
				{
					CString sText;
					double  dMin       = 10000.0, // ms
						     dReference = (double)m_nRefPictRate;
					if (m_nOutputRate[m_nCamIndex] != CB_ERR)
					{
						dReference = (double)m_nOutputRate[m_nCamIndex];
					}
					for (i=0; i<lCount; i++)
					{
						double dRateMS = pllFramRate[i]*100e-6;
						double dFps    = 1/dRateMS*1000; 
						sText.Format(_T("%.3f ms, %.1f fps"), dRateMS, dFps); // ms
						m_cFrameRates.InsertString(i, sText);
						m_cFrameRates.SetItemData(i, (int)dRateMS);
						dRateMS = fabs(dRateMS - dReference);
						if (dRateMS < dMin)
						{
							nSel = i;
							dMin = dRateMS;
						}
					}
					if (nSel != CB_ERR)
					{
						m_cFrameRates.SetCurSel(nSel);
						m_nOutputRate[m_nCamIndex] = m_cFrameRates.GetItemData(nSel);
					}
					CoTaskMemFree(pllFramRate);
				}
				else
				{
					m_cFrameRates.SetCurSel(CB_ERR);
				}
				RELEASE_OBJECT(pVC);
			}
			RELEASE_OBJECT(pPinOut);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnSelchangeComboFramerates() 
{
	int nCurSel = m_cFrameRates.GetCurSel();
	if (m_nCamIndex != CB_ERR && nCurSel != CB_ERR)
	{
		m_nOutputRate[m_nCamIndex] = m_cFrameRates.GetItemData(nCurSel);
	}
}
//////////////////////////////////////////////////////////////////////////
LRESULT CUSBCameraUnitDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (m_pVideoWindow)
	{
		switch (message) 
		{
		case WM_ACTIVATEAPP:
		case WM_DEVMODECHANGE:
		case WM_DISPLAYCHANGE:
		case WM_PALETTECHANGED:
		case WM_QUERYNEWPALETTE:
		case WM_SYSCOLORCHANGE:
		case WM_MOVE:
			m_pVideoWindow->NotifyOwnerMessage((OAHWND)m_hWnd, message, wParam, lParam);
			break;
		default:
			break;
		}
	}
	if (message == WM_DEVICECHANGE)
	{
		DEV_BROADCAST_HDR *pDBH = (DEV_BROADCAST_HDR*)lParam;
		if (pDBH)
		{
			if (pDBH->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE_EX)
			{
				CDevIntEx diex((DEV_BROADCAST_DEVICEINTERFACE_EX*)lParam);
				if (    ((wParam == DBT_DEVICEARRIVAL) || (wParam == DBT_DEVICEREMOVECOMPLETE))
					  && (diex.GetGuidClass() == AM_KSCATEGORY_VIDEO) 
					  && !GetTimerByID(TIMER_DEV_CHANGED))
				{
					{
						SetTimer(TIMER_DEV_CHANGED, 100);
					}
				}
			}
		}
	}

	return CDialog::WindowProc(message, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////////
LRESULT CUSBCameraUnitDlg::OnEnterMenuLoop(WPARAM wParam, LPARAM lParam)
{
	COemGuiApi::DoUpdatePopupMenu(AfxGetMainWnd(), GetMenu());
	return 0;
}
//////////////////////////////////////////////////////////////////////////
int CUSBCameraUnitDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_bStandalone)
	{	
		ShowWindow(SW_SHOW);
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
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnDestroy() 
{
	// Kill all Timers
	POSITION pos = m_TimerEvents.GetStartPosition();
	UINT nEvent;
	DWORD dwValue;
	while (pos)
	{
		m_TimerEvents.GetNextAssoc(pos, nEvent, dwValue);
		if (HIWORD(dwValue))
		{
			KillTimer(nEvent);
		}
	}
	m_TimerEvents.RemoveAll();

	m_bIsShuttingDown = TRUE;
#ifdef _DEBUG
	WK_DELETE(m_pCIPCMedia);
//	ClearVideoStreamConfigCapsList(m_cTestFilters);
#endif

	OnBtnStop();
	WK_DELETE(m_pMD);
	WK_DELETE(m_pInputMDAlarm);
	WK_DELETE(m_pInput);
	WK_DELETE(m_pOutput);
	RemovePendigJobs();
	OnRemoveFromRot();
	DeleteOutputFormats();
	OnReleaseAllFilters();
	ClearFilterListWithMoniker(m_cCameras);
	ClearFilterListWithMoniker(m_cEncoders);
	
	CoUninitialize();

	if (!m_bStandalone)
	{
		NOTIFYICONDATA tnd;

		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		   = m_hWnd;
		tnd.uID			= 1;
		tnd.uFlags		= 0;

		Shell_NotifyIcon(NIM_DELETE, &tnd);
		PostQuitMessage(0);
	}

	CDialog::OnDestroy();
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::PostNcDestroy() 
{
	delete this;
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnOK() 
{
	DestroyWindow();
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnCancel() 
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
//////////////////////////////////////////////////////////////////////////
// Helperfunctions
void CUSBCameraUnitDlg::InitBasicFilters()
{
	HRESULT hr;
	// Create the Filter Graph Manager.
	if (m_pGraph == NULL)
	{
		HR_EXCEPTION(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGraph));
		// Create the Capture Graph Builder.
		HR_EXCEPTION(CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **)&m_pGraphBuilder));
		// Associate the graph with the builder.
		HR_EXCEPTION(m_pGraphBuilder->SetFiltergraph(m_pGraph));
		HR_EXCEPTION(m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl));
		HR_EXCEPTION(m_pGraph->QueryInterface(IID_IMediaEventEx, (void **)&m_pMediaEvent));
		HR_REPORT(m_pMediaEvent->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPHNOTIFY, 0));
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::InitCameraFilters(int nCam)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	HRESULT hr; 
	IPin            *pPin        = NULL,
					*pCapturePin = NULL,
					*pStillPin   = NULL;
	IDump           *pIDump    = NULL;
	IInfTee         *pIInfTee  = NULL;
	IFileSinkFilter *pFileSink = NULL;
	IBaseFilter     *pFilter   = NULL,
					*pDump     = NULL,
					*pInfTee   = NULL,
					*pDecoder  = NULL;
	IAMStreamConfig *pCfg      = NULL;
	IAMBufferNegotiation *pBN  = NULL;							// Suggest the allocator properties
	FILTER_INFO     fi = {NULL, NULL};
	WCHAR           szFilterName[64];
	int             iPin;
	CString         sMediaType;

	TRACE(_T("InitCameraFilters(%d)"), nCam);
	try
	{	
		if (nCam != CB_ERR)
		{
			if (m_pGraph == NULL)
			{
				InitBasicFilters();
			}
			BOOL bDeletePMTR = FALSE;
			if (m_pCapture[nCam] == NULL)							// Kamera GrabberFilter
			{																// initialisieren, wenn nötig
				IMoniker*pMoniker = (IMoniker*)m_cCameras.GetItemData(m_cCameraNo.GetItemData(nCam));
				if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);

				if (m_pCapture[nCam] == NULL)
				{
					swprintf(szFilterName, L"Capture Device (%d)", nCam+1);
					HR_EXCEPTION(AddFilterToGraph(m_pGraph, &m_pCapture[nCam], pMoniker, szFilterName));
				}
			}
			
			iPin = 0;
			if (m_sCameraState[nCam].nCapturePin != 0xf) iPin = m_sCameraState[nCam].nCapturePin;
			HR_EXCEPTION(GetPin(m_pCapture[nCam], PINDIR_OUTPUT, iPin, &pPin));

			hr = pPin->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
			AM_MEDIA_TYPE *pmtr = NULL,
				           *pmt  = (AM_MEDIA_TYPE*)m_cOutputFormats.GetItemData(m_cOutputFormats.GetCurSel());
			if (SUCCEEDED(hr))											// Framerate einstellen bzw. ermitteln
			{
				if (pmt && (CB_ERR != (UINT)pmt))
				{
					if (pmt->formattype == FORMAT_VideoInfo)
					{
						CString str;
						VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)pmt->pbFormat;
						if (m_cFrameRates.GetCurSel() != CB_ERR)	// Framerate einstellen
						{
							m_cFrameRates.GetLBText(m_cFrameRates.GetCurSel(), str);
							pVIH->AvgTimePerFrame = (LONGLONG) (_tstof(str) * 1e4);
						}
						else													// Framerate ermitteln
						{
							str.Format(_T("%f ms"), pVIH->AvgTimePerFrame*100e-6); // ms
							m_cFrameRates.SetCurSel(m_cFrameRates.FindString(-1, str));
						}
					}
					hr = pCfg->SetFormat(pmt);
				}
				RELEASE_OBJECT(pCfg);
			}

			if ((MEDIASUBTYPE_YVYU != pmt->subtype) && (MEDIASUBTYPE_YUY2 != pmt->subtype))
			{
				GetGUIDString(sMediaType, &pmt->subtype, TRUE);
				BSTR bstr = sMediaType.AllocSysString();
				swprintf(szFilterName, L"%d. %s to YUYV Decoder", nCam+1, bstr);
				SysFreeString(bstr);
																				// Filter zur Umwandlung auf YUYV Format suchen
				DWORD dwMerit = MERIT_NORMAL;							// für die Verarbeitung der Videodaten

				if (   (MEDIASUBTYPE_RGB555 == pmt->subtype) || (MEDIASUBTYPE_RGB565 == pmt->subtype)
					 || (MEDIASUBTYPE_RGB24  == pmt->subtype) || (MEDIASUBTYPE_RGB32  == pmt->subtype)
					 || (MEDIASUBTYPE_ARGB32 == pmt->subtype))	// rgb formats can be rendered
				{
//					hr = AddFilterToGraph(m_pGraph, &pDecoder, CLSID_RGB2YUV, szFilterName);
//					if (FAILED(hr))
//					{
						dwMerit = MERIT_DO_NOT_USE;					// with lower Merit
//					}
//					else
//					{
//						dwMerit = 0;
//					}
				}

				if (dwMerit)
				{
					GUID subtypes[2];
					subtypes[0] = MEDIASUBTYPE_YVYU;						// desired format is YUYV
					subtypes[1] = MEDIASUBTYPE_YUY2;
					hr = FindMatchingFilter(dwMerit, pPin, TRUE, pmt->majortype, pmt->subtype, TRUE, pmt->majortype, subtypes, 2, FALSE, &pDecoder, &pmtr, szFilterName);
					bDeletePMTR = TRUE;
				}
			}
			RELEASE_OBJECT(pPin);

			swprintf(szFilterName, L"%d. Infinite Tee", nCam+1);	// 
			HR_EXCEPTION(AddFilterToGraph(m_pGraph, &pInfTee, CLSID_InfiniteTee, szFilterName));

			HR_EXCEPTION(GetPin(pInfTee, PINDIR_INPUT , 0, &pPin));
			IInfTee *pIInfTee = NULL;
			HR_EXCEPTION(pPin->QueryInterface(IID_IInfiniteTee, (void**)&pIInfTee));
			HR_EXCEPTION(pIInfTee->SetDataCheckFunction(0, CheckData, (long)nCam));
			RELEASE_OBJECT(pIInfTee);
			RELEASE_OBJECT(pPin);

			swprintf(szFilterName, L"%d. Sample Dump", nCam+1);
			HR_EXCEPTION(AddFilterToGraph(m_pGraph, &pDump, CLSID_Sample2MemRenderer, szFilterName));

			ISample2MemRenderer *pIDump = NULL;
			HR_EXCEPTION(pDump->QueryInterface(IID_ISample2MemRenderer, (void **)&pIDump));
			if (m_strOutputFileName.IsEmpty())
			{																		// set the function to receive IMediaSample
				hr = pIDump->SetReceiveFunction(ReceiveMediaSample, (long)nCam);
			}
			RELEASE_OBJECT(pIDump);

			if (pDecoder)
			{																		// connect Decoder to InfiniteTee with specific MediaType
				HR_EXCEPTION(ConnectFilters(m_pGraph, pDecoder, 0, pInfTee, 0, pmtr));
				if (!bDeletePMTR)												// MediaType ermitteln
				{
					AM_MEDIA_TYPE mt;
					hr = GetPin(pDecoder, PINDIR_OUTPUT, 0, &pPin);
					if (SUCCEEDED(hr))
					{
						pPin->ConnectionMediaType(&mt);
						pmtr = CreateMediaType(&mt);
						bDeletePMTR = TRUE;
					}
					RELEASE_OBJECT(pPin);
				}
			}
			else
			{
				HR_EXCEPTION(ConnectFilters(m_pGraph, m_pCapture[nCam], 0, pInfTee, 0, pmtr));
			}

			int nSelectedEncoder = m_cEncoders.GetCurSel();
			if (nSelectedEncoder > 0)
			{
				IMoniker*pMoniker = (IMoniker*)m_cEncoders.GetItemData(nSelectedEncoder);
				if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);

				swprintf(szFilterName, L"%d. Encoder Device", nCam+1);
				HR_EXCEPTION(AddFilterToGraph(m_pGraph, &m_pEncoder[nCam], pMoniker, szFilterName));
				
				hr = ConnectFilters(m_pGraph, pInfTee, 0, m_pEncoder[nCam], 0, pmtr);
				if (FAILED(hr))					// Bei Fehler AVI Decompressor zwischenschalten
				{
					swprintf(szFilterName, L"%d. AVI Decompressor", nCam+1);
					HR_EXCEPTION(AddFilterToGraph(m_pGraph, &pFilter, CLSID_AVIDec, szFilterName));
					// connect InfiniteTee to AVI Decompressor
					HR_EXCEPTION(ConnectFilters(m_pGraph, pInfTee, 0, pFilter, 0, pmtr));
					// connect AVI Decompressor to Encoder
					HR_EXCEPTION(ConnectFilters(m_pGraph, pFilter, 0, m_pEncoder[nCam], 0));
					RELEASE_OBJECT(pFilter);
				}
				// connect Encoder to Dump
				HR_EXCEPTION(ConnectFilters(m_pGraph, m_pEncoder[nCam], 0, pDump, 0));
			}
			else
			{	// connect InfiniteTee to Dump
				HR_EXCEPTION(ConnectFilters(m_pGraph, pInfTee, 0, pDump, 0, pmtr));
			}
			RELEASE_OBJECT(pDump);

			if (pmtr)
			{
				CString sMajorType, sSubType, sFormat;
				GetGUIDString(sMajorType, &pmtr->majortype, TRUE);
				GetGUIDString(sSubType, &pmtr->subtype, TRUE);
				GetGUIDString(sFormat, &pmtr->formattype, TRUE);
				sMediaType = sMajorType + _T(", ") + sSubType + _T(", ") + sFormat;
			}
			SetDlgItemText(IDC_STR_MEDIA_SUB_TYPE, sMediaType);

			if (bDeletePMTR) SAFE_DELETEMEDIATYPE(pmtr);

			if (   (m_bShowPicture)
			    && (m_pVideoWindow == NULL))									// Es kann nur einen geben
			{
				hr = GetPin(pInfTee, PINDIR_OUTPUT, 1, &pPin);			// 2. Pin zum Darstellen
				if (SUCCEEDED(hr))
				{
/*
					if (m_pVideoWindow)	// Disconnect Pin führt zu Fehlern
					{
						IBaseFilter *pVideo;
						hr = m_pGraph->FindFilterByName(L"Video Renderer", &pVideo);
						if (SUCCEEDED(hr))
						{
							IPin*pPinI = NULL, *pPinO;
							hr = GetPin(pVideo, PINDIR_INPUT, 0, &pPinI);
							if (SUCCEEDED(hr))
							{
								pPinI->ConnectedTo(&pPinO);
								m_pGraph->Disconnect(pPinO);
								m_pGraph->Disconnect(pPinI);
							}
							RELEASE_OBJECT(pPinO);
							RELEASE_OBJECT(pPinI);
							RELEASE_OBJECT(pVideo);
						}
					}
*/
					hr =m_pGraph->Render(pPin);
					RELEASE_OBJECT(pPin);
					if (SUCCEEDED(hr))
					{
						InitVideoDisplay();
					}
				}
			}

			if ((   m_bStillImage											// Standbildbutton auswerten
				  || (m_pInput && m_pInput->IsAlarmActive(nCam)))	
				  && m_sCameraState[nCam].nStillPin != 0xf)			// wenn vorhanden
			{
				swprintf(szFilterName, L"%d. Still Dump", nCam+1);
				hr = AddFilterToGraph(m_pGraph, &pDump, CLSID_Sample2MemRenderer, szFilterName);
				if (SUCCEEDED(hr))
				{
					hr = pDump->QueryInterface(IID_ISample2MemRenderer, (void **)&pIDump);
					if (SUCCEEDED(hr))
					{																			// set the function to receive IMediaSample
						pIDump->SetReceiveFunction(ReceiveStillImage, (long)nCam);
						AM_MEDIA_TYPE *pmt  = (AM_MEDIA_TYPE*)m_cOutputFormats2.GetItemData(m_cOutputFormats2.GetCurSel());
						if ((UINT)pmt == CB_ERR) pmt = NULL;
						hr = ConnectFilters(m_pGraph, m_pCapture[nCam], m_sCameraState[nCam].nStillPin, pDump, 0, pmt);
						if (FAILED(hr))
						{
							if (pmt) pIDump->SetAcceptedMediaTypes(pmt, 1, true);
							hr = m_pGraphBuilder->RenderStream(&PIN_CATEGORY_STILL, &MEDIATYPE_Video, m_pCapture[nCam], NULL, pDump);
						}

						if (FAILED(hr)) ReportError(HRESULT_ERROR_AT_POS(hr));
					}
					RELEASE_OBJECT(pIDump);
					RELEASE_OBJECT(pDump);
				}
			}

			m_sCameraState[nCam].bInitialized = TRUE;
			SetControlStates(m_sCameraState[nCam]);
		}
	}
   catch(ErrorStruct *ps)
	{
		ReportError(ps);
		ReleaseFilters(nCam);
	}

	RELEASE_OBJECT(pBN);
	RELEASE_OBJECT(pPin);
	RELEASE_OBJECT(pFilter);
	RELEASE_OBJECT(pDump);
	RELEASE_OBJECT(pIDump);
	RELEASE_OBJECT(pInfTee);
	RELEASE_OBJECT(pIInfTee);
	RELEASE_OBJECT(pFileSink);
	RELEASE_OBJECT(pDecoder);
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::InitCameras()
{
	WORD i;
	DWORD dwCameras = 0, dwMask = 1;
	
	for (i=0; i<MAX_CAMERAS; i++)
	{
		if (IsCameraPresent(i) && IsCameraActive(i) && !IsCameraInitialised(i))
		{
			m_cCameraNo.SetCurSel(i);
			OnSelchangeComboCameraNo();
			InitCameraFilters(m_nSelectedCam);
			if (m_sCameraState[i].bInitialized)
			{
				dwCameras |= dwMask;
			}
		}
	}
	if (dwCameras)
	{
		OnBtnStart();
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::ReleaseFilters(int nCam)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	OnBtnStop();
	if (nCam == CB_ERR)
	{
		RELEASE_OBJECT(m_pMediaControl);
		if (m_pMediaEvent)
		{
			m_pMediaEvent->SetNotifyWindow(NULL, 0, 0);
		}
		RELEASE_OBJECT(m_pMediaEvent);
		if (m_pVideoWindow)
		{
			m_pVideoWindow->put_WindowStyle(0);
			m_pVideoWindow->put_Owner(NULL);
			m_pVideoWindow->put_MessageDrain(NULL);
			RELEASE_OBJECT(m_pVideoWindow);
		}
#ifdef _DEBUG
		if (m_pMediaSampleSource)
		{
			m_pGraph->RemoveFilter(m_pMediaSampleSource);
			RELEASE_OBJECT(m_pIMediaSampleSource);
			RELEASE_OBJECT(m_pMediaSampleSource);
		}
#endif
		
		if (m_pGraphBuilder)
		{
			m_pGraphBuilder->SetFiltergraph(NULL);
		}

		RELEASE_OBJECT(m_pGraph);
		RELEASE_OBJECT(m_pGraphBuilder);
	}
	else
	{
		HRESULT       hr;
		IEnumFilters *pEnum = NULL;
		IBaseFilter  *pFilter = NULL;
		ULONG         cFetched;
		WCHAR         szCam[8];
		int           nLen;
		CPtrList      Filters;

		swprintf(szCam, L"%d.", nCam+1);
		nLen = wcslen(szCam);
		if (m_pGraph)
		{
			hr = m_pGraph->EnumFilters(&pEnum);	// Finde die Filter der Kamera (n)
			if (SUCCEEDED(hr))
			{
				while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
				{
					FILTER_INFO FilterInfo;
					hr = pFilter->QueryFilterInfo(&FilterInfo);
					if (SUCCEEDED(hr))
					{
						if (wcsncmp(FilterInfo.achName, szCam, nLen) == 0)
						{
							Filters.AddTail(pFilter);
							pFilter = NULL;
						}
						FilterInfo.pGraph->Release();
					}       
					RELEASE_OBJECT(pFilter);
				}
				pEnum->Release();
			}

			if (m_pCapture[nCam])
			{
				m_pGraph->RemoveFilter(m_pCapture[nCam]);
				// Achtung: wird ein USB Gerät entfernt, so kommt es zu einer Assertion
				// in der Ksutil.cpp (10178). Die scheint beim Disconnect der Pins zum
				// m_pCapture[nCam]-Gerät zu passieren.
				// In der Release version von DirectShow ist das hoffentlich anders !!
				m_sCameraState[nCam].bInitialized = FALSE;
			}
			if (m_pEncoder[nCam])
			{
				m_pGraph->RemoveFilter(m_pEncoder[nCam]);
			}
			
			while (Filters.GetCount())
			{
				pFilter = (IBaseFilter*)Filters.RemoveHead();
				m_pGraph->RemoveFilter(pFilter);
				RELEASE_OBJECT(pFilter);
			}

#ifdef _DEBUG
			if (m_pMediaSampleSource)
			{
				m_pGraph->RemoveFilter(m_pMediaSampleSource);
				RELEASE_OBJECT(m_pIMediaSampleSource);
				RELEASE_OBJECT(m_pMediaSampleSource);
			}
#endif
		}
	
		RELEASE_OBJECT(m_pCapture[nCam]);
		RELEASE_OBJECT(m_pEncoder[nCam]);
		m_sCameraState[nCam].bInitialized = FALSE;
		m_sCameraState[nCam].bVideoPlay   = FALSE;
		SetControlStates(m_sCameraState[nCam]);
		SetDlgItemText(IDC_STR_MEDIA_SUB_TYPE, _T("--"));
		SetDlgItemText(IDC_STR_OUTPUT_SUB_TYPE, _T("--"));
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::DeleteOutputFormats()
{
	AM_MEDIA_TYPE *pmt;
	int i, nCount = m_cOutputFormats.GetCount();
	for (i=0; i<nCount; i++)
	{
		pmt = (AM_MEDIA_TYPE*)m_cOutputFormats.GetItemData(i);
		if (pmt)
		{
			DeleteMediaType(pmt);
		}
		SAFE_COTASKMEMFREE(m_ppVSccOutputFormats[i]);
	}
	SAFE_COTASKMEMFREE(m_ppVSccOutputFormats);
	m_cOutputFormats.ResetContent();
}
//////////////////////////////////////////////////////////////////////////
HRESULT WINAPI CUSBCameraUnitDlg::CheckData(IMediaSample*pSample, long lParam)
{
	ASSERT(gm_pThis != NULL);
	return gm_pThis->OnCheckData(pSample, (WORD)lParam);
}
//////////////////////////////////////////////////////////////////////////
HRESULT CUSBCameraUnitDlg::OnCheckData(IMediaSample*pSample, WORD wSource)
{
	#ifdef _DEBUG
	{
		AM_MEDIA_TYPE *pMT=NULL;
		if (pSample->IsDiscontinuity() == S_OK)
		{
			pSample->GetMediaType(&pMT);
			if (pMT)
			{
				CString sMajorType, sSubType, sFormat;
				GetGUIDString(sMajorType, &pMT->majortype, TRUE);
				GetGUIDString(sSubType, &pMT->subtype, TRUE);
				GetGUIDString(sFormat, &pMT->formattype, TRUE);
				if (sSubType == _T("NULL"))
				{
					FOURCCMap map;
					map.SetFOURCC(&pMT->subtype);
					DWORD dwFourCC = map.GetFOURCC();
					char cTemp = 0;
					sSubType = CString((char*)&dwFourCC).Left(4);
				}
				SetDlgItemText(IDC_STR_MEDIA_SUB_TYPE, sMajorType + _T(", ") + sSubType + _T(", ") + sFormat);
				::DeleteMediaType(pMT);
			}
		}		
	}
	#endif

	HRESULT          hr    = E_FAIL;
	m_csVideoJobs.Lock();
	if (m_VideoJobList[wSource].GetCount())
	{
		CVideoJob *pJob = m_VideoJobList[wSource].GetTail();
		if (pJob)
		{
			if (pJob->m_iOutstandingPics > 0)
			{
				pJob->m_iOutstandingPics--;
				hr = NOERROR;													// dieses Bild wird geliefert
			}
		}
	}
	m_csVideoJobs.Unlock();

	if (   m_pMD	// Motion Detection Object Present
		 && IsBetween(wSource, 0, MAX_CAMERAS) 
		 && m_sCameraState[wSource].bMotionDetection)
	{
		AM_MEDIA_TYPE   *pMT   = NULL;
		pSample->GetMediaType(&pMT);
		if (pMT)
		{
			if ((MEDIASUBTYPE_YVYU == pMT->subtype) || (MEDIASUBTYPE_YUY2 == pMT->subtype))
			{
				VIDEOINFOHEADER *pVIH  = (VIDEOINFOHEADER*)pMT->pbFormat;
				BYTE            *pData = NULL;;
				int              nLen  = pSample->GetActualDataLength();
				pSample->GetPointer(&pData);

				CAPTURE_STRUCT   Capture = {0};
				Capture.wSource = wSource;
				Capture.wCompressionType = COMPRESSION_YUV_422;
				Capture.Buffer.dwLen     = nLen;
				Capture.Buffer.pLinAddr  = pData;
				Capture.wSizeH           = (WORD)(pVIH->bmiHeader.biWidth);
				Capture.wSizeV           = (WORD)(pVIH->bmiHeader.biHeight);
				BOOL bMD = m_pMD->MotionCheck(Capture, m_mdPoints[Capture.wSource]);
				if (bMD == MD_POINTS_RECEIVED)
				{
					if (!m_bStandalone)
					{
						if (m_pInputMDAlarm)
						{
							m_pInputMDAlarm->OnReceivedMotionAlarm(wSource, m_mdPoints[Capture.wSource], TRUE);
							m_pInputMDAlarm->OnReceivedMotionAlarm(wSource, m_mdPoints[Capture.wSource], FALSE);
						}
					}
					if (!GetTimerByID(ICON_RUNNING))
					{
						SetVUIcon(ICON_RECORDING, FALSE);
						SetButtonIcon(IDC_RECORDING, IDC_RECORDING);
						SetTimer(ICON_RUNNING, 200);
					}
					m_mdPoints[Capture.wSource].Reset();
					hr = NOERROR;												// diese Bild wird potentiell geliefert
				}
				DeleteMediaType(pMT);
			}
		}
	}
	else
	{
		hr = NOERROR;												// diese Bild wird potentiell geliefert
	}

#ifdef TEST_WITH_SAVIC 
	m_csVideoJobs.Lock();
	if (m_SyncJob.GetCount())
	{
		CVideoJob *pJob = m_SyncJob.GetTail();
		m_SyncJob.RemoveTail();
		WK_DELETE(pJob);
		if (m_hSyncSemaphore)
		{
			ReleaseSemaphore(m_hSyncSemaphore, 1, NULL);
		}
	}
	m_csVideoJobs.Unlock();
#endif
	
	return hr;
}
//////////////////////////////////////////////////////////////////////////
HRESULT WINAPI CUSBCameraUnitDlg::ReceiveMediaSample(IMediaSample *pSample, long nPackageNo, long lParam)
{
	ASSERT(gm_pThis != NULL);
	return gm_pThis->OnReceiveMediaSample(pSample, nPackageNo, (WORD)lParam);
}
//////////////////////////////////////////////////////////////////////////
HRESULT CUSBCameraUnitDlg::OnReceiveMediaSample(IMediaSample*pSample, long nPackageNo, WORD wSource)
{
#ifdef _DEBUG
	if (m_pCIPCMedia && m_pCIPCMedia->IsConnected())
	{
		MediaSampleProperties msp;
		CIPCMediaVideo *pME = m_pCIPCMedia;
		pSample->GetTime(&msp.rtTimeStart, &msp.rtTimeEnd);
		pSample->GetMediaTime(&msp.llTimeStart, &msp.llTimeEnd);
		pSample->GetPointer(&msp.pBuffer);
		CIPCMediaSampleRecord rec(pME, pME->GetSecID());
		rec.SetData(nPackageNo, pSample->GetActualDataLength(), msp.pBuffer);
		rec.SetTimes(msp.llTimeStart, msp.llTimeEnd, msp.rtTimeStart, msp.rtTimeEnd);
		rec.SetMediaInfo(pSample->IsPreroll(), pSample->IsDiscontinuity() == S_OK, pSample->IsSyncPoint());
		
		if (nPackageNo == 0)
		{
			HRESULT hr;
			AM_MEDIA_TYPE *pmt = NULL;
			hr = pSample->GetMediaType(&pmt);
			ALLOCATOR_PROPERTIES ap = {1, 0, 1, 0};
			IBaseFilter *pFilter = NULL;
			ISample2MemRenderer *pIDump = NULL;
			WCHAR szFilterName[128];
			swprintf(szFilterName, L"%d. Sample Dump", m_nCamIndex+1);
			hr = m_pGraph->FindFilterByName(szFilterName, &pFilter);
			if (SUCCEEDED(hr))
			{
				hr = pFilter->QueryInterface(IID_ISample2MemRenderer, (void **)&pIDump);
				if (SUCCEEDED(hr))
				{
					hr = pIDump->GetAllocatorProperties(&ap);
					if (FAILED(hr))
					{
						ap.cbAlign  = 1;
						ap.cbBuffer = pSample->GetSize();
						ap.cbPrefix = 0;
						ap.cBuffers = 1;
					}
				}
			}
			RELEASE_OBJECT(pFilter);
			RELEASE_OBJECT(pIDump);

			if (SUCCEEDED(hr) && pmt)
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
		}
		rec.CreateBubble();
		pME->DoIndicateMediaData(rec, pME->GetSecID(), 0);
	}
#endif

	AM_MEDIA_TYPE *pMT=NULL;
	if (nPackageNo == 0)
	{
		pSample->GetMediaType(&pMT);
		if (pMT)
		{
			CString sMajorType, sSubType, sFormat;
			GetGUIDString(sMajorType, &pMT->majortype, TRUE);
			GetGUIDString(sSubType, &pMT->subtype, TRUE);
			GetGUIDString(sFormat, &pMT->formattype, TRUE);
			if (sSubType == _T("NULL"))
			{
				FOURCCMap map;
				map.SetFOURCC(&pMT->subtype);
				DWORD dwFourCC = map.GetFOURCC();
				char cTemp = 0;
				sSubType = CString((char*)&dwFourCC).Left(4);
			}
			SetDlgItemText(IDC_STR_OUTPUT_SUB_TYPE, sMajorType + _T(", ") + sSubType + _T(", ") + sFormat);
			::DeleteMediaType(pMT);
		}
	}
	
	if (m_pOutput && m_pOutput->IsConnected())
	{
		IMAGE	Image;
		ZeroMemory(&Image, sizeof(IMAGE));
		pSample->GetMediaType(&pMT);
		VIDEOINFOHEADER *pVIH  = (VIDEOINFOHEADER*)pMT->pbFormat;
		Image.pHeaderData = (BYTE*)&pVIH->bmiHeader;
		Image.dwHeaderLen = pVIH->bmiHeader.biSize;

		pSample->GetPointer(&Image.pImageData);
		Image.dwImageLen = pSample->GetActualDataLength();

		CSystemTime st;														// set the system time
		st.GetLocalTime();
		Image.TimeStamp.bValid		= TRUE;
		Image.TimeStamp.wYear		= st.GetYear();
		Image.TimeStamp.wMonth		= st.GetMonth();
		Image.TimeStamp.wDay		= st.GetDay();
		Image.TimeStamp.wHour		= st.GetHour();
		Image.TimeStamp.wMinute		= st.GetMinute();
		Image.TimeStamp.wSecond		= st.GetSecond();
		Image.TimeStamp.wMSecond	= st.GetMilliseconds();

		Image.wSource = wSource;
		Image.wSizeH  = (WORD)pVIH->bmiHeader.biWidth;
		Image.wSizeV  = (WORD)pVIH->bmiHeader.biHeight;
		if (  (MEDIASUBTYPE_YVYU   == pMT->subtype) 
			 ||(MEDIASUBTYPE_YUY2   == pMT->subtype))
		{
			Image.wCompressionType = COMPRESSION_YUV_422;
			pVIH->bmiHeader.biCompression = mmioFOURCC('Y', '4', '2', '2');
		}
		else if (pMT->subtype == MEDIASUBTYPE_RGB32)
		{
			Image.wCompressionType = COMPRESSION_RGB_24;
		}
		else
		{
			CString str;
			GetGUIDString(str, &pMT->subtype, TRUE);
			str.MakeUpper();
			if (   str.Find(_T("JPG")) != -1
				|| str.Find(_T("MJPG")) != -1
				|| str.Find(_T("JPEG")) != -1)
			{
				Image.wCompressionType = COMPRESSION_JPEG;
			}
			else if (   str.Find(_T("MPG4")) != -1
				     || str.Find(_T("XVID")) != -1
					 || str.Find(_T("DIVX")) != -1)
			{
				Image.wCompressionType = COMPRESSION_MPEG4;
			}
		}

		m_csVideoJobs.Lock();
		if (m_VideoJobList[wSource].GetCount())
		{
			CVideoJob *pJob = m_VideoJobList[wSource].GetTail();
			if (pJob)
			{
				Image.dwProzessID	= pJob->m_dwUserData;
	//			Image.wCompressionType  = pJob->m_compression;
				if (pJob->m_iOutstandingPics == 0)
				{
					m_VideoJobList[wSource].RemoveTail();
					WK_DELETE(pJob);
				}
				Image.bValid = TRUE;
			}
		}

		if (m_bDTS)	// Bei DTS wird das Bild auch ohne dwProzessID akzeptiert !!
		{
			Image.bValid = TRUE;
		}
		m_csVideoJobs.Unlock();
		if (Image.bValid)
		{
			m_pOutput->IndicationDataReceived(&Image, TRUE);
		}
		DeleteMediaType(pMT);
	}

	// S_FALSE liefern, wenn die Daten nicht an die Schnittstelle geliefert werden können
	return NOERROR;
}
//////////////////////////////////////////////////////////////////////////
HRESULT WINAPI CUSBCameraUnitDlg::ReceiveStillImage(IMediaSample *pSample, long nPackageNo, long lParam)
{
	ASSERT(gm_pThis != NULL);
	return gm_pThis->OnReceiveStillImage(pSample, nPackageNo, (WORD)lParam);
}
//////////////////////////////////////////////////////////////////////////
HRESULT CUSBCameraUnitDlg::OnReceiveStillImage(IMediaSample*pSample, long nPackageNo, WORD wSource)
{
	if (m_bStandalone)
	{
		AM_MEDIA_TYPE *pmt;
		pSample->GetMediaType(&pmt);
		BYTE *pBuffer;
		pSample->GetPointer(&pBuffer);
		CFile sFile(_T("C:\\StillImage.bmp"), CFile::modeCreate|CFile::modeWrite);
		VIDEOINFOHEADER*pVIH = (VIDEOINFOHEADER*)pmt->pbFormat;
		BITMAPFILEHEADER bfh;
		memcpy(&bfh.bfType, "BM", 2);
		bfh.bfSize = sizeof(BITMAPFILEHEADER) + pVIH->bmiHeader.biSize + pSample->GetActualDataLength();
		bfh.bfReserved1 = 0;
		bfh.bfReserved2 = 0;
		bfh.bfOffBits   = pVIH->bmiHeader.biSize;

		sFile.Write(&bfh, sizeof(BITMAPFILEHEADER));
		sFile.Write(&pVIH->bmiHeader, pVIH->bmiHeader.biSize);
		sFile.Write(pBuffer, pSample->GetActualDataLength());
		DeleteMediaType(pmt);
	}
	else if (m_pInput)
	{
		m_pInput->AlarmStateChanged(wSource, true);
	}
	SetButtonIcon(IDC_STILL, IDC_RECORDING);
	SetTimer(TIMER_ALARM_MIN+wSource, 1000);
	return NOERROR;
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::ReportError(ErrorStruct*ps, bool bMsgBox)
{
	ASSERT(ps != NULL);
	if (FAILED(ps->m_hr))
	{
		CString str, sError, sDescription;
		GetErrorStrings(ps->m_hr, sError, sDescription);
		str.Format(_T("Error initialising Filters\n%s(%d) :\nError: %s (%x):\n %s\n"), ps->m_sFile, ps->m_nLine, sError, ps->m_hr, sDescription);
#ifdef _DEBUG
		TRACE(str);
#else
		WK_TRACE(str);
#endif		
		if (m_bStandalone && bMsgBox)
		{
			int nResult = AfxMessageBox(str, MB_ICONERROR|MB_OKCANCEL);
			ASSERT(nResult == IDOK);
		}
	}
	delete ps;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CUSBCameraUnitDlg::AddToRot(IUnknown *pUnkGraph) 
{
	IMoniker * pMoniker;
	IRunningObjectTable *pROT;
	if (FAILED(GetRunningObjectTable(0, &pROT))) 
	{
		return E_FAIL;
	}
	WCHAR wsz[256];
	wsprintfW(wsz, L"FilterGraph %08p pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
	HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if (SUCCEEDED(hr))
	{
	  hr = pROT->Register(0, pUnkGraph, pMoniker, &m_dwRegister);
	  pMoniker->Release();
	}
	pROT->Release();
	return hr;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CUSBCameraUnitDlg::FindMatchingFilter(DWORD dwMerit, IPin *pPin, 
														 BOOL bInputNeeded, GUID majortypeIn, GUID subtypeIn, 
														 BOOL bOutputNeeded, GUID majortypeOut, GUID* subtypeOut, int nSubTypes, 
														 BOOL bRender,
														 IBaseFilter **ppFilter, AM_MEDIA_TYPE **ppMT, LPCWSTR pszFilterName)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	HRESULT          hr = NULL;
	IFilterMapper   *pMapper = NULL;
	IEnumRegFilters *pEnumRegFilters = NULL;
	IPin            *pPinIn = NULL, *pPinOut = NULL;

	if (ppFilter == NULL) return E_POINTER;
	if (ppMT     == NULL) return E_POINTER;
	
	hr = CoCreateInstance(CLSID_FilterMapper, NULL, CLSCTX_INPROC, IID_IFilterMapper, (void **) &pMapper);
	if (FAILED(hr)) return hr;

	hr = pMapper->EnumMatchingFilters(&pEnumRegFilters, dwMerit, bInputNeeded, majortypeIn, subtypeIn, bRender, bOutputNeeded, majortypeOut, subtypeOut[0]);

	AM_MEDIA_TYPE *pmt = *ppMT;
	*ppMT = NULL;
	if (SUCCEEDED(hr))
	{
		REGFILTER *prf = NULL;
		pEnumRegFilters->Reset();
		ULONG ulCatched;
		while (SUCCEEDED(pEnumRegFilters->Next(2, &prf, &ulCatched)) && prf)
		{
			CString	str(prf->Name);
			TRACE(_T("%s\n"), str);

			hr = AddFilterToGraph(m_pGraph, ppFilter, GetString(prf->Clsid), pszFilterName);
			if (SUCCEEDED(hr))
			{
				hr = GetPin(*ppFilter, PINDIR_INPUT, 0, &pPinIn);
				if (SUCCEEDED(hr))
				{
//					hr = pPin->Connect(pPinIn, pmt);
//					if (FAILED(hr))
					{
						hr = m_pGraph->Connect(pPin, pPinIn);
					}
					if (hr == S_OK)
					{
						IEnumMediaTypes *pEMT = NULL;
						RELEASE_OBJECT(pPinOut);
						HR_EXCEPTION(GetPin(*ppFilter, PINDIR_OUTPUT, 0, &pPinOut));
						hr = pPinOut->EnumMediaTypes(&pEMT);
						if (SUCCEEDED(hr))
						{
							hr = E_FAIL;
							pEMT->Reset();
							while ((pEMT->Next(1, ppMT, NULL) == S_OK) && ((*ppMT) != NULL))
							{
								for (int i=0; i<nSubTypes; i++)
								{
									if ((*ppMT)->subtype == subtypeOut[i])
									{
										hr = S_OK;
										break;
									}
								}
								if (i<nSubTypes) break;
								DeleteMediaType(*ppMT);
								*ppMT = NULL;
							}
						}
						RELEASE_OBJECT(pEMT);
						break;
					}
					else
					{
						ReportError(HRESULT_ERROR_AT_POS(hr), false);
					}
					RELEASE_OBJECT(pPinIn);
				}
				m_pGraph->RemoveFilter(*ppFilter);
				RELEASE_OBJECT((*ppFilter));
			}
			CoTaskMemFree((void*)prf);
			prf = NULL;
		}
		RELEASE_OBJECT(pEnumRegFilters);
	}
	RELEASE_OBJECT(pMapper);
	RELEASE_OBJECT(pPinIn);
	RELEASE_OBJECT(pPinOut);
	return hr;
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::SetControlStates(CameraStates& sState)
{
	BOOL bNotInitialized = TRUE;
	BOOL bCanInitialize  = FALSE;
	if (m_nSelectedCam == CB_ERR)
	{
		bNotInitialized = FALSE;
	}
	else
	{
		bCanInitialize = TRUE;
	}

	if (sState.bInitialized || sState.bVideoPlay)
	{
		bNotInitialized = FALSE;
		m_btnStart.EnableWindow(!m_bStarted);
		m_btnStop.EnableWindow(  m_bStarted);
		m_btnRelease.EnableWindow(TRUE);
		m_btnInitialize.EnableWindow(FALSE);
	}
	else
	{
		m_btnStart.EnableWindow(FALSE);
		m_btnStop.EnableWindow(m_bStarted);
		m_btnRelease.EnableWindow(FALSE);
		m_btnInitialize.EnableWindow(!m_bStarted && bCanInitialize);
	}


//	m_cCameras.EnableWindow(bNotInitialized);
	m_cOutputFormats.EnableWindow(bNotInitialized);
	m_cOutputFormats2.EnableWindow(bNotInitialized);
	m_cEncoders.EnableWindow(bNotInitialized);
	m_cFrameRates.EnableWindow(bNotInitialized);
	GetDlgItem(IDC_CK_SHOW_PICTUR)->EnableWindow(bNotInitialized);
}
//////////////////////////////////////////////////////////////////////////
LRESULT CUSBCameraUnitDlg::OnRequestCameraControl(WPARAM wParam, LPARAM lParam)
{
	CameraControlStruct*pCCS    = (CameraControlStruct*)wParam;
	CIPC*				pCIPC   = (CIPC*) lParam;
	DWORD				dwValue = _ttoi(pCCS->m_sValue);
	DWORD dwReturn, *pdwReturn = NULL;
	HRESULT hr;

	switch (pCCS->m_Type)
	{
		case eCCSPCameraControlCmd:
		{
			if (pCCS->m_bSet)
			{
				dwReturn = pCCS->m_sValue.Find(_T(':'));
				if (dwReturn != -1)
				{
					dwReturn = _ttoi(pCCS->m_sValue.Mid(dwReturn+1));
					pdwReturn = &dwReturn;
				}
			}
			else
			{
				pdwReturn = &dwValue;
			}
			hr = CameraControlPTZ(pCCS->m_ID.GetSubID(), pCCS->m_CamCtrl.CamCtrlCmd, pdwReturn);
		}break;
		case eCCSPCameraProperty:
			hr = CameraControl(pCCS->m_ID.GetSubID(), pCCS->m_CamCtrl.CamCtrlProp, pCCS->m_bSet, pCCS->m_bAuto, dwValue);
			break;
		case eCCSPVideoProcAmpProperty:
			hr = CameraProperties(pCCS->m_ID.GetSubID(), pCCS->m_CamCtrl.VideoProcAmpProp, pCCS->m_bSet, pCCS->m_bAuto, dwValue);
			break;
		default:
			hr = E_UNEXPECTED;
			break;
	}

	if (hr == E_POINTER)
	{
		pCIPC->DoIndicateError(0, pCCS->m_ID, CIPC_ERROR_CAMERA_NOT_PRESENT, 0);	
	}
	else if (FAILED(hr))
	{
		if (pCCS->m_bSet)
		{
			pCIPC->DoConfirmSetValue(pCCS->m_ID, pCCS->m_sKey, CSD_ERROR, pCCS->m_dwUserData);
		}
		else
		{
			pCIPC->DoConfirmGetValue(pCCS->m_ID, pCCS->m_sKey, pCCS->m_sValue, pCCS->m_dwUserData);
		}
	}
	else
	{
		if (pCCS->m_bSet)
		{
			pCIPC->DoConfirmSetValue(pCCS->m_ID, pCCS->m_sKey, pCCS->m_sValue, pCCS->m_dwUserData);
		}
		else
		{
			pCCS->m_sValue.Format(_T("%d"), dwValue);
			pCIPC->DoConfirmGetValue(pCCS->m_ID, pCCS->m_sKey, pCCS->m_sValue, pCCS->m_dwUserData);
		}
	}
	delete pCCS;
	return 0;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CUSBCameraUnitDlg::CameraProperties(WORD wSource, VideoProcAmpProperty CameraProperty, bool bSet, bool &bAuto, DWORD &dwValue)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (!IsBetween(wSource, 0, MAX_CAMERAS-1)) return E_INVALIDARG;
	if (m_pCapture[wSource] == NULL)           return E_POINTER;

	IAMVideoProcAmp *pVPA = NULL;
	HRESULT hr = m_pCapture[wSource]->QueryInterface(IID_IAMVideoProcAmp, (void**)&pVPA);
	if (SUCCEEDED(hr))
	{
		long lMin, lMax, lDelta, lDefault, lFlags = 0,
			  lMinA, lMaxA;
		double y, m;
		hr = pVPA->GetRange(CameraProperty, &lMin, &lMax, &lDelta, &lDefault, &lFlags);
		switch (CameraProperty)
		{
			case VideoProcAmp_Brightness:
				lMinA = MIN_BRIGHTNESS;
				lMaxA = MAX_BRIGHTNESS;
				break;
			case VideoProcAmp_Contrast:
				lMinA = MIN_CONTRAST;
				lMaxA = MAX_CONTRAST;
				break;
			case VideoProcAmp_Saturation:
				lMinA = MIN_SATURATION;
				lMaxA = MAX_SATURATION;
				break;
			default:
				lMinA = 0;
				lMaxA = 1000;
				break;
		}

		if (SUCCEEDED(hr))
		{
			if (bSet)
			{
				lFlags = (bAuto) ? VideoProcAmp_Flags_Auto : VideoProcAmp_Flags_Manual;
				m = (lMax-lMin) / (double)(lMaxA-lMinA);
				y = m * dwValue + lMin;
				dwValue = (DWORD)y;
				hr = pVPA->Set(CameraProperty, dwValue, lFlags);
			}
			else
			{
				hr = pVPA->Get(CameraProperty, (long*)&dwValue, &lFlags);
				m = (lMaxA-lMinA) / (double)(lMax-lMin);
				y = m * dwValue + lMinA;
				dwValue = (DWORD)y;
				if (lFlags & VideoProcAmp_Flags_Auto)   bAuto = true;
				if (lFlags & VideoProcAmp_Flags_Manual) bAuto = false;
			}
		}
		pVPA->Release();
	}
	return hr;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CUSBCameraUnitDlg::CameraControl(WORD wSource, CameraControlProperty CameraProperty, bool bSet, bool &bAuto, DWORD &dwValue)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (!IsBetween(wSource, 0, MAX_CAMERAS-1)) return E_INVALIDARG;
	if (m_pCapture[wSource] == NULL)           return E_POINTER;

	IAMCameraControl *pVPA = NULL;
	HRESULT hr = m_pCapture[wSource]->QueryInterface(IID_IAMCameraControl, (void**)&pVPA);
	if (SUCCEEDED(hr))
	{
		long lMin, lMax, lDelta, lDefault, lFlags = 0,
			  lMinA = 0, lMaxA = 1000;
		double y, m;

		hr = pVPA->GetRange(CameraProperty, &lMin, &lMax, &lDelta, &lDefault, &lFlags);
		if (SUCCEEDED(hr))
		{
			if (bSet)
			{
				lFlags = (bAuto) ? CameraControl_Flags_Auto : CameraControl_Flags_Manual;
				m = (lMax-lMin) / (double)(lMaxA-lMinA);
				y = m * dwValue + lMin;
				dwValue = (DWORD)y;
				hr = pVPA->Set(CameraProperty, dwValue, lFlags);
			}
			else
			{
				hr = pVPA->Get(CameraProperty, (long*)&dwValue, &lFlags);
				m = (lMaxA-lMinA) / (double)(lMax-lMin);
				y = m * dwValue + lMinA;
				dwValue = (DWORD)y;
				if (lFlags & VideoProcAmp_Flags_Auto)   bAuto = true;
				if (lFlags & VideoProcAmp_Flags_Manual) bAuto = false;
			}
		}
		pVPA->Release();
	}
	return hr;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CUSBCameraUnitDlg::CameraControlPTZ(WORD wSource, CameraControlCmd Cmd, DWORD *pdwReturn)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	if (!IsBetween(wSource, 0, MAX_CAMERAS-1)) return E_INVALIDARG;
	if (m_pCapture[wSource] == NULL)           return E_POINTER;
	IAMCameraControl *pVPA = NULL;
	HRESULT hr = m_pCapture[wSource]->QueryInterface(IID_IAMCameraControl, (void**)&pVPA);
	if (SUCCEEDED(hr))
	{
		bool bTimer = false;
		CString sPosition;
		UINT nTimerEvent = GetTimerByID(TIMER_PTZ_CAM_MIN+wSource);
		switch (Cmd)
		{
			case CCC_INVALID:
			if (pdwReturn)
			{
				long lValue, lFlags;
				if (SUCCEEDED(pVPA->Get(CameraControl_Pan     , &lValue, &lFlags))) *pdwReturn |= PTZF_PAN|PTZF_TURN_180;
				if (SUCCEEDED(pVPA->Get(CameraControl_Tilt    , &lValue, &lFlags))) *pdwReturn |= PTZF_TILT;
				if (SUCCEEDED(pVPA->Get(CameraControl_Zoom    , &lValue, &lFlags))) *pdwReturn |= PTZF_ZOOM;
				if (SUCCEEDED(pVPA->Get(CameraControl_Iris    , &lValue, &lFlags))) *pdwReturn |= PTZF_IRIS;
				if (SUCCEEDED(pVPA->Get(CameraControl_Focus   , &lValue, &lFlags))) *pdwReturn |= PTZF_FOCUS;
				if (SUCCEEDED(pVPA->Get(CameraControl_Roll    , &lValue, &lFlags))) *pdwReturn |= PTZF_ROLL;
				if (SUCCEEDED(pVPA->Get(CameraControl_Exposure, &lValue, &lFlags))) *pdwReturn |= PTZF_EXPOSURE;
				*pdwReturn |= PTZF_SPEED;
			}break;
			case CCC_PAN_STOP:
			case CCC_TILT_STOP:
			case CCC_FOCUS_STOP:
			case CCC_IRIS_STOP:
			case CCC_ZOOM_STOP:
				m_sCameraState[wSource].nPTZCmd = 0;
				if (nTimerEvent)
				{
					KillTimer(nTimerEvent);
					m_TimerEvents.RemoveKey(nTimerEvent);
				}
				break;
			case CCC_PAN_LEFT:   case CCC_PAN_RIGHT: 
			case CCC_TILT_UP:    case CCC_TILT_DOWN:
			case CCC_FOCUS_NEAR: case CCC_FOCUS_FAR:
			case CCC_IRIS_OPEN:  case CCC_IRIS_CLOSE:
			case CCC_ZOOM_IN:	 case CCC_ZOOM_OUT:
			if (nTimerEvent == 0)
			{
				int nTime = 500; // ms
				m_sCameraState[wSource].nPTZCmd = Cmd;
				if (pdwReturn)
				{
					m_sCameraState[wSource].nPTZSpeed = *pdwReturn;
					if (*pdwReturn < 6)
					{
						*pdwReturn = 6 - *pdwReturn;
						nTime *= *pdwReturn;
					}
				}
				bTimer = TRUE;
				nTimerEvent = SetTimer(TIMER_PTZ_CAM_MIN+wSource, nTime);
			}	break;
			case CCC_FOCUS_AUTO:
			case CCC_IRIS_AUTO:
			case CCC_TURN_180:
				bTimer = TRUE;
				break;
			case CCC_POS_HOME: sPosition = _T("Home"); break;
			case CCC_POS_1:    sPosition = _T("Pos1"); break;
			case CCC_POS_2:    sPosition = _T("Pos2"); break;
			case CCC_POS_3:    sPosition = _T("Pos3"); break;
			case CCC_POS_4:    sPosition = _T("Pos4"); break;
			case CCC_POS_5:    sPosition = _T("Pos5"); break;
			case CCC_POS_6:    sPosition = _T("Pos6"); break;
			case CCC_POS_7:    sPosition = _T("Pos7"); break;
			case CCC_POS_8:    sPosition = _T("Pos8"); break;
			case CCC_POS_9:    sPosition = _T("Pos9"); break;
			case CCC_CONFIG:	break;
			default:
				bTimer = TRUE;
				break;
		}
		if (bTimer)
		{
			CameraControlProperty ccp = (CameraControlProperty)-1;
			long lMin, lMax, lDelta, lDefault, lValue,
				  lFlags = CameraControl_Flags_Manual, 
				  lSteps = m_sCameraState[wSource].nPTZSpeed;
			if (lSteps < 6) lSteps  = 1; // Timer langsamer
			else            lSteps -= 4; // Schrittweite hoeher
			bool bUp   = false,
				  bSet  = true,
			     bAuto = false;

			switch (m_sCameraState[wSource].nPTZCmd)
			{
				case CCC_PAN_RIGHT: bUp = true;
				case CCC_PAN_LEFT:  ccp = CameraControl_Pan;
					break;
				case CCC_TILT_UP:   bUp = true;
				case CCC_TILT_DOWN: ccp = CameraControl_Tilt;
					break;
				case CCC_FOCUS_FAR: bUp = true;
				case CCC_FOCUS_NEAR:ccp = CameraControl_Focus;
					break;
				case CCC_IRIS_OPEN: bUp = true;
				case CCC_IRIS_CLOSE:ccp = CameraControl_Iris;
					break;
				case CCC_ZOOM_IN:   bUp = true;
				case CCC_ZOOM_OUT:  ccp = CameraControl_Zoom;
					break;
			}

			if (ccp == -1)
			{
				switch (Cmd)
				{
					case CCC_FOCUS_AUTO:
						bAuto = true;
						ccp   = CameraControl_Focus;
						break;
					case CCC_IRIS_AUTO:
						bAuto = true;
						ccp   = CameraControl_Iris;
						break;
					case CCC_TURN_180:
						ccp = CameraControl_Pan;
						lSteps = -1;
						break;
				}
			}

			hr = pVPA->GetRange(ccp, &lMin, &lMax, &lDelta, &lDefault, &lFlags);
			if (SUCCEEDED(hr))
			{
				hr = pVPA->Get(ccp, (long*)&lValue, &lFlags);
				if (bAuto)					// Set Auto
				{
					lFlags = CameraControl_Flags_Auto;
				}
				else if (lSteps == -1)	// Turn 180
				{
					if (lValue < 0) lValue += 180;
					else            lValue -= 180;
				}
				else if (bUp)				// Manual control
				{
					lValue += lDelta * lSteps;
					if (lValue >= lMax) bSet = false;
				}
				else
				{
					lValue -= lDelta * lSteps;
					if (lValue <= lMin) bSet = false;
				}
				if (bSet)
				{
					hr = pVPA->Set(ccp, lValue, lFlags);
				}
				else if (nTimerEvent)
				{
					KillTimer(nTimerEvent);
					m_TimerEvents.RemoveKey(nTimerEvent);
				}
			}
		}
		else if (!sPosition.IsEmpty())
		{
			CString strSection;
			CWK_Profile wkpDev(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sDeviceKey, _T(""));
			strSection.Format(CAMERA_N, wSource+1);
			strSection += _T("\\") + sPosition;
			long lValue,
				  lFlags = CameraControl_Flags_Manual;
			if (pdwReturn) // Preset
			{
				hr = pVPA->Get(CameraControl_Pan, (long*)&lValue, &lFlags);
				if (SUCCEEDED(hr))
				{
					wkpDev.WriteInt(strSection, CAM_CTRL_PAN, lValue);
				}
				hr = pVPA->Get(CameraControl_Tilt, (long*)&lValue, &lFlags);
				if (SUCCEEDED(hr))
				{
					wkpDev.WriteInt(strSection, CAM_CTRL_TILT, lValue);
				}
				hr = pVPA->Get(CameraControl_Zoom, (long*)&lValue, &lFlags);
				if (SUCCEEDED(hr))
				{
					wkpDev.WriteInt(strSection, CAM_CTRL_ZOOM, lValue);
				}
				hr = pVPA->Get(CameraControl_Focus, (long*)&lValue, &lFlags);
				if (SUCCEEDED(hr))
				{
					wkpDev.WriteInt(strSection, CAM_CTRL_FOCUS, lValue);
				}
				hr = pVPA->Get(CameraControl_Roll, (long*)&lValue, &lFlags);
				if (SUCCEEDED(hr))
				{
					wkpDev.WriteInt(strSection, CAM_CTRL_ROLL, lValue);
				}
			}
			else // Set
			{
				lValue = wkpDev.GetInt(strSection, CAM_CTRL_PAN, 360);
				if (lValue != 360)
				{
					hr = pVPA->Set(CameraControl_Pan, (long)lValue, lFlags);
				}
				lValue = wkpDev.GetInt(strSection, CAM_CTRL_TILT, 360);
				if (lValue != 360)
				{
					hr = pVPA->Set(CameraControl_Tilt, (long)lValue, lFlags);
				}
				lValue = wkpDev.GetInt(strSection, CAM_CTRL_ZOOM, 0);
				if (lValue != 0)
				{
					hr = pVPA->Set(CameraControl_Zoom, (long)lValue, lFlags);
				}
				lValue = wkpDev.GetInt(strSection, CAM_CTRL_FOCUS, -1);
				if (lValue != -1)
				{
					hr = pVPA->Set(CameraControl_Focus, (long)lValue, lFlags);
				}
				lValue = wkpDev.GetInt(strSection, CAM_CTRL_ROLL, 360);
				if (lValue != 360)
				{
					hr = pVPA->Set(CameraControl_Roll, (long)lValue, lFlags);
				}
			}
		}

		pVPA->Release();
	}
	return hr;
}
//////////////////////////////////////////////////////////////////////////
UINT CUSBCameraUnitDlg::SetTimer(UINT nEvent, UINT nMilliSeconds)
{
	UINT nTimerEvent = CWnd::SetTimer(nEvent, nMilliSeconds, NULL);
	if (nTimerEvent)
	{
		WORD wMilliSeconds = nMilliSeconds < 65536 ? (WORD)nMilliSeconds : 1;
		m_TimerEvents.SetAt(nTimerEvent, MAKELONG(nEvent, wMilliSeconds));
	}
	return nTimerEvent;
}
//////////////////////////////////////////////////////////////////////////
UINT CUSBCameraUnitDlg::GetTimerByID(UINT nID)
{
	POSITION pos = m_TimerEvents.GetStartPosition();
	UINT nKey;
	DWORD dwValue;
	while (pos)
	{
		m_TimerEvents.GetNextAssoc(pos, nKey, dwValue);
		if (nID == LOWORD(dwValue))
		{
			return nKey;
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::SetMotionDetection(WORD wSource, bool bOn)
{
	if (IsBetween(wSource, 0, MAX_CAMERAS-1))
	{
		m_sCameraState[wSource].bMotionDetection = bOn;
	}
}
//////////////////////////////////////////////////////////////////////////
DWORD CUSBCameraUnitDlg::GetActiveCameraMask()
{
	DWORD dwCameras = 0, dwMask = 1;
	int i;
	for (i=0; i<MAX_CAMERAS; i++, dwMask<<=1)
	{
		if (m_cCameraNo.GetItemData(i) != CB_ERR)
		{
			dwCameras |= dwMask;
		}
	}
	return dwCameras;
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::SetButtonIcon(int nBtnID, int nIconID)
{
   SendDlgItemMessage(nBtnID, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(nIconID), IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
	GetDlgItem(nBtnID)->UpdateWindow();
}
//////////////////////////////////////////////////////////////////////////
DWORD CUSBCameraUnitDlg::ScanForCameras(DWORD dwRequestedCams)
{
	DWORD dwCameras = 0, dwMask = 1;
	int i;
	for (i=0; i<MAX_CAMERAS; i++, dwMask<<=1)
	{
		if (   (dwRequestedCams & dwMask) 
			 && (m_cCameraNo.GetItemData(i) != CB_ERR))
		{
			dwCameras |= dwMask;
		}
	}
	return dwCameras;
}
//////////////////////////////////////////////////////////////////////////
BOOL CUSBCameraUnitDlg::IsCameraInitialised(WORD nCam)
{
	if (IsBetween(nCam, 0, MAX_CAMERAS-1))
	{
		return m_sCameraState[nCam].bInitialized;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
BOOL CUSBCameraUnitDlg::IsCameraPresent(WORD nCam)
{
	if (IsBetween(nCam, 0, MAX_CAMERAS-1))
	{
		return (m_cCameraNo.GetItemData(nCam) != CB_ERR) ? TRUE : FALSE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::SetCameraActive(WORD nCam, bool bActive)
{
	if (IsBetween(nCam, 0, MAX_CAMERAS-1))
	{
		m_sCameraState[nCam].bActive = bActive;
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CUSBCameraUnitDlg::IsCameraActive(WORD nCam)
{
	if (IsBetween(nCam, 0, MAX_CAMERAS-1))
	{
		return m_sCameraState[nCam].bActive;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::SetVUIcon(WORD wColor, BOOL bType)
{
	static WORD wCurColor = ICON_NOCOLOR;
	static BOOL bCurType	 = FALSE;

	if ((wColor != wCurColor) || (bType != bCurType))
	{
		wCurColor = wColor;
	
		if (m_bStandalone)
		{
			if ((wColor >= ICON_MAIN) && (wColor <= ICON_RECORDING))
			{
				::PostMessage(m_hWnd, WM_SETICON,	bType, (LPARAM)m_hIcon[wColor-1]);
			}
		}
		else
		{
			// Icon ins Systemtray
			NOTIFYICONDATA tnd;

			CString sTip = COemGuiApi::GetApplicationName(theApp.GetApplicationId());

			tnd.cbSize = sizeof(NOTIFYICONDATA);
			tnd.hWnd	  = m_hWnd;
			tnd.uID    = 1;

			tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
			tnd.uCallbackMessage = WM_TRAYCLICKED;
			tnd.hIcon		= m_hIcon[0];

			lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);

			if ((wColor >= ICON_MAIN) && (wColor <= ICON_RECORDING))
			{
				tnd.hIcon = m_hIcon[wColor-1];
			}
			Shell_NotifyIcon(NIM_MODIFY, &tnd);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::InitVideoDisplay()
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	IVideoWindow    *pVW       = NULL;
	HRESULT hr = m_pGraph->QueryInterface(IID_IVideoWindow, (void**)&pVW);
	if (SUCCEEDED(hr) && (pVW != NULL))
	{
//		ASSERT(m_ppVSccOutputFormats != NULL);
//		ASSERT(m_cOutputFormats.GetCurSel() != CB_ERR);
		CRect    rcVW;
		CString  str, sText;
		int      nWidth, nHeight, nD;
		VIDEO_STREAM_CONFIG_CAPS *pVSCC = NULL;

		if (m_ppVSccOutputFormats && m_cOutputFormats.GetCurSel() != CB_ERR)
		{
			VIDEO_STREAM_CONFIG_CAPS *pVSCC = m_ppVSccOutputFormats[m_cOutputFormats.GetCurSel()];
			m_cCameras.GetLBText(m_nSelectedCam, str);
			nD = FindGCD(pVSCC->InputSize.cy, pVSCC->InputSize.cx);
			nWidth  = pVSCC->InputSize.cx / nD;
			nHeight = pVSCC->InputSize.cy / nD;
			sText.Format(_T("%s (%d:%d)"), str, nWidth, nHeight);
			GetDlgItem(IDC_GRP_CAM_PICTURE)->SetWindowText(sText);
		}

		m_cFrame.GetClientRect(&rcVW);
		rcVW.left   =  0;
		rcVW.top    =  0;
		nWidth      = rcVW.Width();
		if (pVSCC)
		{
			nHeight     = MulDiv(nWidth, pVSCC->InputSize.cy, pVSCC->InputSize.cx);
			rcVW.bottom = rcVW.top + nHeight;
		}
		else
		{
			nHeight = rcVW.Height();
		}

		pVW->put_Owner((OAHWND)m_cFrame.m_hWnd);
		pVW->put_MessageDrain((OAHWND)m_hWnd);
		pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
		pVW->put_AutoShow(OATRUE);
		pVW->SetWindowPosition(rcVW.left, rcVW.top, nWidth, nHeight);
		pVW->put_AutoShow(TRUE);

		m_cFrame.GetWindowRect(&rcVW);
		ScreenToClient(&rcVW);
		rcVW.bottom = rcVW.top + nHeight;
		m_cFrame.MoveWindow(&rcVW);
		m_cFrame.InvalidateRect(NULL);
		m_pVideoWindow = pVW;
		m_pVideoWindow->AddRef();
	}
	RELEASE_OBJECT(pVW);
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::InitMotionDetection()
{
	if (m_bMotionDetection)
	{
		if (m_pMD == NULL)
		{
			CString sSMName, sDirectory;
			int n1, n2;
			n1 = m_sDeviceKey.Find('\\')+1;
			n2 = m_sDeviceKey.ReverseFind('\\');
			CSize szCapture;
			szCapture.cx = m_szResolution.cx / 2;
			szCapture.cy = m_szResolution.cy / 2;
			GetWindowText(sDirectory);
			m_pMD = new CMotionDetection(m_sDeviceKey.Mid(n1, n2-n1), szCapture, sDirectory);
			m_pMD->Init();

			sSMName = SM_MD_INPUT;
			if (theApp.GetInstanceCount() != 1)
				sSMName += (_TCHAR)(_T('0') + theApp.GetInstanceCount());
			m_pInputMDAlarm = new CIPCInputUSBcamUnitMDAlarm(this, sSMName);
			if (!m_pInputMDAlarm)
				WK_TRACE_ERROR(_T("CUSBcamUnitDlg::Create\tCIPCInputUSBcamUnitMDAlarm-Objekt konnte nicht angelegt werden.\n"));
		}
	}
	else
	{
		WK_DELETE(m_pMD);
		WK_DELETE(m_pInputMDAlarm);
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::InitFunctions()
{
	InitMotionDetection();

	CString sSMName;					// Kamerabilder, Bewegungserkennung
	sSMName = SM_OUTPUT_CAMERA;
	if (theApp.GetInstanceCount() != 1)
		sSMName += (_TCHAR)(_T('0') + theApp.GetInstanceCount());

	m_pOutput = new CIPCOutputUSBcamUnit(this, sSMName);//, m_sDeviceKey);

	if (m_bCameraDetectors)
	{
		sSMName = SM_INPUT;				// Standbildbutton
		if (theApp.GetInstanceCount() != 1)
			sSMName += (_TCHAR)(_T('0') + theApp.GetInstanceCount());
		m_pInput  = new CIPCInputUSBcamUnit(this, sSMName);
	}
	
#ifdef TEST_WITH_SAVIC
	// SyncEvent wird nur für die Savic benötigt
	CString sEventName = SYNC_EVENT;
	if (theApp.GetInstanceCount() != 1)
		sEventName += (_TCHAR)('0'+ theApp.GetApplicationId());
	WK_TRACE(_T("sync semaphore %s\n"),sEventName);
	m_hSyncSemaphore = CreateSemaphore(NULL, 0, 10, sEventName);
	if (m_hSyncSemaphore==NULL) 
	{
		WK_TRACE_ERROR(_T("Failed to create sync semaphore\n"));
	}
#endif		
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent)
	{
		//////////////////////////////////////////////////////////////////////////
		case EVENT_UPDATE_FRAME:
			m_cFrame.InvalidateRect(NULL);
			break;
		//////////////////////////////////////////////////////////////////////////
		default:
		{
			DWORD dwValue;
			if (m_TimerEvents.Lookup(nIDEvent, dwValue))
			{
				switch (LOWORD(dwValue))
				{
					//////////////////////////////////////////////////////////////////////////
					case ICON_RUNNING:
					{
						SetVUIcon(ICON_RUNNING, FALSE);
						SetButtonIcon(IDC_RECORDING, IDC_STOP);
						KillTimer(nIDEvent);
						m_TimerEvents.RemoveKey(nIDEvent);
					}break;
					//////////////////////////////////////////////////////////////////////////
					case TIMER_RESET:
					{
						if (m_pOutput && m_pOutput->DoReset())
						{
							if (m_bCameraDetectors && m_bMotionDetection)
							{
								if (   m_pInput        && m_pInput->DoReset() 
									&& m_pInputMDAlarm && m_pInputMDAlarm->DoReset())
								{
									OnRequestReset();
								}
							}
							else if (m_bCameraDetectors)
							{
								if (m_pInput && m_pInput->DoReset())
								{
									OnRequestReset();
								}
							}
							else if (m_bMotionDetection)
							{
								if (m_pInputMDAlarm && m_pInputMDAlarm->DoReset())
								{
									OnRequestReset();
								}
							}
							else
							{
								OnRequestReset();
							}
						}
						KillTimer(nIDEvent);
						m_TimerEvents.RemoveKey(nIDEvent);
					}
					//////////////////////////////////////////////////////////////////////////
					case TIMER_DEV_UPDATE:
					case TIMER_DEV_CHANGED:
					{
						int	 i, // Index für die Kameras
							 j, // Index für die Kameranummer
							 nFind;
						BOOL bStarted = m_bStarted;
						CString sCamera;
						if (HIWORD(dwValue))
						{
							KillTimer(nIDEvent);
						}
						m_TimerEvents.RemoveKey(nIDEvent);

						if (LOWORD(dwValue) == TIMER_DEV_CHANGED)
						{
							m_cCameras.ResetContent();
							EnumFiltersWithMonikerToList(NULL, &CLSID_VideoInputDeviceCategory, NULL, &m_cCameras);
							SaveCamerasToRegistry();
						}
						for (j=0; j<MAX_CAMERAS; j++)				// wurde eine Kamera entfernt ?
						{
							if (!m_sVideoInput[j].IsEmpty())
							{										// Kamera suchen
								nFind = m_cCameras.FindStringExact(-1, m_sVideoInput[j]);
								if (nFind == CB_ERR && m_pCapture[j])// nicht mehr da ?
								{
									ReleaseFilters(j);				// entfernen aus dem Graphen
									m_cCameraNo.SetItemData(j, (DWORD)CB_ERR);// auch aus der Combobox
									if (m_pOutput)
									{
										m_pOutput->IndicationVideoState((WORD)j, FALSE);
									}
									break;
								}
							}
						}
						if (j == m_nCamIndex) 						// es wurde die selektierte entfernt
						{
							for (j=0; j<MAX_CAMERAS; j++)			// neu selektieren
							{
								if (m_cCameraNo.GetItemData(j) != CB_ERR)
								{
									m_cCameraNo.SetCurSel(j);
									OnSelchangeComboCameraNo();
									break;
								}
							}
						}

						for (i=0; i<MAX_CAMERAS; i++)				// wurde eine Kamera hinzugefügt ?
						{
							if (m_cCameras.GetLBTextLen(i) != CB_ERR)
							{
								for (j=0; j<MAX_CAMERAS; j++)		// suchen in der CombBox
								{
									if (m_cCameraNo.GetItemData(j) == (DWORD)i)
									{
										break;							// gefunden
									}
								}
								if (j == MAX_CAMERAS)				// nicht gefunden
								{
									m_cCameras.GetLBText(i, sCamera);
									for (j=0; j<MAX_CAMERAS; j++) // suchen in dem StringArray
									{
										if (!m_sVideoInput[j].IsEmpty() &&
											sCamera == m_sVideoInput[j])
										{									// gefunden: an dieser Stelle einfügen
											m_cCameraNo.SetItemData(j, i);
											m_cCameraNo.SetCurSel(j);
											m_nCamIndex = -1;
											OnSelchangeComboCameraNo();
											if (!m_bStandalone)		// alte Kamera starten
											{
												InitCameraFilters(i);
												if (m_pOutput)
												{
													m_pOutput->IndicationVideoState((WORD)j, TRUE);
												}
											}
											break;
										}
									}
									if (j == MAX_CAMERAS)			// nicht gefunden
									{
										for (j=0; j<MAX_CAMERAS; j++)// suchen nach einer freien Stelle in der CombBox
										{
											if (m_cCameraNo.GetItemData(j) == CB_ERR)
											{
												m_cCameraNo.SetItemData(j, i);
												m_cCameraNo.SetCurSel(j);
												m_nCamIndex = -1;
												OnSelchangeComboCameraNo();
												m_sVideoInput[j] = sCamera;
												bStarted = FALSE;	// neue Kamera nicht starten
												break;				// gefunden
											}
										}
									}
								}
							}
						}

						if (bStarted && !m_bStandalone)
						{
							OnBtnStart();
						}
					}break;
					//////////////////////////////////////////////////////////////////////////
					default:
					//////////////////////////////////////////////////////////////////////////
					if (IsBetween(LOWORD(dwValue), TIMER_PTZ_CAM_MIN, TIMER_PTZ_CAM_MAX))
					{
						CameraControlPTZ((WORD)(LOWORD(dwValue)-TIMER_PTZ_CAM_MIN), (CameraControlCmd)-1, NULL);
					}
					else if (IsBetween(LOWORD(dwValue), TIMER_ALARM_MIN, TIMER_ALARM_MAX))
					{
						if (m_pInput)
						{
							m_pInput->AlarmStateChanged((WORD)(LOWORD(dwValue)-TIMER_ALARM_MIN), false);
						}
						SetButtonIcon(IDC_STILL, IDC_STOP);
						KillTimer(nIDEvent);
						m_TimerEvents.RemoveKey(nIDEvent);
					}
					else if (IsBetween(LOWORD(dwValue), EVENT_VIDEO_JOB_MIN, EVENT_VIDEO_JOB_MAX))
					{
						int nCam = LOWORD(dwValue) - EVENT_VIDEO_JOB_MIN;
						m_csVideoJobs.Lock();
						if (m_VideoJobList[nCam].GetCount())
						{
							CVideoJob *pJob = m_VideoJobList[nCam].GetTail();
							switch (pJob->m_action)
							{
								case  VJA_ENCODE:
								if (!m_bStarted && !m_bIsShuttingDown)
								{
									InitCameras();
									OnBtnStart();
								}break;
								case VJA_STOP_ENCODE:
								if (m_bStarted)
								{
									OnBtnStop();
								}break;
							}
						}
						m_csVideoJobs.Unlock();
					}

				}
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::RemovePendigJobs()
{
#ifdef TEST_WITH_SAVIC 
	while (m_SyncJob.GetCount())
	{
		CVideoJob *pJob = m_SyncJob.GetTail();
		WK_DELETE(pJob);
		m_SyncJob.RemoveTail();
	}
#endif
	for (int nCam=0; nCam<MAX_CAMERAS; nCam++)
	{
		while (m_VideoJobList[nCam].GetCount())
		{
			CVideoJob *pJob = m_VideoJobList[nCam].GetTail();
			WK_DELETE(pJob);
			m_VideoJobList[nCam].RemoveTail();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::SaveCamerasToRegistry()
{
	CWK_Profile wkp;
	CStringArray sa;
	CString sCam;
	int i, nCount = m_cCameras.GetCount();
	for (i=0; i<nCount; i++)
	{
		m_cCameras.GetLBText(i, sCam);
		sa.Add(sCam);
	}
	wkp.WriteMultiString(USB_CAM_UNIT, USB_CAMERAS, sa);
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::InitResetTimer()
{
	if (GetTimerByID(TIMER_RESET) == 0)
	{
		SetTimer(TIMER_RESET, 1000);
	}
}
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
LRESULT CUSBCameraUnitDlg::OnDecodeMediaData(WPARAM wParam, LPARAM lParam)
{
	// TODO! RKE: dwFlags auswerten
	CIPCMediaSampleRecord *pRec = (CIPCMediaSampleRecord*)wParam;
	DWORD dwFlags = lParam;
	HRESULT hr = NOERROR;
	pRec->InitFromBubble();
	int nPNo = pRec->GetPackageNumber();
	if (nPNo == 0)																// Bei Packet 0 neu initialisieren
	{
		WK_TRACE(_T("Initialize Media Decoder\n"), nPNo);

		OnReleaseAllFilters();

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

		{
			CString sMajorType, sSubType, sFormat;
			GetGUIDString(sMajorType, &m_MediaType.majortype, TRUE);
			GetGUIDString(sSubType, &m_MediaType.subtype, TRUE);
			GetGUIDString(sFormat, &m_MediaType.formattype, TRUE);
			if (sSubType == _T("NULL"))
			{
				FOURCCMap map;
				map.SetFOURCC(&m_MediaType.subtype);
				DWORD dwFourCC = map.GetFOURCC();
				char cTemp = 0;
				sSubType = CString((char*)&dwFourCC).Left(4);
			}
			SetDlgItemText(IDC_STR_MEDIA_SUB_TYPE, sMajorType + _T(", ") + sSubType + _T(", ") + sFormat);
		}

		pRec->GetAllocatorProperties(m_AP.cBuffers, m_AP.cbBuffer, m_AP.cbAlign, m_AP.cbPrefix);
		m_AP.cBuffers = 1;
		if (m_AP.cbBuffer == 0)
		{
			m_AP.cbBuffer = pRec->GetSampleSize();
			m_AP.cbPrefix = 0;
			m_AP.cbAlign  = 1;
		}

		OnInitVideoPlay(); 
		m_sCameraState[m_nCamIndex].bInitialized = TRUE;
		m_sCameraState[m_nCamIndex].bVideoPlay   = TRUE;
		SetControlStates(m_sCameraState[m_nCamIndex]);
	}
	else if (m_pMediaSampleSource == NULL)								// Packetnummer > 0 und nicht initialisiert ?
	{
		WK_TRACE(_T("ReInitialize Media Decoder\n"), nPNo);

		OnInitVideoPlay();
	}

	if (m_pIMediaSampleSource)												// neuen Puffer setzen
	{
		PushSourceProperties msp;
		pRec->GetReferenceTimes(&msp.rtTimeStart, &msp.rtTimeEnd);
		pRec->GetMediaTimes(&msp.llTimeStart, &msp.llTimeEnd);
		msp.bIsPreroll       = pRec->GetIsPreroll();		// Media properties
		msp.bIsDiscontinuity = pRec->GetIsDiscontinuty();
		msp.bIsSyncPoint     = pRec->GetIsSyncPoint();
		msp.nLength          = pRec->GetActualLength();				// Length
		msp.pBuffer          = pRec->DetachData();					// Detached data is freed in ReleaseBufferFunction
		hr = m_pIMediaSampleSource->SetBuffer(&msp);
		if (FAILED(hr))
		{
			ReportError(HRESULT_ERROR_AT_POS(hr));
		}
	}
	if (m_btnStart.IsWindowEnabled())
	{
		OnBtnStart();
	}
	if (m_bShowPicture)
	{
		PostMessage(WM_TIMER, EVENT_UPDATE_FRAME, NULL);
	}
	WK_DELETE(pRec);
	return 0;
}
CFile file;
//////////////////////////////////////////////////////////////////////////
LRESULT CUSBCameraUnitDlg::OnConfirmPictureRecord(WPARAM wParam, LPARAM lParam)
{
	CIPCPictureRecord *pPict = (CIPCPictureRecord*)wParam;
	DWORD dwFlags = lParam;
	HRESULT hr = NOERROR;
	TRACE(_T("BlockNr:%d\n"), pPict->GetBlockNr());
	int nPNo = 0;//pRec->GetPackageNumber();
//	if (nPNo == 0)															// Bei Packet 0 neu initialisieren
//	pPict->GetPictureType() == CIPCSavePictureType::SPT_FULL_PICTURE

	const BYTE *pData = pPict->GetData();
	long  lDatalength = pPict->GetDataLength();
	if (file.m_hFile == (HANDLE)0xffffffff)
	{
		file.Open(_T("c:\\test.mp4"), CFile::modeWrite|CFile::modeCreate);
	}
	file.Write(pData, lDatalength);

	BITMAPINFOHEADER *pBMI = (BITMAPINFOHEADER*) pData;
	if (pBMI->biSize == sizeof(BITMAPINFOHEADER))
	{
		pData += pBMI->biSize;
		lDatalength -= pBMI->biSize;
	}

	if (m_MediaType.pbFormat)
	{
		GUID guidsubtype =GUID_NULL;
		switch (pPict->GetCompressionType())
		{
			case COMPRESSION_YUV_422:
				guidsubtype = MEDIASUBTYPE_YUY2;
				break;
			case COMPRESSION_JPEG:
			{
				guidsubtype = MEDIASUBTYPE_MJPG;
			}	break;
			case COMPRESSION_RGB_24:
				guidsubtype = MEDIASUBTYPE_RGB24;
				break;
			case COMPRESSION_MPEG4:
				guidsubtype = MEDIASUBTYPE_YUY2;
				guidsubtype.Data1 = mmioFOURCC('M', 'P', '4', '2');
//				guidsubtype.Data1 = mmioFOURCC('M', 'P', 'G', '4');
// TODO! RKE: compare when ready, this is only for test
//				guidsubtype = m_MediaType.subtype;
				break;
//			case CompressionType::COMPRESSION_H263:
//				guidsubtype              = MEDIASUBTYPE_;
//				break;
//			case CompressionType::COMPRESSION_PRESENCE:
//				guidsubtype              = MEDIASUBTYPE_;
//				break;
		}

		if (guidsubtype != m_MediaType.subtype)
		{
			OnReleaseAllFilters();
			FreeMediaType(m_MediaType);
			m_MediaType.pbFormat = NULL;
		}
	}

	if (m_pMediaSampleSource == NULL)
	{
		if (m_MediaType.pbFormat == NULL)
		{
			m_MediaType.majortype            = MEDIATYPE_Video;
			BOOL bFixedSizeSamples = FALSE;
			BOOL bTemporalCompression = FALSE;

			m_MediaType.formattype           = WMFORMAT_VideoInfo;
			m_MediaType.cbFormat             = sizeof(VIDEOINFOHEADER);
			m_MediaType.pbFormat = (PBYTE)CoTaskMemAlloc(m_MediaType.cbFormat);
			m_MediaType.pUnk = NULL;
			ZeroMemory(m_MediaType.pbFormat, m_MediaType.cbFormat);
			VIDEOINFOHEADER*pVIH = (VIDEOINFOHEADER*)m_MediaType.pbFormat;
			if (pData != pPict->GetData())
			{
				memcpy(&pVIH->bmiHeader, pPict->GetData(), sizeof(BITMAPINFOHEADER));
			}

	//		pVIH->rcSource	?
	//		pVIH->rcTarget  ?
			pVIH->dwBitRate	= 24330118;			// ?
			pVIH->AvgTimePerFrame =	666670;		// ?
			pVIH->dwBitErrorRate = 0;			// ?

			switch (pPict->GetCompressionType())
			{
				case COMPRESSION_YUV_422:
					m_MediaType.subtype = MEDIASUBTYPE_YUY2;
					bFixedSizeSamples = TRUE;
					pVIH->bmiHeader.biCompression = m_MediaType.subtype.Data1;
					break;
				case COMPRESSION_JPEG:
				{
					m_MediaType.subtype = MEDIASUBTYPE_MJPG;
					CJpeg jpg;
					jpg.GetBMIfromJPGdata(pData, lDatalength, &pVIH->bmiHeader);
					pVIH->bmiHeader.biCompression = m_MediaType.subtype.Data1;
					pVIH->dwBitRate	= 36495178;
					pVIH->AvgTimePerFrame =	666670;
					pVIH->bmiHeader.biSizeImage = lDatalength;
				}	break;
				case COMPRESSION_RGB_24:
					m_MediaType.subtype = MEDIASUBTYPE_RGB24;
					bFixedSizeSamples = TRUE;
					pVIH->bmiHeader.biCompression = COMPRESSION_RGB_24;
					break;
				case COMPRESSION_MPEG4:
					m_MediaType.subtype = MEDIASUBTYPE_YUY2;
					m_MediaType.subtype.Data1 = mmioFOURCC('M', 'P', '4', '2');
//					m_MediaType.subtype.Data1 = mmioFOURCC('M', 'P', 'G', '4');
//					m_MediaType.subtype.Data1 = mmioFOURCC('D', 'I', 'V', 'X');
//					m_MediaType.subtype.Data1 = mmioFOURCC('X', 'V', 'I', 'D');

					pVIH->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					pVIH->bmiHeader.biWidth = 704;
					pVIH->bmiHeader.biHeight = 288;
					pVIH->bmiHeader.biPlanes = 1;
					pVIH->bmiHeader.biBitCount = 16;
					pVIH->bmiHeader.biCompression = m_MediaType.subtype.Data1;
					pVIH->bmiHeader.biSizeImage = lDatalength;
					break;
	//			case CompressionType::COMPRESSION_H263:
	//				m_MediaType.subtype              = MEDIASUBTYPE_;
	//				break;
	//			case CompressionType::COMPRESSION_PRESENCE:
	//				m_MediaType.subtype              = MEDIASUBTYPE_;
	//				break;
			}

			m_MediaType.bFixedSizeSamples    = bFixedSizeSamples;
			m_MediaType.bTemporalCompression = bTemporalCompression;
			m_MediaType.lSampleSize          = lDatalength;
			m_AP.cBuffers = 1;
			m_AP.cbBuffer = lDatalength;
			if (!bFixedSizeSamples)
			{
				m_AP.cbBuffer = MulDiv(m_AP.cbBuffer, 120, 100); // + 20 %
			}
			m_AP.cbPrefix = 0;
			m_AP.cbAlign  = 1;

			OnInitVideoPlay(); 
			m_sCameraState[m_nCamIndex].bInitialized = TRUE;
			m_sCameraState[m_nCamIndex].bVideoPlay   = TRUE;
			SetControlStates(m_sCameraState[m_nCamIndex]);
		}
		else
		{
			WK_TRACE(_T("ReInitialize Media Decoder\n"), nPNo);
			OnInitVideoPlay();
		}
	}

	if (m_pIMediaSampleSource)												// neuen Puffer setzen
	{
	//	LARGE_INTEGER 
		PushSourceProperties msp;
		CSystemTime time = pPict->GetTimeStamp();
		LARGE_INTEGER liResult;
		FILETIME ftResult;

		ftResult.dwLowDateTime = 0; 
		ftResult.dwHighDateTime = 0; 
		liResult.LowPart = 0;
		liResult.HighPart = 0;

		if (!SystemTimeToFileTime(&time,&ftResult))
		{
			WK_TRACE(_T("SystemTimeToFileTime failed, %s\n"), GetLastErrorString());
		}

		liResult.LowPart = ftResult.dwLowDateTime;
		liResult.HighPart = ftResult.dwHighDateTime;

//		msp.nSampleNumber = wBlockNr;
//		memcpy(&msp.rtTimeStart, &liResult, sizeof(LARGE_INTEGER));
//		msp.rtTimeEnd = msp.rtTimeStart + 666670;
//		pRec->GetReferenceTimes(&msp.rtTimeStart, &msp.rtTimeEnd);
//		pRec->GetMediaTimes(&msp.llTimeStart, &msp.llTimeEnd);

		msp.bIsPreroll       = FALSE;		// Media properties
		msp.bIsDiscontinuity = pPict->GetPictureType() == SPT_DIFF_PICTURE ? TRUE : FALSE;
		msp.bIsSyncPoint     = pPict->GetPictureType() == SPT_DIFF_PICTURE ? FALSE : TRUE;
		msp.nLength          = lDatalength;		// Length
		msp.pBuffer			 = new BYTE[msp.nLength];
		memcpy(msp.pBuffer, pData, msp.nLength);	// Detached data is freed in ReleaseBufferFunction

		hr = m_pIMediaSampleSource->SetBuffer(&msp);
		if (FAILED(hr))
		{
			ReportError(HRESULT_ERROR_AT_POS(hr));
		}
	}
	if (m_btnStart.IsWindowEnabled())
	{
		OnBtnStart();
	}
	if (m_bShowPicture)
	{
		PostMessage(WM_TIMER, EVENT_UPDATE_FRAME, NULL);
	}

	WK_DELETE(pPict);
	return 0;
}
//////////////////////////////////////////////////////////////////////////
#define DIRECT_WITH_SELECTED_FILTER 0
#define VIDEO_RENDERER 1
#define SAMP2MEM       2
#define DIRECT_WITH_SELECTED_FILTER2 3

void CUSBCameraUnitDlg::OnInitVideoPlay() 
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	HRESULT hr = NOERROR;
	BOOL         bFound = 0;
	IPin         *pPinTemp = NULL;
	IEnumFilters *pEnum = NULL;
	IBaseFilter  *pFilter = NULL,
				    *pDump   = NULL,
					 *pDecoder  = NULL,
					 *pRender   = NULL,
					 *pSmartTee = NULL;

	try
	{
		OnRemoveFromRot();
		if (m_MediaType.pbFormat == NULL) throw HRESULT_ERROR_AT_POS(ERROR_INVALID_MEDIA);
		InitBasicFilters();
		int nReceive =	m_cTest.GetCurSel();
		
		// Source Filter CLSID_MEDIA_SOURCE_FILTER mit direkter Verbindung zu Dump funktioniert !!
		if ((m_pMediaSampleSource==NULL) && (nReceive == DIRECT_WITH_SELECTED_FILTER))
		{
			IMediaFilter *pMF;
			hr = m_pGraph->QueryInterface(IID_IMediaFilter, (void**)&pMF);
			if (SUCCEEDED(hr))
			{
				pMF->SetSyncSource(NULL);
				pMF->Release();
			}
			// Create the MediaSampleSource Filter 
			HR_EXCEPTION(AddFilterToGraph(m_pGraph, &m_pMediaSampleSource, CLSID_PushSource, L"PushSource"));

			HR_EXCEPTION(m_pMediaSampleSource->QueryInterface(IID_IPushSource, (void**) &m_pIMediaSampleSource));

			HR_EXCEPTION(m_pIMediaSampleSource->SetMediaType(&m_MediaType));
			if (m_MediaType.subtype.Data1 == mmioFOURCC('M', 'P', '4', '2'))
			{
				m_MediaType.subtype.Data1 = mmioFOURCC('M', 'P', 'G', '4');
				HR_EXCEPTION(m_pIMediaSampleSource->SetMediaType(&m_MediaType));
				m_MediaType.subtype.Data1 = mmioFOURCC('D', 'I', 'V', 'X');
				HR_EXCEPTION(m_pIMediaSampleSource->SetMediaType(&m_MediaType));
				m_MediaType.subtype.Data1 = mmioFOURCC('X', 'V', 'I', 'D');
				HR_EXCEPTION(m_pIMediaSampleSource->SetMediaType(&m_MediaType));
				m_MediaType.subtype.Data1 = mmioFOURCC('M', 'P', '4', '2');
			}

			HR_EXCEPTION(m_pIMediaSampleSource->SetBufferType(BUFFER_ATTACH, ReleaseBufferFunction, (long)0, &m_AP));

			HR_EXCEPTION(GetPin(m_pMediaSampleSource, PINDIR_OUTPUT , 0, &pPinTemp));

			IKsPropertySet *pPS = NULL;
			hr = pPinTemp->QueryInterface(IID_IKsPropertySet, (void **)&pPS);
			if (SUCCEEDED(hr))
			{
				pPS->Set(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0,  (void*)&PIN_CATEGORY_CAPTURE, sizeof(GUID));
				pPS->Release();
			}
			RELEASE_OBJECT(pPinTemp);

			int nSel = m_cEncoders.GetCurSel();
			if (nSel > 0)
			{
				IMoniker *pM = (IMoniker*)m_cEncoders.GetItemData(nSel);
				CString str;
				m_cEncoders.GetLBText(nSel, str);
				BSTR bstr = str.AllocSysString();
				hr = AddFilterToGraph(m_pGraph, &m_pEncoder[0], pM, bstr);
				SysFreeString(bstr);
				HR_EXCEPTION(hr);			// get the dump filter
			}
			else
			{
				nSel = m_cTestFilters.GetCurSel();
				if (nSel > 0)
				{
					IMoniker *pM = (IMoniker*)m_cTestFilters.GetItemData(nSel);
					CString str;
					m_cTestFilters.GetLBText(nSel, str);
					BSTR bstr = str.AllocSysString();
					hr = AddFilterToGraph(m_pGraph, &m_pEncoder[0], pM, bstr);
					SysFreeString(bstr);
					HR_EXCEPTION(hr);			// get the dump filter
					HR_EXCEPTION(ConnectFilters(m_pGraph, m_pMediaSampleSource, 0, m_pEncoder[0], 0));
					HR_EXCEPTION(GetPin(m_pEncoder[0], PINDIR_OUTPUT , 0, &pPinTemp));
					hr = m_pGraph->Render(pPinTemp);
					HR_EXCEPTION(hr);			// 
					InitVideoDisplay();
				}
			}
/*
			// Create the SampleGrabber Filter 
			HR_EXCEPTION(AddFilterToGraph(m_pGraph, &pDump, CLSID_SampleGrabber, L"Sample Grabber"));

			ISampleGrabber *pGrab = NULL;
			HR_EXCEPTION(pDump->QueryInterface(IID_ISampleGrabber, (void**)&pGrab));
			CMediaType GrabType;
			GrabType.SetType( &MEDIATYPE_Video );
			GrabType.SetSubtype( &MEDIASUBTYPE_YUY2);
			hr = pGrab->SetMediaType( &GrabType );

			// Create the NullRenderer Filter 
			HR_EXCEPTION(AddFilterToGraph(m_pGraph, &pFilter, CLSID_NullRenderer, L"Null Renderer"));
				
			if (nSel > 0)
			{
				HR_EXCEPTION(ConnectFilters(m_pGraph, m_pMediaSampleSource, 0, m_pEncoder[0], 0));
				HR_EXCEPTION(ConnectFilters(m_pGraph, m_pEncoder[0], 0, pDump, 0));
			}
			else
			{
				HR_EXCEPTION(ConnectFilters(m_pGraph, m_pMediaSampleSource, 0, pDump, 0));
			}
			HR_EXCEPTION(ConnectFilters(m_pGraph, pDump, 0, pFilter, 0));

			hr = pGrab->SetBufferSamples(FALSE);
			hr = pGrab->SetOneShot( FALSE );
			hr = pGrab->SetCallback(&g_CB, 1);
			RELEASE_OBJECT(pGrab);
*/
		}

		// Source Filter zum anzeigen der empfangenen Daten
		if ((m_pMediaSampleSource==NULL) && (nReceive == VIDEO_RENDERER))
		{
			IAMGraphStreams *pAMGS;
			hr = m_pGraph->QueryInterface(IID_IAMGraphStreams, (void**)&pAMGS);
			if (SUCCEEDED(hr))
			{
				pAMGS->SyncUsingStreamOffset(TRUE);
				pAMGS->Release();
			}

			// Create the MediaSampleSource Filter 
			HR_EXCEPTION(AddFilterToGraph(m_pGraph, &m_pMediaSampleSource, CLSID_PushSource, L"PushSource"));

			HR_EXCEPTION(m_pMediaSampleSource->QueryInterface(IID_IPushSource, (void**) &m_pIMediaSampleSource));

			PushSourceParams psp;
			ZERO_INIT(psp);
			psp.m_bCapture            = 0;
			psp.m_bGenerateTimeStamps = TRUE;
			psp.m_nOffsetInMS         = 50;
			psp.m_bInternalRM         = FALSE;
			psp.m_bLiveSource         = FALSE;
			psp.m_bPreview            = FALSE;
			psp.m_bStill			  = FALSE;
			psp.m_lMiscFlags          = 0;//AM_FILTER_MISC_FLAGS_IS_SOURCE;
			psp.m_bSortSamples        = FALSE; // TODO! RKE: SortSamples nur, wenn bei Sample 0 zurückgesetzt wird!
			m_pIMediaSampleSource->SetParams(&psp);

			HR_EXCEPTION(m_pIMediaSampleSource->SetMediaType(&m_MediaType));
/*
			if (m_MediaType.subtype.Data1 == mmioFOURCC('M', 'P', '4', '2'))
			{
				m_MediaType.subtype.Data1 = mmioFOURCC('M', 'P', 'G', '4');
				HR_EXCEPTION(m_pIMediaSampleSource->SetMediaType(&m_MediaType));
				m_MediaType.subtype.Data1 = mmioFOURCC('D', 'I', 'V', 'X');
				HR_EXCEPTION(m_pIMediaSampleSource->SetMediaType(&m_MediaType));
				m_MediaType.subtype.Data1 = mmioFOURCC('X', 'V', 'I', 'D');
				HR_EXCEPTION(m_pIMediaSampleSource->SetMediaType(&m_MediaType));
			}
*/
			HR_EXCEPTION(m_pIMediaSampleSource->SetBufferType(BUFFER_ATTACH, ReleaseBufferFunction, (long)0, &m_AP));

			HR_EXCEPTION(GetPin(m_pMediaSampleSource, PINDIR_OUTPUT , 0, &pPinTemp));

			IKsPropertySet *pPS = NULL;
			hr = pPinTemp->QueryInterface(IID_IKsPropertySet, (void **)&pPS);
			GUID guid;
			if (SUCCEEDED(hr))
			{
				CString str;
				DWORD dwSize;
				pPS->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0,  &guid, sizeof(GUID), &dwSize);
				GetGUIDString(str, &guid);
				pPS->Release();
			}

			hr = m_pGraphBuilder->RenderStream(&guid, &MEDIATYPE_Video, 
				  m_pMediaSampleSource,   // Capture filter
				  NULL,                   // Compression filter (optional)
				  NULL						  // Multiplexer or renderer filter
				  );

			InitVideoDisplay();

			IMediaFilter *pMF;
			hr = m_pGraph->QueryInterface(IID_IMediaFilter, (void**)&pMF);
			if (SUCCEEDED(hr))
			{
				pMF->SetSyncSource(NULL);
				pMF->Release();
			}
		}

		// Rendering to the Sample2MemRenderer filter
		if ((m_pMediaSampleSource==NULL) && (nReceive == SAMP2MEM))
		{
			IMediaFilter *pMF;
			hr = m_pGraph->QueryInterface(IID_IMediaFilter, (void**)&pMF);
			if (SUCCEEDED(hr))
			{
				pMF->SetSyncSource(NULL);
				pMF->Release();
			}
			// Create the MediaSampleSource Filter 
			HR_EXCEPTION(AddFilterToGraph(m_pGraph, &m_pMediaSampleSource, CLSID_PushSource, L"PushSource"));
			HR_EXCEPTION(m_pMediaSampleSource->QueryInterface(IID_IPushSource, (void**) &m_pIMediaSampleSource));
			HR_EXCEPTION(m_pIMediaSampleSource->SetMediaType(&m_MediaType));
			HR_EXCEPTION(m_pIMediaSampleSource->SetBufferType(BUFFER_ATTACH, ReleaseBufferFunction, (long)0, &m_AP));
/*
			HR_EXCEPTION(AddFilterToGraph(m_pGraph, &pDump, CLSID_DUMPFILTER, L"Dump"));
			IDump*pIDump = NULL;
			HR_EXCEPTION(pDump->QueryInterface(IID_IDump, (void **)&pIDump));
			hr = pIDump->SetReceiveFunction(TestMediaSample, (long)this);
*/
			HR_EXCEPTION(AddFilterToGraph(m_pGraph, &pDump, CLSID_Sample2MemRenderer, L"Sample2Mem"));
			ISample2MemRenderer *pIDump = NULL;
			HR_EXCEPTION(pDump->QueryInterface(IID_ISample2MemRenderer, (void **)&pIDump));
			hr = pIDump->SetReceiveFunction(TestMediaSample, (long)this);

			AM_MEDIA_TYPE mt[3];
			ZeroMemory(mt, sizeof(AM_MEDIA_TYPE)*3);
			mt[0].majortype  = MEDIATYPE_Video;
			mt[0].subtype    = MEDIASUBTYPE_YUY2;
			mt[1].majortype  = MEDIATYPE_Video;
			mt[1].subtype    = MEDIASUBTYPE_YUYV;
			mt[2].majortype  = MEDIATYPE_Video;
			mt[2].subtype    = MEDIASUBTYPE_RGB32;

			pIDump->SetAcceptedMediaTypes(&mt[0], 3, TRUE);
			RELEASE_OBJECT(pIDump);

			HR_EXCEPTION(GetPin(m_pMediaSampleSource, PINDIR_OUTPUT , 0, &pPinTemp));
			IKsPropertySet *pPS = NULL;
			GUID guid;
			hr = pPinTemp->QueryInterface(IID_IKsPropertySet, (void **)&pPS);
			if (SUCCEEDED(hr))
			{
				CString str;
				DWORD dwSize;
				pPS->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0,  &guid, sizeof(GUID), &dwSize);
				GetGUIDString(str, &guid);
				pPS->Release();
			}
			RELEASE_OBJECT(pPinTemp);

			HR_EXCEPTION(m_pGraphBuilder->RenderStream(&guid, &MEDIATYPE_Video, 
				  m_pMediaSampleSource,   // Capture filter
				  NULL,                   // Compression filter (optional)
				  pDump						  // Multiplexer or renderer filter
				  ));

			ULONG         cFetched;
			int nFilter = 0;
			hr = m_pGraph->EnumFilters(&pEnum);	// Finde die Filter der Kamera (n)
			if (SUCCEEDED(hr))
			{
				while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
				{
					FILTER_INFO FilterInfo;
					hr = pFilter->QueryFilterInfo(&FilterInfo);
					if (SUCCEEDED(hr))
					{
						FilterInfo.pGraph->Release();
						IOverlay*pOvn = NULL;
						hr = pFilter->QueryInterface(IID_IOverlay, (void**)&pOvn);
						if (SUCCEEDED(hr))
						{
							CString str(FilterInfo.achName);
							TRACE(_T("%s contains IOverlay\n"), str);
							pOvn->Release();
						}						
						IQualityControl*pQC = NULL;
						hr = pFilter->QueryInterface(IID_IQualityControl, (void**)&pQC);
						if (SUCCEEDED(hr))
						{
							CString str(FilterInfo.achName);
							TRACE(_T("%s contains IQualityControl\n"), str);
							pQC->Release();
						}						
						RELEASE_OBJECT(pFilter);
					}
					nFilter++;
				}
				pEnum->Release();
			}
		}
		if ((m_pMediaSampleSource==NULL) && (nReceive == DIRECT_WITH_SELECTED_FILTER2))
		{
			// Create the MediaSampleSource Filter 
			HR_EXCEPTION(AddFilterToGraph(m_pGraph, &m_pMediaSampleSource, CLSID_PushSource, L"PushSource"));
			HR_EXCEPTION(m_pMediaSampleSource->QueryInterface(IID_IPushSource, (void**) &m_pIMediaSampleSource));
			HR_EXCEPTION(m_pIMediaSampleSource->SetMediaType(&m_MediaType));
			HR_EXCEPTION(m_pIMediaSampleSource->SetBufferType(BUFFER_ATTACH, ReleaseBufferFunction, (long)0, &m_AP));

			HR_EXCEPTION(GetPin(m_pMediaSampleSource, PINDIR_OUTPUT , 0, &pPinTemp));

			int nSel = m_cTestFilters.GetCurSel();
			if (nSel > 0)
			{
				IMoniker *pM = (IMoniker*)m_cTestFilters.GetItemData(nSel);
				CString str;
				m_cTestFilters.GetLBText(nSel, str);
				BSTR bstr = str.AllocSysString();
				hr = AddFilterToGraph(m_pGraph, &m_pEncoder[0], pM, bstr);
				SysFreeString(bstr);
				HR_EXCEPTION(hr);			// get the dump filter
			}
/*
			hHR_EXCEPTION(AddFilterToGraph(m_pGraph, &pDump, CLSID_DUMPFILTER, L"Dump"));
			IDump*pIDump = NULL;
			HR_EXCEPTION(pDump->QueryInterface(IID_IDump, (void **)&pIDump));
*/
			HR_EXCEPTION(AddFilterToGraph(m_pGraph, &pDump, CLSID_Sample2MemRenderer, L"Sample Dump"));
			ISample2MemRenderer *pIDump = NULL;
			HR_EXCEPTION(pDump->QueryInterface(IID_ISample2MemRenderer, (void **)&pIDump));
			
			HR_EXCEPTION(pIDump->SetReceiveFunction(TestMediaSample, (long)this));
			RELEASE_OBJECT(pIDump);

			if (nSel > 0)
			{
				HR_EXCEPTION(AddFilterToGraph(m_pGraph, &pSmartTee, CLSID_Colour, L"MUX"));

				HR_EXCEPTION(ConnectFilters(m_pGraph, m_pMediaSampleSource, 0, m_pEncoder[0], 0));

				HR_EXCEPTION(ConnectFilters(m_pGraph, m_pEncoder[0], 0, pSmartTee, 0));

				HR_EXCEPTION(ConnectFilters(m_pGraph, pSmartTee, 0, pDump, 0));
			}
			else
			{
				HR_EXCEPTION(ConnectFilters(m_pGraph, m_pMediaSampleSource, 0, pDump, 0));
			}
		}
		m_sCameraState[0].bInitialized = TRUE;
		m_sCameraState[0].bVideoPlay   = TRUE;
		SetControlStates(m_sCameraState[0]);
	}
   catch(ErrorStruct *ps)
	{
		if (m_MediaType.pbFormat != NULL)
		{
			FreeMediaType(m_MediaType);
		}
		ReportError(ps);
		CameraStates s = {0};
		SetControlStates(s);
		OnReleaseAllFilters();
	}
	RELEASE_OBJECT(pDump);
	RELEASE_OBJECT(pFilter);
	RELEASE_OBJECT(pPinTemp);
	RELEASE_OBJECT(pDecoder);
	RELEASE_OBJECT(pSmartTee);
}
//////////////////////////////////////////////////////////////////////////
void CUSBCameraUnitDlg::DisplayPicture(VIDEOINFOHEADER *pVIH, void *pData, int nLen)
{
	BYTE *pRGB     = NULL;
	BYTE *pPicture = NULL;
	DWORD dwCompression = pVIH->bmiHeader.biCompression;
	DWORD dwBitCount    = pVIH->bmiHeader.biBitCount;
	if (pVIH->bmiHeader.biCompression == mmioFOURCC('Y', 'U', 'Y', '2'))
	{
		int red, green, blue, rt, gt, bt, i, j, nTupels, nPixels;
		YUV422 *pBuffer;
		i = pVIH->bmiHeader.biBitCount / 8;
		if (nLen == 0)
		{
			nLen = abs(i * pVIH->bmiHeader.biHeight * pVIH->bmiHeader.biWidth);
		}
		nPixels = nLen / i;
		nTupels = nPixels / 2;
		pBuffer = (YUV422*) pData;
		pRGB    =  new BYTE[nLen*4];
		if (m_bColor)
		{
			for (i=0, j=0; i<nTupels; i++)
			{
				bt = (( 91881*(pBuffer[i].bV-128))>>16);
				gt = (( 22554*(pBuffer[i].bU-128) + 46802*(pBuffer[i].bV-128))>>16);
				rt = ((116129*(pBuffer[i].bU-128))>>16);

				((COLORREF*)pRGB)[j++] = RGB(CheckRange(pBuffer[i].bY1 + rt, 0, 255),	// red
													  CheckRange(pBuffer[i].bY1 - gt, 0, 255),	// green
													  CheckRange(pBuffer[i].bY1 + bt, 0, 255));// blue

				((COLORREF*)pRGB)[j++] = RGB(CheckRange(pBuffer[i].bY2 + rt, 0, 255),	// red
													  CheckRange(pBuffer[i].bY2 - gt, 0, 255),	// green
													  CheckRange(pBuffer[i].bY2 + bt, 0, 255));// blue
			}
		}
		else
		{
			for (i=0, j=0; i<nTupels; i++)
			{
				red = green = blue = pBuffer[i].bY1;
				((COLORREF*)pRGB)[j++] = RGB(red, green, blue);

				red = green = blue = pBuffer[i].bY2;
				((COLORREF*)pRGB)[j++] = RGB(red, green, blue);
			}
		}
		pVIH->bmiHeader.biBitCount = 32;
		pVIH->bmiHeader.biCompression = BI_RGB;
		pPicture = pRGB;
	}
	else
	{
		pPicture = (BYTE*) pData;
	}
	
	CDC *pDC = m_cFrame.GetDC();
	CRect rc;
	m_cFrame.GetClientRect(&rc);
	pDC->SetStretchBltMode(HALFTONE);
	BOOL bReturn = ::StretchDIBits(pDC->m_hDC,
		rc.left, rc.Height(), rc.Width(), -rc.Height()-1,
		0, 0, pVIH->bmiHeader.biWidth, abs(pVIH->bmiHeader.biHeight),
		pPicture, (BITMAPINFO*)&pVIH->bmiHeader, DIB_RGB_COLORS, SRCCOPY);

	pVIH->bmiHeader.biCompression = dwCompression;
	pVIH->bmiHeader.biBitCount = (WORD) dwBitCount;
	WK_DELETE(pRGB);
}
//////////////////////////////////////////////////////////////////////////
HRESULT WINAPI CUSBCameraUnitDlg::TestMediaSample(IMediaSample *pSample, long nPackageNo, long lParam)
{
	CUSBCameraUnitDlg *pThis = (CUSBCameraUnitDlg*)lParam;
	AM_MEDIA_TYPE *pMT=NULL;
	BYTE*pBuffer;
	pSample->GetPointer(&pBuffer);
	pSample->GetMediaType(&pMT);

#ifdef _DEBUG
	pThis->DisplayPicture((VIDEOINFOHEADER*)pMT->pbFormat, pBuffer, pSample->GetActualDataLength());
#endif
	
	TRACE(_T("TestMediaSample\n"));

	return NOERROR;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CUSBCameraUnitDlg::FindMatchingFilter2(DWORD dwMerit, IPin *pPin, 
														 BOOL bInputNeeded, GUID majortypeIn, GUID subtypeIn, 
														 BOOL bOutputNeeded, GUID majortypeOut, GUID* subtypeOut, int nSubTypes, 
														 BOOL bRender,
														 IBaseFilter **ppFilter, AM_MEDIA_TYPE **ppMT, LPCWSTR pszFilterName)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	HRESULT          hr = NULL;
	IFilterMapper2   *pMapper = NULL;
	IEnumMoniker     *pEnumMoniker = NULL;
	IPin            *pPinIn = NULL, *pPinOut = NULL;
	IMoniker*pMoniker = NULL;

	if (ppFilter == NULL) return E_POINTER;
	if (ppMT     == NULL) return E_POINTER;
	
	hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC, IID_IFilterMapper, (void **) &pMapper);
	if (FAILED(hr)) return hr;
	GUID pInputTypes[2];
	pInputTypes[0] = majortypeIn;
	pInputTypes[1] = subtypeIn;
	GUID pOutputTypes[4];
	pOutputTypes[0] = majortypeOut;
	pOutputTypes[1] = subtypeOut[0];
	pOutputTypes[2] = majortypeOut;
	pOutputTypes[3] = subtypeOut[1];

	hr = pMapper->EnumMatchingFilters(&pEnumMoniker,
		0,			// Flags
		TRUE,		// ExactMatch
		dwMerit,
		bInputNeeded,
		1, pInputTypes, NULL, NULL,
		bRender,
		bOutputNeeded,
		2, pOutputTypes, NULL, NULL);

	AM_MEDIA_TYPE *pmt = *ppMT;
	*ppMT = NULL;
	if (SUCCEEDED(hr))
	{
		pEnumMoniker->Reset();
		ULONG ulCatched;
		while (SUCCEEDED(pEnumMoniker->Next(1, &pMoniker, &ulCatched)) && pMoniker)
		{
			hr = AddFilterToGraph(m_pGraph, ppFilter, pMoniker, pszFilterName);
			if (SUCCEEDED(hr))
			{
				hr = GetPin(*ppFilter, PINDIR_INPUT, 0, &pPinIn);
				if (SUCCEEDED(hr))
				{
//					hr = pPin->Connect(pPinIn, pmt);
//					if (FAILED(hr))
					{
						hr = m_pGraph->Connect(pPin, pPinIn);
					}
					if (hr == S_OK)
					{
						IEnumMediaTypes *pEMT = NULL;
						RELEASE_OBJECT(pPinOut);
						HR_EXCEPTION(GetPin(*ppFilter, PINDIR_OUTPUT, 0, &pPinOut));
						hr = pPinOut->EnumMediaTypes(&pEMT);
						if (SUCCEEDED(hr))
						{
							hr = E_FAIL;
							pEMT->Reset();
							while ((pEMT->Next(1, ppMT, NULL) == S_OK) && ((*ppMT) != NULL))
							{
								for (int i=0; i<nSubTypes; i++)
								{
									if ((*ppMT)->subtype == subtypeOut[i])
									{
										hr = S_OK;
//										GetDlgItem(IDC_TXT_FILTER)->SetWindowText(str);
										break;
									}
								}
								if (i<nSubTypes) break;
								DeleteMediaType(*ppMT);
								*ppMT = NULL;
							}
						}
						RELEASE_OBJECT(pEMT);
						break;
					}
					else
					{
						ReportError(HRESULT_ERROR_AT_POS(hr), false);
					}
					RELEASE_OBJECT(pPinIn);
				}
				m_pGraph->RemoveFilter(*ppFilter);
				RELEASE_OBJECT((*ppFilter));
			}
			RELEASE_OBJECT(pMoniker);
		}
		RELEASE_OBJECT(pEnumMoniker);
	}
	RELEASE_OBJECT(pMapper);
	RELEASE_OBJECT(pPinIn);
	RELEASE_OBJECT(pPinOut);
	return hr;
}
//////////////////////////////////////////////////////////////////////////
#ifdef __TEST 
void CUSBCameraUnitDlg::InitCameraFilters2(int nCam)
{
	ASSERT(theApp.GetMainTreadID() == GetCurrentThreadId());	
	HRESULT hr;
	IPin            *pPin      = NULL;
	IDump           *pIDump    = NULL;
	IInfTee         *pIInfTee  = NULL;
	IFileSinkFilter *pFileSink = NULL;
	IBaseFilter     *pFilter   = NULL,
					    *pDump     = NULL,
						 *pInfTee   = NULL,
						 *pDecoder  = NULL,
						 *pEncoder  = NULL,
						 *pRenderer = NULL;
	IEnumFilters    *pEnum     = NULL;
	FILTER_INFO     fi = {NULL, NULL};
	WCHAR           szFilterName[64];

	try
	{	
		if (nCam != CB_ERR)
		{
			if (m_pGraph == NULL)
			{
				InitBasicFilters();
			}
			BOOL bDeletePMTR = FALSE;
			if (m_pCapture[nCam] == NULL)
			{
				IMoniker*pMoniker = (IMoniker*)m_cCameras.GetItemData(nCam);
				if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);

				if (m_pCapture[nCam] == NULL)
				{
					swprintf(szFilterName, L"%d. Capture Device", nCam+1);
					HR_EXCEPTION(AddFilterToGraph(m_pGraph, &m_pCapture[nCam], pMoniker, szFilterName));
				}
			}

			int iPin = 0;
			if (m_sCameraState[nCam].nCapturePin != 0xf) iPin = m_sCameraState[nCam].nCapturePin;
			HR_EXCEPTION(GetPin(m_pCapture[nCam], PINDIR_OUTPUT, iPin, &pPin));

			IAMStreamConfig *pCfg      = NULL;
			hr = pPin->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
			AM_MEDIA_TYPE *pmtr = NULL,
				           *pmt  = (AM_MEDIA_TYPE*)m_cOutputFormats.GetItemData(m_cOutputFormats.GetCurSel());
			if (SUCCEEDED(hr))
			{
				if (pmt && (CB_ERR != (UINT)pmt))
				{
					hr = pCfg->SetFormat(pmt);
				}
				RELEASE_OBJECT(pCfg);
			}
			RELEASE_OBJECT(pPin);
			HR_EXCEPTION(m_pGraphBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, 
				  m_pCapture[nCam],       // Capture filter
				  NULL,                   // Compression filter (optional)
				  NULL						  // Multiplexer or renderer filter
			 ));


			HR_EXCEPTION(m_pGraph->QueryInterface(IID_IVideoWindow, (void**)&pVW));
			if (pVW)
			{
				CString str;
				m_cCameras.GetLBText(nCam, str);
				BSTR bstr = str.AllocSysString();
				hr = pVW->put_Caption(bstr);
				::SysFreeString(bstr);
			}

			int nFilter = 0;
			HR_EXCEPTION(m_pGraph->EnumFilters(&pEnum));							// get the created filters
			pEnum->Reset();
			while (SUCCEEDED(pEnum->Next(1, &pFilter, NULL)) && (pFilter != NULL))
			{
				HR_EXCEPTION(pFilter->QueryFilterInfo(&fi));
				CString str(fi.achName);
				if (nFilter == 0)												// Last filter is the Renderer
				{
					pRenderer = pFilter;
					pFilter = NULL;
				}
				else if (nFilter == 1)										// 2nd last is the decodig device
				{
					pDecoder = pFilter;
					pFilter = NULL;
				}
				RELEASE_OBJECT(fi.pGraph);
				RELEASE_OBJECT(pFilter);
				nFilter++;
			}
			RELEASE_OBJECT(pEnum);

			if (pRenderer)
			{
				HR_EXCEPTION(GetPin(pRenderer, PINDIR_INPUT, 0, &pPin));
				HR_EXCEPTION(m_pGraph->Disconnect(pPin));
				RELEASE_OBJECT(pPin);
			}
			if (pDecoder)
			{
				HR_EXCEPTION(GetPin(pDecoder, PINDIR_OUTPUT, 0, &pPin));
				HR_EXCEPTION(m_pGraph->Disconnect(pPin));
				RELEASE_OBJECT(pPin);
			}

			HR_EXCEPTION(AddFilterToGraph(m_pGraph, &pInfTee, CLSID_INF_TEE, L"Infinite Tee"));

			HR_EXCEPTION(GetPin(pInfTee, PINDIR_INPUT, 0, &pPin));
			HR_EXCEPTION(pPin->QueryInterface(CGuid(GUID_IINF_TEE), (void**)&pIInfTee));
			HR_EXCEPTION(pIInfTee->SetDataCheckFunction(0, CheckData, (long) nCam));
			RELEASE_OBJECT(pPin);
			
			AM_MEDIA_TYPE mt;
			ZeroMemory(&mt, sizeof(mt));
			mt.majortype = MEDIATYPE_Video;
			mt.subtype   = MEDIASUBTYPE_YUY2;
			mt.bFixedSizeSamples = TRUE;

			HR_EXCEPTION(ConnectFilters(m_pGraph, pDecoder, 0, pInfTee, 0, &mt));

			HR_EXCEPTION(AddFilterToGraph(m_pGraph, &pDump, CLSID_Sample2MemRenderer, L"Sample Dump"));
			HR_EXCEPTION(pDump->QueryInterface(IID_ISample2MemRenderer, (void **)&pIDump));
			HR_EXCEPTION(pIDump->SetReceiveFunction(ReceiveMediaSample, (long)nCam));

			HR_EXCEPTION(ConnectFilters(m_pGraph, pInfTee, 0, pRenderer, 0));

			HR_EXCEPTION(ConnectFilters(m_pGraph, pInfTee, 1, pDump, 0));

			m_sCameraState[nCam].bInitialized = TRUE;
			SetControlStates(m_sCameraState[nCam]);
		}
	}
   catch(ErrorStruct *ps)
	{
		ReportError(ps);
		ReleaseFilters(nCam);
	}

	RELEASE_OBJECT(pPin);
	RELEASE_OBJECT(pFilter);
	RELEASE_OBJECT(pEnum);
	RELEASE_OBJECT(pDump);
	RELEASE_OBJECT(pIDump);
	RELEASE_OBJECT(pInfTee);
	RELEASE_OBJECT(pIInfTee);
	RELEASE_OBJECT(pFileSink);
	RELEASE_OBJECT(pDecoder);
	RELEASE_OBJECT(pRenderer);
}
#endif // __TEST
/////////////////////////////////////////////////////////////////////////////
HRESULT  WINAPI CUSBCameraUnitDlg::ReleaseBufferFunction(void*pData, long lParam)
{
	ASSERT(gm_pThis != NULL);
//	pThis->DisplayPicture((VIDEOINFOHEADER*)pThis->m_MediaType.pbFormat, pData, 0);
	WK_DELETE(pData);
	TRACE(_T("Release Buffer\n"));
	return NOERROR;
}
#endif // _DEBUG

void CUSBCameraUnitDlg::OnBnClickedBtnServer()
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_BTN_SERVER, m_bServer);

	if (m_pOutput)
	{
		if (m_bServer)
		{
			m_pOutput->DoRequestSetGroupID(1);
			m_pOutput->DoRequestStartVideo(CSecID(SECID_GROUP_OUTPUT, 0),			// camera nr.
										RESOLUTION_HIGH,		// resolution
										COMPRESSION_1,		// image size or bitrate
										COMPRESSION_MPEG4,	// JPEG, YUV, MPEG4 ...
										5,				// frames per second
										5,   // for MPEG4 I-Frame interval, 1=I-Frame only
										0);		// unique server id
		}
		else
		{
			m_pOutput->DoRequestStopVideo(CSecID(SECID_GROUP_OUTPUT, 0), 0);
		}
	}
}
