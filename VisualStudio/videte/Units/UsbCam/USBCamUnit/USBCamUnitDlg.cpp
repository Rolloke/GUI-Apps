/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: USBCamUnitDlg.cpp $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCamUnit/USBCamUnitDlg.cpp $
// CHECKIN:		$Date: 17.11.03 14:01 $
// VERSION:		$Revision: 9 $
// LAST CHANGE:	$Modtime: 6.05.03 11:55 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "USBCamUnit.h"
#include "USBCamUnitDlg.h"

#include "CMotionDetection.h"
#include "CIPCInputUSBcamUnit.h"
#include "CIPCInputUSBcamUnitMDAlarm.h"

#include <atlbase.h>
#include <Dxerr8.h>
#include "BaseClasses\Streams.h"
#include "Dump\idump.h"
#include "inftee\iinftee.h"

#include "oemgui\oemguiapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define USB_CAM_UNIT     "USBcamUnit\\"
#define USB_CAM_UNIT_DEV "USBcamUnit\\Device%d\\"

#define SETTINGS_SECTION "Settings"
#define COLOR	          "Color"

#define DEBUG_SETTINGS	 "Debug"
#define STAND_ALONE		 "StandAlone"
#define OUT_FILE_NAME	 "FileName"

#define DIRECT_SHOW      "DirectShow"
#define SHOW_RENDERER	 "ShowRenderer"

#define CAMERA_N		    "Camera%d"
#define VIDEO_INPUTS 	 "VideoInputs"
#define MOTION_DETECTION "MotionDetection"
#define OUTPUT_FORMAT    "OutputFormat"
#define CAM_ENABLED      "Enabled"


#define MT_YUYV			0
#define MT_UYVY			1
#define MT_YUV9			2
#define MT_YUV411			3
#define MT_YUV211			4
#define MT_RGB1			5
#define MT_RGB4			6
#define MT_RGB8			7
#define MT_RGB565			8
#define MT_RGB555			9
#define MT_RGB24			10
#define MT_RGB32			11
#define MT_ARGB32			12
#define MT_CLJR			13
#define MT_IF09			14
#define MT_CPLA			15
#define MT_MJPG			16
#define MT_TVMJ			17
#define MT_WAKE			18
#define MT_CFCC			19
#define MT_IJPG			20
#define MT_Plum			21
#define MT_Overlay		22
#define MT_QTMovie		23
#define MT_QTRpza			24
#define MT_QTSmc			25
#define MT_QTRle			26
#define MT_QTJpeg			27
#define MT_dvsd			28
#define MT_dvhd			29
#define MT_dvsl			30
#define MT_MPEG1Packet	31
#define MT_MPEG1Payload	32
#define MT_VideoPort		33	
#define MT_VPVideo		34
#define MT_VPVBI			35
#define MT_I400			36
#define MT_UNKNOWN		37

CGuid GUID_I400("30323449-0000-0010-8000-00AA00389B71");

static unsigned int WM_GRAPHNOTIFY = RegisterWindowMessage("GRAPHNOTIFY");

#define RELEASE_OBJECT(pObject)																\
{																										\
	if (pObject) {																					\
		int nR = pObject->Release();															\
		if (nR > 0) TRACE("Release:%d\n%s(%d):\n", nR, __FILE__, __LINE__);		\
		pObject = NULL;																			\
	}																									\
}																

struct ErrorStruct
{
	ErrorStruct(HRESULT hr, int nLine, LPCTSTR sFile) 
	{
		m_hr = hr;
		m_nLine = nLine;
		m_sFile = sFile;
	}
	int         m_nLine;
	HRESULT     m_hr;
	LPCTSTR     m_sFile;
};

struct RGB24
{
	BYTE red, green, blue;
};

struct RGB32
{
	BYTE red, green, blue, alpha;
};

struct RGB555
{
	WORD red  :5;
	WORD green:5;
	WORD blue :5;
	WORD dummy:1;
};

struct RGB565
{
	WORD red  :5;
	WORD green:6;
	WORD blue :5;
};

#define HRESULT_ERROR_AT_POS(hr) new ErrorStruct(hr, __LINE__, __FILE__)

extern CUSBCamUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CUSBCamUnitDlg dialog

CUSBCamUnitDlg::CUSBCamUnitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUSBCamUnitDlg::IDD, pParent)
{
	CString strKey;
	strKey.Format(USB_CAM_UNIT_DEV, theApp.GetInstanceCount()+1);
	strKey = "DVRT\\" + strKey;
	CWK_Profile wkpDev(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, strKey, "");

	strKey = _T(USB_CAM_UNIT);
	strKey = _T("DVRT\\") + strKey;
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, strKey, "");

	//{{AFX_DATA_INIT(CUSBCamUnitDlg)
	m_bColor = FALSE;
	m_bShowRenderer = FALSE;
	m_bHalftone = FALSE;
	m_nSelectedCam = -1;
	//}}AFX_DATA_INIT

	m_hIcon[0] = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon[1] = AfxGetApp()->LoadIcon(IDR_RUNNING);
	m_hIcon[2] = AfxGetApp()->LoadIcon(IDR_RECORDING);
	m_nIconTimer = 0;

	m_pSysDevEnum = NULL;
	m_pGB         = NULL;
	m_pCaptureGB  = NULL;
	
	m_pMC         = NULL;
	m_pME         = NULL;

	m_nCurrSelVideoSource = CB_ERR;
	m_pMD         = NULL;

	int i;
	CString strSection, strGUID;
	strGUID = CGuid((GUID)CLSID_VideoInputDeviceCategory).GetString();
	for (i=0; i<MAX_CAMERAS; i++)
	{
		strSection.Format(CAMERA_N, i);

		m_pCapture[i] = NULL;
		m_pInfTee[i]  = NULL;
		m_pDecode[i]  = NULL;
		m_pEncode[i]  = NULL;
		m_pDump[i]    = NULL;  

		m_pRemovedCapture[i] = NULL;

		m_sCamSources[i].pThis   = this;
		m_sCamSources[i].wSource = i;

		m_strVideoInput[i]    = wkpDev.GetString(strSection, strGUID, NULL);
		m_nOutputFormat[i]    = wkpDev.GetInt(   strSection, OUTPUT_FORMAT, CB_ERR);
		m_bMotionDetection[i] = wkpDev.GetInt(   strSection, MOTION_DETECTION, FALSE);
		m_bEnableCam[i]       = wkpDev.GetInt(   strSection, CAM_ENABLED, TRUE);
	}
	
	m_bStandalone        = wkp.GetInt(   DEBUG_SETTINGS, STAND_ALONE, FALSE);

	m_strOutputFileName  = wkpDev.GetString(SETTINGS_SECTION, OUT_FILE_NAME, NULL);
	m_bColor             = wkpDev.GetInt(   SETTINGS_SECTION, COLOR, m_bColor);
	m_bShowRenderer      = wkpDev.GetInt(   SETTINGS_SECTION, SHOW_RENDERER, m_bShowRenderer);
/*	
#ifdef _DEBUG
	m_bStandalone = FALSE;
#endif
*/
	m_pRGBBuffer = NULL;
	m_p2ndBuffer = NULL;
	m_pVideoWindow = NULL;
	
	m_pInputMDAlarm = NULL;
	m_pInput        = NULL;

	DWORD_PTR ptr;
}

CUSBCamUnitDlg::~CUSBCamUnitDlg()
{
	WK_DELETE(m_pRGBBuffer);
	WK_DELETE(m_p2ndBuffer);
	WK_DELETE(m_pMD);
}

void CUSBCamUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUSBCamUnitDlg)
	DDX_Control(pDX, IDC_COMBO_OUTPUTFORMAT, m_cOutputFormats);
	DDX_Control(pDX, IDC_COMBO_VIDEO_TYPE, m_cMediaType);
	DDX_Control(pDX, IDC_PICTURE, m_cPicture);
	DDX_Control(pDX, IDC_COMBO1, m_cComboVideoInput);
	DDX_Check(pDX, IDC_CK_COLOR, m_bColor);
	DDX_Check(pDX, IDC_CK_LAST_FILTER, m_bShowRenderer);
	DDX_Check(pDX, IDC_CK_HALFTONE, m_bHalftone);
	//}}AFX_DATA_MAP

	if (m_sCamera.IsEmpty())
	{
		pDX->m_bSaveAndValidate = TRUE;
		DDX_Text(pDX, IDC_TXT_CAMERA, m_sCamera);
	}
	if (m_nSelectedCam != CB_ERR)
	{
		DDX_Check(pDX, IDC_CK_ENABLE_CAM, m_bEnableCam[m_nSelectedCam]);
		DDX_Check(pDX, IDC_CK_MD, m_bMotionDetection[m_nSelectedCam]);
		if (!pDX->m_bSaveAndValidate)
		{
			CString str;
			str.Format(m_sCamera, m_nSelectedCam);
			DDX_Text(pDX, IDC_TXT_CAMERA, str);
		}
	}
}

BEGIN_MESSAGE_MAP(CUSBCamUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CUSBCamUnitDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SOURCE_PROPERTY, OnBtnSourceProperty)
	ON_BN_CLICKED(IDC_BTN_ENCODER_PROPERTIES, OnBtnEncoderProperty)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_INIT_VIDEO_CAM, OnInitVideoCam)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_SETTINGS, OnSettings)
	ON_BN_CLICKED(ID_APPLY_NOW, OnApplyNow)
	ON_BN_CLICKED(IDC_BTN_RENDERER_PROPERTIES, OnBtnRendererProperties)
	ON_BN_CLICKED(IDC_CK_COLOR, OnCkColor)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CK_LAST_FILTER, OnCkLastFilter)
	ON_BN_CLICKED(IDC_CK_HALFTONE, OnHalftone)
	ON_BN_CLICKED(IDC_CK_MD, OnCkMd)
	ON_BN_CLICKED(IDC_BTN_MD_PROPERTIES, OnBtnMdProperties)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeVideoSource)
	ON_CBN_SELCHANGE(IDC_COMBO_OUTPUTFORMAT, OnSelchangeComboOutputformat)
	ON_BN_CLICKED(IDC_CK_ENABLE_CAM, OnCkEnableCam)
	ON_BN_CLICKED(IDC_PAUSE, OnPause)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphNotify)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBCamUnitDlg message handlers

BOOL CUSBCamUnitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
/*	
	CString sSMName;
	// die CIPC-Schnittstellen aufbauen.
	sSMName = SM_USBCAM_INPUT;
	if (theApp.GetInstanceCount() != 1)
		sSMName += ('0'+ theApp.GetInstanceCount());
	m_pInput = new CIPCInputUSBcamUnit(this, sSMName);
	if (!m_pInput)
		WK_TRACE_ERROR("CUSBcamUnitDlg::Create\tCIPCInputUSBcamUnit-Objekt konnte nicht angelegt werden.\n");

	sSMName = SM_USBCAM_MD_INPUT;
	if (theApp.GetInstanceCount() != 1)
		sSMName += ('0'+ theApp.GetInstanceCount());
	m_pInputMDAlarm = new CIPCInputUSBcamUnitMDAlarm(this, sSMName);
	if (!m_pInputMDAlarm)
		WK_TRACE_ERROR("CUSBcamUnitDlg::Create\tCIPCInputUSBcamUnitMDAlarm-Objekt konnte nicht angelegt werden.\n");
*/
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon[0], TRUE);			// Set big icon
	SetIcon(m_hIcon[0], FALSE);		// Set small icon
	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		hr = InitFilterGraph();
		if (FAILED(hr)) return FALSE;

		hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, 
		  CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, 
		  (void**)&m_pSysDevEnum);
		if (FAILED(hr))
		{
			ReportError(HRESULT_ERROR_AT_POS(hr), false);
			return FALSE;
		}
	}

	EnumDevices(CLSID_VideoInputDeviceCategory, m_cComboVideoInput, m_strVideoInput[0]);
	
	RELEASE_OBJECT(m_pSysDevEnum);

	SetButtonIcon(IDC_START, IDC_START);
	SetButtonIcon(IDC_PAUSE, IDC_PAUSE);
	SetButtonIcon(IDC_STOP , IDC_STOP );
	SetButtonIcon(IDC_RECORDING, IDC_STOP);
	
	CString str, strCam;
	str.Format(USB_CAM_UNIT_DEV, theApp.GetInstanceCount()+1);
	m_pMD = new CMotionDetection(str);
	m_pMD->Init();
	
	IMoniker     *pMoniker = NULL;
	IPropertyBag *pPropBag = NULL;
	VARIANT var;
	VariantInit(&var);

	int i, nCamID, nOldFormat, nCount = m_cComboVideoInput.GetCount();
	for (i=0; i<nCount; i++)
	{
		pMoniker = (IMoniker*) m_cComboVideoInput.GetItemData(i);
		pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
		
		hr = pPropBag->Read(L"CameraID", &var, 0);	// SZ
		if (SUCCEEDED(hr) && (var.vt == VT_I4))
		{
			nCamID = var.intVal;
		}
		else
		{
			for (nCamID=0; nCamID<MAX_CAMERAS; nCamID++)
			{
				if (m_nOutputFormat[nCamID] == CB_ERR)
					break;
			}
			if (nCamID == MAX_CAMERAS)
			{
				continue;
			}
		}
		VariantClear(&var);

		nOldFormat = m_nOutputFormat[nCamID];
		if (m_bEnableCam[nCamID] && !m_bStandalone)
		{
			m_nCurrSelVideoSource = -1;
			m_nSelectedCam = nCamID;
			m_cComboVideoInput.SetCurSel(CB_ERR);
			OnSelchangeVideoSource();
			m_nSelectedCam = CB_ERR;
			if ((m_nOutputFormat[nCamID] == CB_ERR) && (nOldFormat == CB_ERR))
			{
				m_bEnableCam[nCamID] = FALSE;
			}
		}
		if (m_nOutputFormat[nCamID] != CB_ERR)
		{
			if (!m_bStandalone)
			{
				InitVideoCamEx(i);
			}
			var.vt     = VT_I4;
			var.intVal = nCamID;
			pPropBag->Write(L"CameraID", &var);
		}
		pPropBag->Release();
	}
	
	if (m_bStandalone)
	{
		OnSelchangeVideoSource();
	}
	else
	{
		OnStart();
	}
//	ComboResetContent(m_cComboVideoInput);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUSBCamUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUSBCamUnitDlg::OnPaint() 
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUSBCamUnitDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CUSBCamUnitDlg::ReleaseFilters(WORD wSource)
{
	if ((m_pGB) && (wSource != 0xffff))
	{
		if (m_pCapture[wSource])  m_pGB->RemoveFilter(m_pCapture[wSource]);
		if (m_pDecode[wSource])   m_pGB->RemoveFilter(m_pDecode[wSource]);
		if (m_pEncode[wSource])   m_pGB->RemoveFilter(m_pEncode[wSource]);
		if (m_pInfTee[wSource])   m_pGB->RemoveFilter(m_pInfTee[wSource]);
		if (m_pDump[wSource])     m_pGB->RemoveFilter(m_pDump[wSource]);
	}

	if (wSource != 0xffff)
	{
		RELEASE_OBJECT(m_pCapture[wSource]);
		RELEASE_OBJECT(m_pInfTee[wSource]);
		RELEASE_OBJECT(m_pDecode[wSource]);
		RELEASE_OBJECT(m_pEncode[wSource]);
		RELEASE_OBJECT(m_pDump[wSource]);
	}
	else
	{
		if (m_pME)
		{
			m_pME->SetNotifyWindow(NULL, 0, 0);
		}

		if (m_pVideoWindow)
		{
			m_pVideoWindow->put_Owner((OAHWND)NULL);
			m_pVideoWindow->put_MessageDrain((OAHWND)NULL);
		}
		RELEASE_OBJECT(m_pVideoWindow);
		RELEASE_OBJECT(m_pMC);
		RELEASE_OBJECT(m_pME);

		RELEASE_OBJECT(m_pGB);
		RELEASE_OBJECT(m_pCaptureGB);
	}
}

void CUSBCamUnitDlg::ReleaseAllFilters()
{
	for (int i=0; i<MAX_CAMERAS; i++)
	{
		if (m_pCapture[i])
		{
			ReleaseFilters(i);
		}
		else if (m_pRemovedCapture[i])
		{
			m_pGB->RemoveFilter(m_pRemovedCapture[i]);
			RELEASE_OBJECT(m_pRemovedCapture[i]);
		}
	}

	ReleaseFilters(0xffff);
}

void CUSBCamUnitDlg::EnumDevices(REFCLSID rclsid, CComboBox &cCombo, CString &strCombo)
{
	HRESULT hr;
	IEnumMoniker *pEnum    = NULL;
	IMoniker     *pMoniker = NULL;

	if (m_pSysDevEnum)
	{
		hr = m_pSysDevEnum->CreateClassEnumerator(rclsid, &pEnum, 0);

		if (SUCCEEDED(hr) && pEnum)
		{
			while (S_OK == pEnum->Next(1, &pMoniker, NULL))
			{
				IPropertyBag *pPropBag = NULL;
				LRESULT       iSel     = CB_ERR;
				pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
				VARIANT var;
				VariantInit(&var);
				hr = pPropBag->Read(L"FriendlyName", &var, 0);	// SZ
				if (SUCCEEDED(hr))
				{
					if (var.vt == VT_BSTR)
					{
						CString str;
						int nLen = wcslen(var.bstrVal);
						WideCharToMultiByte(CP_ACP,0, var.bstrVal, nLen, str.GetBufferSetLength(nLen+1), nLen, NULL, NULL);
						str.SetAt(nLen, 0);
						iSel = cCombo.AddString(str);
					}
				}   
				VariantClear(&var); 
				RELEASE_OBJECT(pPropBag);
				if (iSel != CB_ERR)
				{
					cCombo.SetItemData(iSel, (DWORD)pMoniker);
				}
				else
				{
				  RELEASE_OBJECT(pMoniker);
				}
			}
		}
	}
	
	if (strCombo.IsEmpty())
	{
		cCombo.SetCurSel(0);
	}
	else
	{
		int nFind = cCombo.FindString(0, strCombo);
		if (nFind != CB_ERR)
		{
			cCombo.SetCurSel(nFind);
		}
	}
	
	RELEASE_OBJECT(pEnum);
}

void CUSBCamUnitDlg::ComboResetContent(CComboBox &cCombo)
{
	IUnknown *pMoniker = NULL;
	int i, nCount = cCombo.GetCount();
	for (i=0; i< nCount; i++)
	{
		pMoniker = (IUnknown*) cCombo.GetItemData(i);
		RELEASE_OBJECT(pMoniker);
	}
   cCombo.ResetContent();
}

void CUSBCamUnitDlg::ReportError(ErrorStruct*ps, bool bMsgBox)
{
	ASSERT(ps != NULL);
	if (FAILED(ps->m_hr))
	{
		CString str;
		LPCTSTR pstrError = DXGetErrorString8(ps->m_hr);
		CString strDescr = _T(DXGetErrorDescription8(ps->m_hr));
		strDescr.Replace("&", "\n");
		str.Format("Error initialising Filters\n%s(%d) :\nError: %s (%x):\n %s\n", ps->m_sFile, ps->m_nLine, pstrError, ps->m_hr, strDescr);
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
	delete ps;
}

int CUSBCamUnitDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

		CString sTip = _T("USBCamUnit");//COemGuiApi::GetApplicationName(theApp.GetApplicationId());

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

void CUSBCamUnitDlg::OnDestroy() 
{
	OnStop();
	ReleaseAllFilters();

	ComboResetContent(m_cComboVideoInput);
	RELEASE_OBJECT(m_pSysDevEnum);

	CoUninitialize();

	CDialog::OnDestroy();
	
	if (!m_bStandalone)
	{
		NOTIFYICONDATA tnd;

		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID			= 1;
		tnd.uFlags		= 0;

		Shell_NotifyIcon(NIM_DELETE, &tnd);
	}
	PostQuitMessage(0);
}

void CUSBCamUnitDlg::OnCancel() 
{
	if	(m_bStandalone)
	{
		PostMessage(WM_DESTROY);
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
	
	CDialog::OnCancel();
}

void CUSBCamUnitDlg::OnOK() 
{
	
	CDialog::OnOK();
}

LRESULT CUSBCamUnitDlg::OnTrayClicked(WPARAM wParam, LPARAM lParam)
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
			pM = menu.GetSubMenu(0);
			
			nEnable |= (GetDlgItem(IDC_START)->IsWindowEnabled() != 0) ? MF_ENABLED : MF_GRAYED;
			pM->EnableMenuItem(IDC_START, nEnable);

			nEnable = MF_BYCOMMAND;
			nEnable |= (GetDlgItem(IDC_STOP)->IsWindowEnabled() != 0) ? MF_ENABLED : MF_GRAYED;
			pM->EnableMenuItem(IDC_STOP, nEnable);

			nEnable = MF_BYCOMMAND;
			nEnable |= (GetDlgItem(IDC_BTN_SOURCE_PROPERTY)->IsWindowEnabled() != 0) ? MF_ENABLED : MF_GRAYED;
			pM->EnableMenuItem(IDC_BTN_SOURCE_PROPERTY, nEnable);

			nEnable = MF_BYCOMMAND;
			nEnable |= (GetDlgItem(IDC_BTN_ENCODER_PROPERTY)->IsWindowEnabled() != 0) ? MF_ENABLED : MF_GRAYED;
			pM->EnableMenuItem(IDC_BTN_ENCODER_PROPERTY, nEnable);
			
			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}

void CUSBCamUnitDlg::OnStart() 
{
	if (m_pMC)
	{
		HRESULT hr;
      hr = m_pMC->Run();
		if (FAILED(hr))
		{
			ReportError(HRESULT_ERROR_AT_POS(hr));
		}
		else
		{
			ControlStartStopBtn(true);
			SetUSBCamIcon(ICON_RUNNING);
		}

	}
}

void CUSBCamUnitDlg::OnStop() 
{
	if (m_pMC)
	{
		HRESULT hr;
		hr = m_pMC->StopWhenReady();
		if (FAILED(hr))
		{
			ReportError(HRESULT_ERROR_AT_POS(hr));
		}
		else
		{
			ControlStartStopBtn(false);
			SetUSBCamIcon(ICON_MAIN);
		}
	}
}

void CUSBCamUnitDlg::OnPause() 
{
	if (m_pMC)
	{
		HRESULT hr;
		hr = m_pMC->Pause();
		if (FAILED(hr))
		{
			ReportError(HRESULT_ERROR_AT_POS(hr));
		}
		else
		{
			ControlStartStopBtn(false);
			SetUSBCamIcon(ICON_MAIN);
		}
	}
}


void CUSBCamUnitDlg::OnInitVideoCam() 
{
	InitVideoCam(m_nSelectedCam);
}

void CUSBCamUnitDlg::InitVideoCam(WORD wSource) 
{
	HRESULT          hr        = NOERROR;
	IMoniker        *pMoniker  = NULL;
	IPin            *pPinOut   = NULL;
	IPin            *pPinIn    = NULL;
	IFileSinkFilter *pFileSink = NULL;
	CGuid            guid;
	IEnumFilters    *pEnum     = NULL;
	IBaseFilter     *pFilter   = NULL;
	IBaseFilter     *pRender   = NULL;
	FILTER_INFO     fi = {NULL, NULL};
	
	UpdateData();
	try
	{	
		if (m_pCapture[wSource] == NULL)
		{
			throw HRESULT_ERROR_AT_POS(VFW_E_NO_INTERFACE);
		}
		 CComPtr<IBaseFilter>        ppf;
		 CComPtr<IFileSinkFilter>    pSink;     
		 hr = m_pCaptureGB->SetOutputFileName(&MEDIASUBTYPE_Avi, L"test.avi", &ppf, &pSink);


		hr = GetPin(m_pCapture[wSource], PINDIR_OUTPUT, 0, &pPinOut);
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);

		IAMStreamConfig *pCfg      = NULL;
		hr = pPinOut->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
		AM_MEDIA_TYPE *pmt = (AM_MEDIA_TYPE*)m_cOutputFormats.GetItemData(m_cOutputFormats.GetCurSel());
		if (SUCCEEDED(hr))
		{
			if (pmt && (0xffffffff != (UINT)pmt))
			{
				hr = pCfg->SetFormat(pmt);
			}
			RELEASE_OBJECT(pCfg);
		}

		hr = m_pCaptureGB->RenderStream(&PIN_CATEGORY_PREVIEW,  // Pin category
												  &MEDIATYPE_Video,       // Media type
												  m_pCapture[wSource],    // Capture filter
												  NULL,                   // Compression filter (optional)
                                      NULL                     // Multiplexer or renderer filter
			);

//		hr = m_pCaptureGB->RenderStream(, , pPinOut, NULL, NULL);
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);

//		hr = m_pGB->Render(pPinOut);												// render the Capture device
//		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		RELEASE_OBJECT(pPinOut);

		CString strFilters;
		int nFilter = 0;
		hr = m_pGB->EnumFilters(&pEnum);									// get the created filters
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		while (SUCCEEDED(pEnum->Next(1, &pFilter, NULL)) && (pFilter != NULL))
		{
			hr = pFilter->QueryFilterInfo(&fi);
			if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
			CString str(fi.achName);
			if (nFilter == 0)													// Last Filter is the render device
			{
				GetDlgItem(IDC_CK_LAST_FILTER)->SetWindowText(str);
				strFilters = str;
				pRender = pFilter;
				GetDlgItem(IDC_BTN_RENDERER_PROPERTIES)->EnableWindow((ShowFilterPropertyPage(pRender, false)== NOERROR) ? true: false);
				pFilter = NULL;
			}
			else if (nFilter == 1)										// 2nd last is the decodig device
			{
				GetDlgItem(IDC_TXT_FILTER)->SetWindowText(str);
				strFilters = str + ", " + strFilters;
				if (m_pCapture[wSource] != pFilter)
				{
					m_pDecode[wSource]  = pFilter;
					GetDlgItem(IDC_BTN_ENCODER_PROPERTY)->EnableWindow((ShowFilterPropertyPage(m_pDecode[wSource], false)== NOERROR) ? true: false);
					pFilter    = NULL;
				}
			}
			else																// enum until capture device
			{
				strFilters = str + ", " + strFilters;
			}
			RELEASE_OBJECT(fi.pGraph);
			RELEASE_OBJECT(pFilter);
			nFilter++;
		}
		RELEASE_OBJECT(pEnum);
		CWnd *pWnd = GetDlgItem(IDC_PICTURE);

		if (pWnd)
		{
			CRect rc;
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
			m_pGB->QueryInterface(IID_IVideoWindow, (void**)&m_pVideoWindow);
			if (m_pVideoWindow)
			{
				m_pVideoWindow->put_Owner((OAHWND)m_hWnd);
				m_pVideoWindow->put_WindowStyle(WS_CHILD|WS_CLIPSIBLINGS);
				m_pVideoWindow->put_MessageDrain((OAHWND)NULL);
				m_pVideoWindow->put_Width(rc.Width());
				m_pVideoWindow->put_Height(rc.Height());
				m_pVideoWindow->put_Left(rc.left);
				m_pVideoWindow->put_Top(rc.top);
			}
		}

		hr = GetPin(m_pDecode[wSource], PINDIR_OUTPUT, 0, &pPinOut);// get decoder device out pin
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		AM_MEDIA_TYPE sMT = {0};
		if (GetVideoSubtype(pPinOut, sMT))							// test the video subtype
		{
			m_nMediaType = m_cMediaType.GetCurSel();
		}
		else
		{
//			throw HRESULT_ERROR_AT_POS(VFW_E_INVALIDSUBTYPE);
		}
		FreeMediaType(sMT);


		hr = m_pGB->Disconnect(pPinOut);								// disconnect the pin
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		RELEASE_OBJECT(pPinOut);

		hr = GetPin(pRender, PINDIR_INPUT, 0, &pPinIn);// get render device in pin
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		hr = m_pGB->Disconnect(pPinIn);								// disconnect the pin
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		RELEASE_OBJECT(pPinIn);
		
		hr = AddFilterToGraph(m_pGB, &m_pInfTee[wSource], CLSID_INF_TEE, L"Infinite Tee");
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);			// Inifnite Tee Filter

		hr = GetPin(m_pInfTee[wSource], PINDIR_INPUT , 0, &pPinIn);
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		if (pPinIn)
		{
			guid.MakeGUID(GUID_IINF_TEE);								// Query IInfTee interface
			IInfTee *pInfTee = NULL;
			hr = pPinIn->QueryInterface(guid, (void**)&pInfTee);
			if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);		// set the CheckData() function
			hr = pInfTee->SetDataCheckFunction(0, CheckData, (long) &m_sCamSources[wSource]);
			RELEASE_OBJECT(pInfTee);
		}
		RELEASE_OBJECT(pPinIn);

		hr = AddFilterToGraph(m_pGB, &m_pDump[wSource], CLSID_DUMPFILTER, L"Sample Dump");
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);			// get the dump filter
		if (!m_strOutputFileName.IsEmpty())
		{
			m_pDump[wSource]->QueryInterface(IID_IFileSinkFilter, (void **)&pFileSink);
			if (pFileSink)															// Query IFileSinkFilter interface
			{
				WCHAR wstrFileName[MAX_PATH];
				int i, nLen = m_strOutputFileName.GetLength();
				for (i=0; i<nLen; i++)
				{
					wstrFileName[i] = (WCHAR)m_strOutputFileName.GetAt(i);
				}
				wstrFileName[i] = 0;
				pFileSink->SetFileName(wstrFileName, NULL);			// Set filename
			}
		}

		hr = GetPin(m_pDump[wSource], PINDIR_INPUT , 0, &pPinIn);
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		guid.MakeGUID(GUID_IDUMP);										// Query IDump interface
		IDump *pDump = NULL;
		hr = pPinIn->QueryInterface(guid, (void **)&pDump);
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		if (m_strOutputFileName.IsEmpty())
		{																			// set the function to receive IMediaSample
			hr = pDump->SetReceiveFunction(ReceiveMediaSample, (long)&m_sCamSources[wSource]);
		}
		RELEASE_OBJECT(pDump);
		RELEASE_OBJECT(pPinIn);

		hr = ConnectFilters(m_pGB, m_pDecode[wSource], 0, m_pInfTee[wSource], 0);	// connect Decoder to InfiniteTee
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);

		hr = ConnectFilters(m_pGB, m_pInfTee[wSource], 0, m_pDump[wSource], 0);		// connect InfiniteTee to Dump
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);

		if (!m_bStandalone && !IsWindowVisible())
		{
			if (!m_bShowRenderer)
			{
				m_pGB->RemoveFilter(pRender);
				RELEASE_OBJECT(pRender);
			}
		}
		else if (m_bShowRenderer)
		{
			hr = ConnectFilters(m_pGB, m_pInfTee[wSource], 1, pRender, 0);
			if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		}
*/
	}
   catch(ErrorStruct *ps)
	{
		ReportError(ps);
		ReleaseFilters(wSource);
	}

	RELEASE_OBJECT(pFileSink);
	RELEASE_OBJECT(pPinOut);
	RELEASE_OBJECT(pPinIn);
	RELEASE_OBJECT(fi.pGraph);
	RELEASE_OBJECT(pEnum);
	RELEASE_OBJECT(pRender);
	
	GetDlgItem(IDC_INIT_VIDEO_CAM)->EnableWindow(hr != NOERROR);
	m_cOutputFormats.EnableWindow(hr != NOERROR);
}

void CUSBCamUnitDlg::InitVideoCamEx(WORD wSource) 
{
	HRESULT          hr        = NOERROR;
	IMoniker        *pMoniker  = NULL;
	IPin            *pPinOut   = NULL;
	IPin            *pPinIn    = NULL;
	IFileSinkFilter *pFileSink = NULL;
	CGuid            guid;
	IEnumFilters    *pEnum     = NULL;
	IBaseFilter     *pFilter   = NULL;
	FILTER_INFO     fi = {NULL, NULL};
	
	UpdateData();
	try
	{	
		m_nMediaType = CB_ERR;
		AM_MEDIA_TYPE *pmt = (AM_MEDIA_TYPE*)m_cOutputFormats.GetItemData(m_nOutputFormat[wSource]);
		if (pmt == (AM_MEDIA_TYPE*)CB_ERR)
		{
			throw HRESULT_ERROR_AT_POS(VFW_E_INVALIDSUBTYPE);
		}
		if (m_pCapture[wSource] == NULL)
		{
			// Create the capture device
			pMoniker = (IMoniker*)m_cComboVideoInput.GetItemData(m_cComboVideoInput.GetCurSel());
			if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
			hr = AddFilterToGraph(m_pGB, &m_pCapture[wSource], pMoniker, L"Capture Device");
			pMoniker = NULL;
			if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		}
		
		hr = GetPin(m_pCapture[wSource], PINDIR_OUTPUT, 0, &pPinOut);
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);

		IAMStreamConfig *pCfg      = NULL;
		hr = pPinOut->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
		if (SUCCEEDED(hr))
		{
			hr = pCfg->SetFormat(pmt);
			if (FAILED(hr))
			{
				pCfg->GetFormat(&pmt);
			}
			GetVideoSubtype(NULL, *pmt);
			RELEASE_OBJECT(pCfg);
		}

		int nSourceMediaType = m_cMediaType.GetCurSel();
		if (nSourceMediaType != MT_YUYV)
		{
			DWORD dwMerit = MERIT_NORMAL;
			switch (nSourceMediaType)
			{
				case MT_RGB555: case MT_RGB565:					// rgb formats can be rendered
				case MT_RGB24:  case MT_RGB32:case MT_ARGB32:
				{
					hr = AddFilterToGraph(m_pGB, &m_pDecode[wSource], "D76E2820-1563-11CF-AC98-00AA004C0FA9", L"YUV");
					if (FAILED(hr))
					{
						dwMerit = MERIT_DO_NOT_USE;					// with lower Merit
					}
					else
					{
						dwMerit = 0;
					}
				} break;
				default:
				{
				}break;
			}
			if (dwMerit)
			{
				GUID subtypes[2];
				subtypes[0] = MEDIASUBTYPE_YVYU;						// desired format is YUYV
				subtypes[1] = MEDIASUBTYPE_YUY2;
				hr = FindMatchingFilter(dwMerit, pPinOut, TRUE, pmt->majortype, pmt->subtype, TRUE, pmt->majortype, subtypes, 2, FALSE, &m_pDecode[wSource], &pmt);
			}
			RELEASE_OBJECT(pPinOut);

			if (pmt) GetVideoSubtype(NULL, *pmt);
			m_nMediaType = m_cMediaType.GetCurSel();
			if (m_nMediaType != MT_YUYV)							// could not be rendered ?
			{
				if (pmt)
				{
					DeleteMediaType(pmt);
					pmt = NULL;
				}
			}
			if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		}		

		hr = AddFilterToGraph(m_pGB, &m_pInfTee[wSource], CLSID_INF_TEE, L"Infinite Tee");
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);			// Inifnite Tee Filter

		hr = GetPin(m_pInfTee[wSource], PINDIR_INPUT , 0, &pPinIn);
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		if (pPinIn)
		{
			guid.MakeGUID(GUID_IINF_TEE);									// Query IInfTee interface
			IInfTee *pInfTee = NULL;
			hr = pPinIn->QueryInterface(guid, (void**)&pInfTee);
			if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);		// set the CheckData() function
			hr = pInfTee->SetDataCheckFunction(0, CheckData, (long) &m_sCamSources[wSource]);
			RELEASE_OBJECT(pInfTee);
		}
		RELEASE_OBJECT(pPinIn);

		hr = AddFilterToGraph(m_pGB, &m_pDump[wSource], CLSID_DUMPFILTER, L"Sample Dump");
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);			// get the dump filter
		if (!m_strOutputFileName.IsEmpty())
		{
			m_pDump[wSource]->QueryInterface(IID_IFileSinkFilter, (void **)&pFileSink);
			if (pFileSink)															// Query IFileSinkFilter interface
			{
				WCHAR wstrFileName[MAX_PATH];
				int i, nLen = m_strOutputFileName.GetLength();
				for (i=0; i<nLen; i++)
				{
					wstrFileName[i] = (WCHAR)m_strOutputFileName.GetAt(i);
				}
				wstrFileName[i] = 0;
				pFileSink->SetFileName(wstrFileName, NULL);			// Set filename
			}
		}

		hr = GetPin(m_pDump[wSource], PINDIR_INPUT , 0, &pPinIn);
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		guid.MakeGUID(GUID_IDUMP);											// Query IDump interface
		IDump *pDump = NULL;
		hr = pPinIn->QueryInterface(guid, (void **)&pDump);
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		if (m_strOutputFileName.IsEmpty())
		{																			// set the function to receive IMediaSample
			hr = pDump->SetReceiveFunction(ReceiveMediaSample, (long)&m_sCamSources[wSource]);
		}
		RELEASE_OBJECT(pDump);
		RELEASE_OBJECT(pPinIn);

		if (m_pDecode[wSource])
		{
			hr = ConnectFilters(m_pGB, m_pDecode[wSource], 0, m_pInfTee[wSource], 0, pmt);
			DeleteMediaType(pmt);												// connect Decoder to InfiniteTee with specific MediaType
			if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
		}
		else
		{
			hr = ConnectFilters(m_pGB, m_pCapture[wSource], 0, m_pInfTee[wSource], 0, pmt);
			if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);			// connect InfiniteTee to Dump
		}
		hr = ConnectFilters(m_pGB, m_pInfTee[wSource], 0, m_pDump[wSource], 0);
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);			// connect InfiniteTee to Dump
	}
   catch(ErrorStruct *ps)
	{
		ReportError(ps);
		ReleaseFilters(wSource);
	}

	RELEASE_OBJECT(pFileSink);
	RELEASE_OBJECT(pPinOut);
	RELEASE_OBJECT(pPinIn);
	RELEASE_OBJECT(fi.pGraph);
	RELEASE_OBJECT(pEnum);
	
	GetDlgItem(IDC_INIT_VIDEO_CAM)->EnableWindow(hr != NOERROR);
	m_cOutputFormats.EnableWindow(hr != NOERROR);
}

HRESULT CUSBCamUnitDlg::InitFilterGraph()
{
	HRESULT hr = NOERROR;
	try
	{
		// Create the capture graph builder.
		hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,IID_ICaptureGraphBuilder2, (void **)&m_pCaptureGB);
		if (!m_pCaptureGB)throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);

		// Create the  graph builder.
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGB);
		if (!m_pGB)			throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
		
		hr = m_pCaptureGB->SetFiltergraph(m_pGB);						// Associate the filter graph with the capture graph builder
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);

		// Query the Media Control Interface
		hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
		if (!m_pMC)			throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);

		// Query the Media Event Interface
		hr = m_pGB->QueryInterface(IID_IMediaEventEx, (void **)&m_pME);
		if (!m_pME)			throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
		hr = m_pME->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPHNOTIFY, 0);
		if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
	}
	catch(ErrorStruct *ps)
	{
		ReportError(ps);
		ReleaseFilters(0xffff);
	}
	return hr;
}

void CUSBCamUnitDlg::SetButtonIcon(int nBtnID, int nIconID)
{
   SendDlgItemMessage(nBtnID, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(nIconID), IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
	GetDlgItem(nBtnID)->UpdateWindow();
}

void CUSBCamUnitDlg::ControlStartStopBtn(bool bStarted)
{
	GetDlgItem(IDC_START)->EnableWindow(!bStarted);
	GetDlgItem(IDC_STOP)->EnableWindow(bStarted);
	GetDlgItem(IDC_PAUSE)->EnableWindow(bStarted);
}

void CUSBCamUnitDlg::SetUSBCamIcon(WORD wColor, BOOL bType)
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

			CString sTip = _T("UsbCamUnit");//COemGuiApi::GetApplicationName(theApp.GetApplicationId());

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

LRESULT CUSBCamUnitDlg::OnGraphNotify(WPARAM wParam,  LPARAM lParam)
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
				case EC_DEVICE_LOST:
				{
					IUnknown    *pUnk = (IUnknown*)evParam1;
					IBaseFilter *pFilter = NULL;						// Search for BaseFilter
					pUnk->QueryInterface(IID_IBaseFilter, (void**)&pFilter);
					for (int i=0; i<MAX_CAMERAS; i++)
					{
						if (   (m_pCapture[i]        == pFilter)
							 || (m_pRemovedCapture[i] == pFilter))
						{
							break;
						}
					}
					RELEASE_OBJECT(pFilter);
					if (i<MAX_CAMERAS)
					{
						if (GetDlgItem(IDC_STOP)->IsWindowEnabled())
						{
							OnStop();
						}
						if (evParam2 == 0)									// Device removed
						{
							m_pRemovedCapture[i] = m_pCapture[i];
							m_pCapture[i] = NULL;
							ReleaseFilters(i);
						}
						else														// Device plugged
						{
							m_pCapture[i] = m_pRemovedCapture[i];
							m_pRemovedCapture[i] = NULL;
							RELEASE_OBJECT(m_pCapture[i]);
							InitVideoCamEx(i);
						}
						for (int i=0; i<MAX_CAMERAS; i++)
						{
							if (m_pCapture[i])
							{
								break;
							}
						}
						if (i<MAX_CAMERAS)
						{
							OnStart();
						}
					}
				}	break;
				case EC_PAUSED:
				{
//					OnStop();
//					OnStart();
				}break;
				case EC_RECEIVE_MEDIA_SAMPLE:
				{
					IMediaSample *pSample    = (IMediaSample*)evParam1;
					long          nPackageNo = (long)evParam2;
//					ReceiveMediaSample(pSample, nPackageNo, (long)this);
					pSample->Release();
				} continue;	// User define Event: do not invoke FreeEventParams(..)!
				default:
					str.Format("Event(%d, %x), %x, %x\r\n", evCode, evCode, evParam1, evParam2);
					WK_TRACE(str);
					break;
			}
			// Free event parameters to prevent memory leaks
			hr = m_pME->FreeEventParams(evCode, evParam1, evParam2);
		}
		
	}
	return 0;
}

void CUSBCamUnitDlg::OnApplyNow() 
{
	CString strKey;
	strKey.Format(USB_CAM_UNIT_DEV, theApp.GetInstanceCount()+1);
	strKey = "DVRT\\" + strKey;
	CWK_Profile wkpDev(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, strKey, "");

	strKey = _T(USB_CAM_UNIT);
	strKey = _T("DVRT\\") + strKey;
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, strKey, "");

	int i;
	CString strSection, strGUID;
	strGUID = CGuid((GUID)CLSID_VideoInputDeviceCategory).GetString();
	for (i=0; i<MAX_CAMERAS; i++)
	{
		strSection.Format(CAMERA_N, i);
		wkpDev.WriteString(strSection, strGUID, m_strVideoInput[i]);
		wkpDev.WriteInt(   strSection, OUTPUT_FORMAT, m_nOutputFormat[i]);
		wkpDev.WriteInt(   strSection, MOTION_DETECTION, m_bMotionDetection[i]);
		wkpDev.WriteInt(   strSection, CAM_ENABLED, m_bEnableCam[i]);
	}
	wkpDev.WriteString(SETTINGS_SECTION, OUT_FILE_NAME, m_strOutputFileName);
	wkpDev.WriteInt(   SETTINGS_SECTION, COLOR, m_bColor);
	wkpDev.WriteInt(   SETTINGS_SECTION, SHOW_RENDERER, m_bShowRenderer);
	
	wkp.WriteInt(   DEBUG_SETTINGS, STAND_ALONE, m_bStandalone);
}

long CUSBCamUnitDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, (WPARAM)theApp.GetApplicationId(), 0);
	}

	return 0;
}

void CUSBCamUnitDlg::OnAppExit() 
{
	DestroyWindow();
}

void CUSBCamUnitDlg::OnSettings() 
{
	if (!m_bStandalone)
	{
		ShowWindow(SW_SHOW);
	}
}

void CUSBCamUnitDlg::OnAbout() 
{
	COemGuiApi::AboutDialog(this);
}

HRESULT WINAPI CUSBCamUnitDlg::ReceiveMediaSample(IMediaSample *pSample, long nPackageNo, long lParam)
{
	SourceCtrl  *pCtrl = (SourceCtrl*)lParam;
	pCtrl->pThis->m_mdPoints[pCtrl->wSource].Reset();
	return NOERROR;
}

HRESULT WINAPI CUSBCamUnitDlg::CheckData(IMediaSample*pSample, long lParam)
{
	SourceCtrl  *pCtrl = (SourceCtrl*)lParam;
	return pCtrl->pThis->CheckData(pSample, pCtrl->wSource);
}

HRESULT CUSBCamUnitDlg::CheckData(IMediaSample *pSample, WORD wSource)
{
	AM_MEDIA_TYPE   *pMT   = NULL;
	pSample->GetMediaType(&pMT);
	VIDEOINFOHEADER *pVIH  = (VIDEOINFOHEADER*)pMT->pbFormat;
	BYTE            *pData = NULL;;
	int              nLen  = pSample->GetActualDataLength();
	HRESULT          hr    = E_FAIL;
	pSample->GetPointer(&pData);

	if (m_bMotionDetection[wSource])
	{
		CAPTURE_STRUCT   Capture = {0};
		Capture.wSource = wSource;
		Capture.wCompressionType = COMPRESSION_YUV_422;
		Capture.Buffer.dwLen     = nLen;
		Capture.Buffer.pLinAddr  = pData;
		Capture.wSizeH           = (pVIH->bmiHeader.biWidth);
		Capture.wSizeV           = (pVIH->bmiHeader.biHeight);
		BOOL bMD = m_pMD->MotionCheck(Capture, m_mdPoints[Capture.wSource]);
		if (bMD == MD_POINTS_RECEIVED)
		{
			if (!m_bStandalone)
			{
				if (m_pInputMDAlarm)
					m_pInputMDAlarm->OnReceivedMotionAlarm(wSource, m_mdPoints[Capture.wSource], TRUE);
 				if (m_pInputMDAlarm)
					m_pInputMDAlarm->OnReceivedMotionAlarm(wSource, m_mdPoints[Capture.wSource], FALSE);
			}
			if (m_nIconTimer == 0)
			{
				SetUSBCamIcon(ICON_RECORDING);
				SetButtonIcon(IDC_RECORDING, IDC_RECORDING);
				m_nIconTimer = SetTimer(ICON_RUNNING, 200, NULL);
			}
			hr = NOERROR;
		}
	}
	else
	{
		hr = NOERROR;
	}
	DeleteMediaType(pMT);

	return hr;
}


HRESULT CUSBCamUnitDlg::ConnectFilters(IGraphBuilder* pGB, IBaseFilter *pF1, int nPinF1, IBaseFilter *pF2, int nPinF2, AM_MEDIA_TYPE *pMT)
{
	HRESULT hr;
	IPin *pPinIn  = NULL, 
		  *pPinOut = NULL;

	if (pGB == NULL) return E_POINTER;

	hr = GetPin(pF1, PINDIR_OUTPUT, nPinF1, &pPinOut);
	if (SUCCEEDED(hr))
	{
		hr = GetPin(pF2, PINDIR_INPUT , nPinF2, &pPinIn);
		if (SUCCEEDED(hr))
		{
			if (pPinOut && pPinIn)
			{
				if (pMT)
				{
					hr = pPinOut->Connect(pPinIn, pMT);
				}
				else
				{
					hr = pGB->Connect(pPinOut, pPinIn);						// Connect the splitter device to the encoder device
				}
			}
		}
	}
	RELEASE_OBJECT(pPinOut);
	RELEASE_OBJECT(pPinIn);
	return hr;
}

HRESULT CUSBCamUnitDlg::GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
{
    CComPtr< IEnumPins > pEnum;
    *ppPin = NULL;
	 if (pFilter == NULL) return E_POINTER;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) return hr;

    ULONG ulFound;
    IPin *pPin;
    hr = E_FAIL;

    while(S_OK == pEnum->Next(1, &pPin, &ulFound))
    {
        PIN_DIRECTION pindir = (PIN_DIRECTION)3;
        pPin->QueryDirection(&pindir);
        if(pindir == dirrequired)
        {
            if(iNum == 0)
            {
                *ppPin = pPin;
                // Found requested pin, so clear error
                hr = S_OK;
                break;
            }
            iNum--;
        } 

        pPin->Release();
    } 

    return hr;
}

HRESULT CUSBCamUnitDlg::ShowFilterPropertyPage(IBaseFilter *pFilter, bool bShow)
{
   HRESULT hr;
   ISpecifyPropertyPages *pSpecify=0;

   if (!pFilter) return E_NOINTERFACE;
	
   // Discover if this filter contains a property page
   hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpecify);
   if (SUCCEEDED(hr)) 
   {
      do 
      {
         FILTER_INFO FilterInfo;
         hr = pFilter->QueryFilterInfo(&FilterInfo);
         if (FAILED(hr))
            break;

			CAUUID caGUID;
			hr = pSpecify->GetPages(&caGUID);
			if (FAILED(hr))
				 break;

			pSpecify->Release();
			if (bShow)
			{
				OleCreatePropertyFrame(
					 m_hWnd,             // Parent window
					 0,                      // x (Reserved)
					 0,                      // y (Reserved)
					 FilterInfo.achName,     // Caption for the dialog box
					 1,                      // Number of filters
					 (IUnknown **)&pFilter,  // Pointer to the filter 
					 caGUID.cElems,          // Number of property pages
					 caGUID.pElems,          // Pointer to property page CLSIDs
					 0,                      // Locale identifier
					 0,                      // Reserved
					 NULL                    // Reserved
				);
			}
			CoTaskMemFree(caGUID.pElems);
			FilterInfo.pGraph->Release(); 

      } while(0);
   }

   return hr;
}

HRESULT CUSBCamUnitDlg::AddFilterToGraph(IGraphBuilder* pGB, IBaseFilter **ppFilter, LPCTSTR strGUID, LPCWSTR strFilterName)
{
	CGuid guid(strGUID);
	return AddFilterToGraph(pGB, ppFilter, guid, strFilterName);
}

HRESULT CUSBCamUnitDlg::AddFilterToGraph(IGraphBuilder* pGB, IBaseFilter **ppFilter, CGuid &guid, LPCWSTR strFilterName)
{
	if (!pGB)      return E_POINTER;
	if (!ppFilter) return E_POINTER;
	ASSERT(*ppFilter == NULL);

	FILTER_INFO fi;
	HRESULT hr = CoCreateInstance(guid, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)ppFilter);
	if (!*ppFilter) return (hr);
	if (strFilterName == NULL)
	{
		(*ppFilter)->QueryFilterInfo(&fi);
		RELEASE_OBJECT(fi.pGraph);
		strFilterName = fi.achName;
	}
	hr = pGB->AddFilter(*ppFilter, strFilterName);
	return hr;
}

HRESULT CUSBCamUnitDlg::AddFilterToGraph(IGraphBuilder *pGB, IBaseFilter **ppFilter, IMoniker *pMoniker, LPCWSTR strFilterName)
{
	if (!pGB)      return E_POINTER;
	if (!pMoniker) return E_POINTER;
	if (!ppFilter) return E_POINTER;
	ASSERT(*ppFilter == NULL);
	
	HRESULT hr = pMoniker->BindToObject(0,0, IID_IBaseFilter, (void **)ppFilter);
	FILTER_INFO fi;
	if (!*ppFilter)	return (hr);
	if (strFilterName == NULL)
	{
		(*ppFilter)->QueryFilterInfo(&fi);
		RELEASE_OBJECT(fi.pGraph);
		strFilterName = fi.achName;
	}
	hr = pGB->AddFilter(*ppFilter, strFilterName);
	return hr;
}

void CUSBCamUnitDlg::OnBtnSourceProperty() 
{
	if (m_pCapture[m_nSelectedCam])
	{
		HRESULT hr = ShowFilterPropertyPage(m_pCapture[m_nSelectedCam], true);
		if (FAILED(hr)) ReportError(HRESULT_ERROR_AT_POS(hr));
	}
}

void CUSBCamUnitDlg::OnBtnEncoderProperty() 
{
	if (m_pDecode[m_nSelectedCam])
	{
		HRESULT hr = ShowFilterPropertyPage(m_pDecode[m_nSelectedCam], true);
		if (FAILED(hr)) ReportError(HRESULT_ERROR_AT_POS(hr));
	}
}

void CUSBCamUnitDlg::OnBtnRendererProperties() 
{
/*
	if (pRender)
	{
		HRESULT hr = ShowFilterPropertyPage(m_pRender[m_nSelectedCam], true);
		if (FAILED(hr)) ReportError(HRESULT_ERROR_AT_POS(hr));
	}
*/
}

void CUSBCamUnitDlg::OnCkColor() 
{
	CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_CK_COLOR, m_bColor);
}

void CUSBCamUnitDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	m_bIsVisible = bShow;	
}

void CUSBCamUnitDlg::OnCkLastFilter() 
{
//	GetDlgItem(IDC_INIT_VIDEO_CAM)->EnableWindow(TRUE);
}

void CUSBCamUnitDlg::OnHalftone() 
{
	CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_CK_HALFTONE, m_bHalftone);
}

bool CUSBCamUnitDlg::GetVideoSubtype(IPin *pPin, AM_MEDIA_TYPE  &sMT)
{
	bool bAllowed = false;
	int nCurSel = CB_ERR;
	if (pPin)
	{
		pPin->ConnectionMediaType(&sMT);
	}

	if (sMT.majortype != MEDIATYPE_Video)bAllowed = false;
	else if (MEDIASUBTYPE_UYVY == sMT.subtype)			nCurSel = MT_UYVY;
	else if (MEDIASUBTYPE_YVU9 == sMT.subtype)			nCurSel = MT_YUV9;
	else if (MEDIASUBTYPE_Y211 == sMT.subtype)			nCurSel = MT_YUV211;
	else if (MEDIASUBTYPE_RGB1 == sMT.subtype)			nCurSel = MT_RGB1;
	else if (MEDIASUBTYPE_RGB4 == sMT.subtype)			nCurSel = MT_RGB4;
	else if (MEDIASUBTYPE_RGB8 == sMT.subtype)			nCurSel = MT_RGB8;
	else if (MEDIASUBTYPE_RGB565 == sMT.subtype)			nCurSel = MT_RGB565;
	else if (MEDIASUBTYPE_RGB555 == sMT.subtype)			nCurSel = MT_RGB555;
	else if (MEDIASUBTYPE_RGB24 == sMT.subtype)			nCurSel = MT_RGB24;
	else if (MEDIASUBTYPE_RGB32 == sMT.subtype)			nCurSel = MT_RGB32;
	else if (MEDIASUBTYPE_ARGB32 == sMT.subtype)			nCurSel = MT_ARGB32;
	else if (MEDIASUBTYPE_CLJR == sMT.subtype)			nCurSel = MT_CLJR;
	else if (MEDIASUBTYPE_IF09 == sMT.subtype)			nCurSel = MT_IF09;
	else if (MEDIASUBTYPE_CPLA == sMT.subtype)			nCurSel = MT_CPLA;
	else if (MEDIASUBTYPE_MJPG == sMT.subtype)			nCurSel = MT_MJPG;
	else if (MEDIASUBTYPE_TVMJ == sMT.subtype)			nCurSel = MT_TVMJ;
	else if (MEDIASUBTYPE_WAKE == sMT.subtype)			nCurSel = MT_WAKE;
	else if (MEDIASUBTYPE_CFCC == sMT.subtype)			nCurSel = MT_CFCC;
	else if (MEDIASUBTYPE_IJPG== sMT.subtype)				nCurSel = MT_IJPG;
	else if (MEDIASUBTYPE_Plum == sMT.subtype)			nCurSel = MT_Plum;
	else if (MEDIASUBTYPE_Overlay == sMT.subtype)		nCurSel = MT_Overlay;
	else if (MEDIASUBTYPE_QTMovie == sMT.subtype)		nCurSel = MT_QTMovie;
	else if (MEDIASUBTYPE_QTRpza == sMT.subtype)			nCurSel = MT_QTRpza;
	else if (MEDIASUBTYPE_QTSmc == sMT.subtype)			nCurSel = MT_QTSmc;
	else if (MEDIASUBTYPE_QTRle == sMT.subtype)			nCurSel = MT_QTRle;
	else if (MEDIASUBTYPE_QTJpeg == sMT.subtype)			nCurSel = MT_QTJpeg;
	else if (MEDIASUBTYPE_dvsd == sMT.subtype)			nCurSel = MT_dvsd;
	else if (MEDIASUBTYPE_dvhd == sMT.subtype)			nCurSel = MT_dvhd;
	else if (MEDIASUBTYPE_dvsl == sMT.subtype)			nCurSel = MT_dvsl;
	else if (MEDIASUBTYPE_MPEG1Packet == sMT.subtype)	nCurSel = MT_MPEG1Packet;
	else if (MEDIASUBTYPE_MPEG1Payload == sMT.subtype)	nCurSel = MT_MPEG1Payload;
//	else if (MEDIASUBTYPE_VideoPort== sMT.subtype)		nCurSel = MT_VideoPort;
	else if (MEDIASUBTYPE_VPVideo== sMT.subtype)			nCurSel = MT_VPVideo;
	else if (MEDIASUBTYPE_VPVBI== sMT.subtype)			nCurSel = MT_VPVBI;
	else if (sMT.subtype == GUID_I400)			         nCurSel = MT_I400;
	else if ((MEDIASUBTYPE_YVYU == sMT.subtype) || (MEDIASUBTYPE_YUY2 == sMT.subtype))
	{
		nCurSel = MT_YUYV;
		bAllowed = true;
	}
	else if ((MEDIASUBTYPE_Y411 == sMT.subtype) || (MEDIASUBTYPE_Y41P == sMT.subtype))
	{
		nCurSel = MT_YUV411;
	}
	else	nCurSel = MT_UNKNOWN;
	
	m_cMediaType.SetCurSel(nCurSel);
	return bAllowed;
}

void CUSBCamUnitDlg::OnCkMd() 
{
	if (m_nSelectedCam != CB_ERR)
	{
		CDataExchange dx(this, true);
		DDX_Check(&dx, IDC_CK_MD, m_bMotionDetection[m_nSelectedCam]);
	}
}

void CUSBCamUnitDlg::OnBtnMdProperties() 
{
	ASSERT(m_pMD != NULL);
		
	m_pMD->OpenConfigDlg(0);
}

void CUSBCamUnitDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nIconTimer)
	{
		SetUSBCamIcon(ICON_RUNNING);
		SetButtonIcon(IDC_RECORDING, IDC_STOP);
		KillTimer(nIDEvent);
		m_nIconTimer = 0;
	}
	CDialog::OnTimer(nIDEvent);
}

void CUSBCamUnitDlg::OnSelchangeVideoSource() 
{
	int nCurSel = m_cComboVideoInput.GetCurSel();
	if ((nCurSel == CB_ERR) && (m_nSelectedCam != CB_ERR))		// Hier muﬂ die korrekte Kamera anhand einer ID ausgew‰hlt werden
	{
		nCurSel = m_nSelectedCam;
	}

	if (m_nCurrSelVideoSource != nCurSel)
	{
		AM_MEDIA_TYPE *pmt=NULL;
		int i, iCurrent;
		HRESULT          hr        = NOERROR;
		IMoniker        *pMoniker  = NULL;
		IPin            *pPinOut   = NULL;
		if (m_bStandalone)
		{
			OnStop();
			ReleaseFilters(nCurSel);
		}
		iCurrent = m_cOutputFormats.GetCount();
		for (i=0; i<iCurrent; i++)
		{
			pmt = (AM_MEDIA_TYPE*)m_cOutputFormats.GetItemData(i);
			if (pmt)
			{
				DeleteMediaType(pmt);
			}
		}
		m_cOutputFormats.ResetContent();
		try
		{
			IPropertyBag *pPropBag = NULL;
			VARIANT var;
			VariantInit(&var);

			pMoniker = (IMoniker*)m_cComboVideoInput.GetItemData(nCurSel);
			if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
			if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
			hr = pPropBag->Read(L"CameraID", &var, 0);
			RELEASE_OBJECT(pPropBag);
			
			if (SUCCEEDED(hr) && (var.vt == VT_I4))
			{
				m_nSelectedCam = var.intVal;
			}
			else
			{
				m_nSelectedCam = nCurSel;
			}

			if (m_pCapture[m_nSelectedCam] == NULL)
			{
				hr = AddFilterToGraph(m_pGB, &m_pCapture[m_nSelectedCam], pMoniker, L"Capture Device");
				if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
			}

			GetDlgItem(IDC_BTN_SOURCE_PROPERTY)->EnableWindow((ShowFilterPropertyPage(m_pCapture[m_nSelectedCam], false)== NOERROR) ? true: false);
			GetPin(m_pCapture[m_nSelectedCam], PINDIR_OUTPUT, 0, &pPinOut);

			IAMStreamConfig *pCfg      = NULL;
			hr = pPinOut->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
			if (SUCCEEDED(hr))
			{
				VIDEO_STREAM_CONFIG_CAPS *pVSCC = NULL;
				CString  str, strWnd;
				int  iCount, iSize, nType, nWeight, nMaxWeight = 0, nSelect = CB_ERR;

				pCfg->GetNumberOfCapabilities(&iCount, &iSize);
				pVSCC = (VIDEO_STREAM_CONFIG_CAPS*) CoTaskMemAlloc(iSize);

				for (i=0; i<iCount; i++)
				{
					nWeight = 0;
					pCfg->GetStreamCaps(i, &pmt, (BYTE*)pVSCC);
					GetVideoSubtype(NULL, *pmt);
					nType = m_cMediaType.GetCurSel();
					m_cMediaType.GetLBText(nType, strWnd);
					str.Format("%s (%d x %d)", strWnd, pVSCC->InputSize.cx, pVSCC->InputSize.cy);
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
					switch (nType)
					{
						case MT_I400:  nWeight |= 0x000f0000; break;
						case MT_YUYV:  nWeight |= 0x000e0000; break;
						case MT_ARGB32:nWeight |= 0x00050000; break;
						case MT_RGB32: nWeight |= 0x00040000; break;
						case MT_RGB24: nWeight |= 0x00030000; break;
						case MT_RGB565:nWeight |= 0x00020000; break;
						case MT_RGB555:nWeight |= 0x00010000; break;
					}
					m_cOutputFormats.AddString(str);
					m_cOutputFormats.SetItemData(i, (ULONG)pmt);
					if (nWeight > nMaxWeight)
					{
						nMaxWeight = nWeight;
						nSelect = i;
					}
				}
				CoTaskMemFree(pVSCC);
				RELEASE_OBJECT(pCfg);
				if (m_nOutputFormat[m_nSelectedCam] == CB_ERR)
				{
					m_nOutputFormat[m_nSelectedCam] = nSelect;
				}
				m_cOutputFormats.SetCurSel(m_nOutputFormat[m_nSelectedCam]);
				m_nCurrSelVideoSource = nCurSel;
				UpdateData(FALSE);
			}
		}
		catch(ErrorStruct *ps)
		{
			ReportError(ps);
			ReleaseFilters(nCurSel);
		}
		RELEASE_OBJECT(pPinOut);
		if (m_bStandalone)
		{
			GetDlgItem(IDC_INIT_VIDEO_CAM)->EnableWindow(hr == NOERROR);
		}
		m_cOutputFormats.EnableWindow(hr == NOERROR);
	}
}

void CUSBCamUnitDlg::OnSelchangeComboOutputformat() 
{
	int nSel = m_cOutputFormats.GetCurSel();
	if (nSel != CB_ERR)
	{
		AM_MEDIA_TYPE*pmt = (AM_MEDIA_TYPE*)m_cOutputFormats.GetItemData(nSel);
		if (pmt)
		{
			GetVideoSubtype(NULL, *pmt);
		}
		if (m_nSelectedCam != CB_ERR)
		{
			m_nOutputFormat[m_nSelectedCam] = nSel;
		}
	}
}

HRESULT CUSBCamUnitDlg::FindMatchingFilter(DWORD dwMerit, IPin *pPin, 
														 BOOL bInputNeeded, GUID majortypeIn, GUID subtypeIn, 
														 BOOL bOutputNeeded, GUID majortypeOut, GUID* subtypeOut, int nSubTypes, 
														 BOOL bRender,
														 IBaseFilter **ppFilter, AM_MEDIA_TYPE **ppMT)
{
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
		while (SUCCEEDED(pEnumRegFilters->Next(1, &prf, NULL)) && prf)
		{
			CString	str(prf->Name);
			TRACE("%s\n", str);

			hr = AddFilterToGraph(m_pGB, ppFilter, CGuid(prf->Clsid), L"Test");
			if (SUCCEEDED(hr))
			{
				hr = GetPin(*ppFilter, PINDIR_INPUT, 0, &pPinIn);
				if (SUCCEEDED(hr))
				{
//					hr = pPin->Connect(pPinIn, pmt);
//					if (FAILED(hr))
					{
						hr = m_pGB->Connect(pPin, pPinIn);
					}
					if (hr == S_OK)
					{
						IEnumMediaTypes *pEMT = NULL;
						RELEASE_OBJECT(pPinOut);
						hr = GetPin(*ppFilter, PINDIR_OUTPUT, 0, &pPinOut);
						if (FAILED(hr))	throw HRESULT_ERROR_AT_POS(hr);
						hr = pPinOut->EnumMediaTypes(&pEMT);
						if (SUCCEEDED(hr))
						{
							pEMT->Reset();
							while ((pEMT->Next(1, ppMT, NULL) == S_OK) && ((*ppMT) != NULL))
							{
								for (int i=0; i<nSubTypes; i++)
								{
									if ((*ppMT)->subtype == subtypeOut[i])
									{
										hr = S_OK;
										GetDlgItem(IDC_TXT_FILTER)->SetWindowText(str);
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
				m_pGB->RemoveFilter(*ppFilter);
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

void CUSBCamUnitDlg::OnCkEnableCam() 
{
	if (m_nSelectedCam != CB_ERR)	
	{
		CDataExchange dx(this, true);
		DDX_Check(&dx, IDC_CK_ENABLE_CAM, m_bEnableCam[m_nSelectedCam]);
	}
}

void CUSBCamUnitDlg::RequestInputReset()
{

}

void CUSBCamUnitDlg::RequestOutputReset()
{

}

void CUSBCamUnitDlg::ClearAllJobs()
{

}

/*
class CTestCmdUI : public CCmdUI
{
public:
	CTestCmdUI();

public: // re-implementations only
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetRadio(BOOL bOn);
	virtual void SetText(LPCTSTR);

	BOOL m_bEnabled;
};

CTestCmdUI::CTestCmdUI()
{
	m_bEnabled = TRUE;  // assume it is enabled
}

void CTestCmdUI::Enable(BOOL bOn)
{
	m_bEnabled = bOn;
	m_bEnableChanged = TRUE;
}

void CTestCmdUI::SetCheck(int)
{
	// do nothing -- just want to know about calls to Enable
}

void CTestCmdUI::SetRadio(BOOL)
{
	// do nothing -- just want to know about calls to Enable
}

void CTestCmdUI::SetText(LPCTSTR)
{
	// do nothing -- just want to know about calls to Enable
}

 */


