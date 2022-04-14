/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: SavicUnitDlg.cpp $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/SavicUnitDlg.cpp $
// CHECKIN:		$Date: 18.07.03 13:04 $
// VERSION:		$Revision: 35 $
// LAST CHANGE:	$Modtime: 18.07.03 13:04 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <direct.h>
#include "CInstanceCounter.h"
#include "SavicUnitApp.h"
#include "SavicUnitDlg.h"
#include "CipcInputSavicUnit.h"
#include "CipcInputSavicUnitMDAlarm.h"
#include "CipcOutputSavicUnit.h"
#include "CipcExtraMemory.h"
#include "CVideoControl.h"
#include "VideoInlayWindow.h"	   
#include "oemgui\oemguiapi.h"
#include "WK_Msg.h"
#include "CMySavic.h"
#ifdef _DEBUG
#undef THIS_FILE											   
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
						    
CWK_Timer	theTimer;

extern CSaVicApp theApp;

/////////////////////////////////////////////////////////////////////////////
UINT PollProzessThread(LPVOID pData)
{
	CSaVicUnitDlg*	pAppDlg	 = (CSaVicUnitDlg*)pData;  // Pointer auf's App-Objekt

	if (!pData)
		return 0;

	// Warte bis MainProzeß initialisiert ist.
	
	while (!pAppDlg->m_bRun)
	{
		Sleep(10);
	}
	
	// Poll bis Mainprozeß terminiert werden soll.
	while (pAppDlg->m_bRun)
	{
		pAppDlg->PollProzess();	// Prozeßmanagement
	}
	WK_TRACE(_T("Exit PollProzessManagementThread\n"));
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
UINT SwitchThread(LPVOID pData)
{
	CSaVicUnitDlg*	pAppDlg	 = (CSaVicUnitDlg*)pData;  // Pointer auf's App-Objekt

	if (!pData)
		return 0;

	// Warte bis MainProzeß initialisiert ist.
	
	while (!pAppDlg->m_bRun)
	{
		Sleep(10);
	}
	
	// Poll bis Mainprozeß terminiert werden soll.
	while (pAppDlg->m_bRun)
	{
		pAppDlg->SwitchThread();
	}
	WK_TRACE(_T("Exit SwitchThread\n"));
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSaVicUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CSaVicUnitDlg)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_DLG_LUMINANCE, OnDlgLuminance)
	ON_BN_CLICKED(IDC_FE_NTSC_CCIR, OnFeNtscCCIR)
	ON_BN_CLICKED(IDC_FE_PAL_CCIR, OnFePalCCIR)
	ON_BN_CLICKED(IDC_SOURCE1, OnSource0)
	ON_BN_CLICKED(IDC_SOURCE2, OnSource1)
	ON_BN_CLICKED(IDC_SOURCE3, OnSource2)
	ON_BN_CLICKED(IDC_SOURCE4, OnSource3)
	ON_BN_CLICKED(IDC_FE_LNOTCH, OnFeLNotch)
	ON_BN_CLICKED(IDC_FE_LDEC, OnFeLDec)
	ON_BN_CLICKED(IDC_RELAIS0, OnRelais0)
	ON_BN_CLICKED(IDC_ENC_START, OnEncStart)
	ON_BN_CLICKED(IDC_ENC_STOP, OnEncStop)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_BN_CLICKED(IDC_COMPRESS_1, OnCompress1)
	ON_BN_CLICKED(IDC_COMPRESS_2, OnCompress2)
	ON_BN_CLICKED(IDC_COMPRESS_3, OnCompress3)
	ON_BN_CLICKED(IDC_COMPRESS_4, OnCompress4)
	ON_BN_CLICKED(IDC_COMPRESS_5, OnCompress5)
	ON_BN_CLICKED(IDC_HIGH_RES, OnHighResolution)
	ON_BN_CLICKED(IDC_LOW_RES, OnLowResolution)
	ON_WM_DESTROY()
	ON_WM_QUERYENDSESSION()
	ON_WM_ENDSESSION()
	ON_BN_CLICKED(IDC_VIDEOINLAY, OnVideoInlay)
	ON_BN_CLICKED(IDC_COMPRESS_0, OnCompress0)
	ON_BN_CLICKED(IDC_DLG_MD_CONFIG, OnDlgMdConfig)
	ON_BN_CLICKED(IDC_ENABLE_TWO_FIELDS, OnEnableDoubleFieldMode)
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_LED1, OnLed1)
	ON_BN_CLICKED(IDC_LED2, OnLed2)
	ON_EN_CHANGE(IDC_EDIT_FPS, OnChangeEditFps)
	ON_BN_CLICKED(IDC_INIT_PLL, OnInitPll)
	ON_BN_CLICKED(IDC_NOISE_REDUCTION, OnNoiseReduction)
	ON_MESSAGE(WM_SAVIC_REQUEST_OUTPUT_RESET,	OnWmRequestOutputReset)
	ON_MESSAGE(WM_SAVIC_REQUEST_INPUT_RESET,	OnWmRequestInputReset)
	ON_WM_ACTIVATEAPP( )	
	ON_WM_QUERYOPEN()
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_POWERBROADCAST, OnWmPowerBroadcast)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(SAVIC_OPEN_MD_DLG, DoOpenMDDlg)	
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_COMMAND(ID_SETTINGS, OnDlgMdConfig)
	ON_COMMAND(ID_ABOUT,	OnAbout)
	ON_BN_CLICKED(IDC_RADIO_JPEG, OnSelectCompressionTypeJpeg)
	ON_BN_CLICKED(IDC_RADIO_DIB, OnSelectCompressionTypeDIB)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaVicUnitDlg)
	DDX_Text(pDX, IDC_EDIT_FPS, m_sFPS);
	DDV_MaxChars(pDX, m_sFPS, 2);
	//}}AFX_DATA_MAP
}
  
/////////////////////////////////////////////////////////////////////////////
CSaVicUnitDlg::CSaVicUnitDlg() : CDialog(CSaVicUnitDlg::IDD)
{
	//{{AFX_DATA_INIT(CSaVicUnitDlg)
	m_sFPS = _T("25");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
 	
	m_bOk				= FALSE;
	m_bRun				= FALSE;
	m_bOn				= FALSE;
	m_bDoNoiseReduction	= FALSE;
	m_hIcon1			= AfxGetApp()->LoadIcon(IDR_SAVICUNIT1);	// Blue
	m_hIcon2			= AfxGetApp()->LoadIcon(IDR_SAVICUNIT2);	// Green
	m_hIcon3			= AfxGetApp()->LoadIcon(IDR_SAVICUNIT3);	// Red
	
	m_pCodec			= NULL;
	m_pPollProzessThread= NULL;
	m_pSwitchThread		= NULL;
	m_pUser				= NULL;
	m_pPermission		= NULL;
  	m_pCurrentEncodeJob	= NULL;
	m_pCVideoControl	= NULL;

	m_dwEncodedFrames	= 0;
	m_dwFrameNumber[0]	= 0;
	m_dwFrameNumber[1]	= 0;
	m_dwFrameNumber[2]	= 0;
	m_dwFrameNumber[3]	= 0;

	m_dwDecodedFrames	= 0;
	m_wFormat			= ENCODER_RES_HIGH;
	m_dwBPF				= BPF_3;
	m_nEncoderState		= ENCODER_OFF;
	m_nTimePerFrame		= 40;
	CString sIniFile	= _T("SaVicUnit\\Device");
	m_eCompressionType	= COMPRESSION_JPEG;

	m_dwSourceSwitch	= 0;
	m_bVideoPresentCheck= TRUE;
	m_bSaveInLog		= FALSE;
	m_dwStandbyTime		= 30000;

	// Alle Pointer initialisieren.
	m_pOutputSaVicCamera= NULL;
	m_pOutputSaVicRelay	= NULL;			
	m_pInput			= NULL;
	m_pInputMDAlarm	 	= NULL;
	m_pVideoInlayWindow	= NULL;

	m_hSyncSemaphore	= NULL;

	m_dwLastJpegArrivedTC = 0;

	m_dwCamMask			= 0;

	m_bLED1State		= FALSE;
	m_bLED2State		= FALSE;

	m_nSource			= 0;

	_TCHAR szBuffer[256];

	m_sIniFile.Format(sIniFile + _T("%u"), theApp.GetCardRef());

	CString sJpegPath;
	sJpegPath.Format(_T("E:\\Jpeg%u\\"), theApp.GetCardRef());

	CString sMDPath;
	sMDPath.Format(_T("MDMask%u\\"), theApp.GetCardRef());

	// Defaulteinstellungen für die SaVic in die Registry schreiben, wenn nicht vorhanden
	ReadConfigurationString(_T("Debug"),		  _T("JpegPath"),	sJpegPath,	szBuffer, 128,	m_sIniFile);
	ReadConfigurationString(_T("MotionDetection"),_T("MaskPath"),	sMDPath,	szBuffer, 128,	m_sIniFile);

	if (theApp.GetCardRef() == 1)
		WriteConfigurationInt(_T("General"),	_T("ListenToExtEvent"), 1,	m_sIniFile);
	else
		WriteConfigurationInt(_T("General"),	_T("ListenToExtEvent"), 0,	m_sIniFile);	

	//m_bStandalone = 0-> SaVicUnit arbeitet mit Security, 1-> SaVicUnit ist Standalone
	m_bStandalone = ReadConfigurationInt(_T("Debug"), _T("Mode"),  0, m_sIniFile);
	if (m_bStandalone){
		WK_TRACE(_T("Running as 'standalone'\n"));
	}
    
	m_bSaveInLog = ReadConfigurationInt(_T("Debug"), _T("SaveInLog"),  0, m_sIniFile);
	if (m_bSaveInLog){
		WK_TRACE(_T("Jpeg saved in log'\n"));
	}

	// Pfad zum Speichern der Jpeg-dateien einlesen
	ReadConfigurationString(_T("Debug"), _T("JpegPath"), _T(""), szBuffer,  255, m_sIniFile); 
	m_sJpegPath = szBuffer;

	if (m_bStandalone)
	{
		_tmkdir(m_sJpegPath);
		_tmkdir(m_sJpegPath+_T("\\Camera1"));
		_tmkdir(m_sJpegPath+_T("\\Camera2"));
		_tmkdir(m_sJpegPath+_T("\\Camera3"));
		_tmkdir(m_sJpegPath+_T("\\Camera4"));

	}
 	// Soll das Videosignal überwacht werden ?
	m_bVideoPresentCheck = (BOOL)ReadConfigurationInt(_T("General"), _T("VideoPresentCheck"), m_bVideoPresentCheck, m_sIniFile);
	if (m_bVideoPresentCheck)
		WK_TRACE(_T("Video checking on\n"));
	else
		WK_TRACE(_T("Video checking off\n"));

	// Zeit bis der Encoder gestopt wird, wenn keine EncoderJobs vorliegen. 
	m_dwStandbyTime = ReadConfigurationInt(_T("General"), _T("StandbyTime"),  30000, m_sIniFile);
	WK_TRACE(_T("StandbyTime=%lu ms\n"), m_dwStandbyTime);

	// Zeit zwischen 2 (Halb)Bilder
	if (ReadConfigurationInt(_T("General"), _T("StoreField"),  0, m_sIniFile) == 2)
		m_nTimePerFrame = 20-2;	// Beide Halbbilder -> 20ms
	else
		m_nTimePerFrame = 40-2;	// Ein Halbbild -> 40ms
}

/////////////////////////////////////////////////////////////////////////////
CSaVicUnitDlg::~CSaVicUnitDlg()
{
	WK_TRACE(_T("~CSaVicUnitDlg\n"));
	WK_DELETE(m_pUser);
	WK_DELETE(m_pPermission);
	if (m_hShutDownEvent)
	{	
		CloseHandle(m_hShutDownEvent);
		m_hShutDownEvent = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::PostNcDestroy() 
{
	WK_TRACE(_T("PostNcDestroy\n"));
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSaVicUnitDlg::InitializeCodec()
{
	// Suche nach PCI-Karte...
	CPCIBus			PCIBus;
	CPCIDevice		*pDevice	 = NULL;
	CPCIDeviceList	*pDeviceList = PCIBus.ScanBus();

	if (m_pCodec)
	{
		WK_TRACE_WARNING(_T("Codec is already initialized\n"));
		return FALSE;
	}

	if (pDeviceList)
	{
		pDeviceList->Trace();

		// Soll die SubSystemID berücksichtigt werden?
		if (ReadConfigurationInt(_T(""), _T("IgnoreSubSystemIDs"),  0, _T("SaVicUnit")))
		{
			// Scanne nach der x-ten SaVic (x=CardRef = 1...4)
			pDevice = pDeviceList->GetDeviceByIndex(SAVIC_VENDOR_ID, SAVIC_DEVICE_ID, theApp.GetCardRef());
			if (pDevice)
				m_pCodec = new CMySaVic(this, m_sIniFile, pDevice);
		}
		else
		{
			// Scanne nach Savic 1....4
			if (theApp.GetCardRef() == 1)
				pDevice = pDeviceList->GetDevice(SAVIC_VENDOR_ID, SAVIC_DEVICE_ID, SAVIC_SUBSYSTEM_ID1);
			else if (theApp.GetCardRef() == 2)
				pDevice = pDeviceList->GetDevice(SAVIC_VENDOR_ID, SAVIC_DEVICE_ID, SAVIC_SUBSYSTEM_ID2);
			else if (theApp.GetCardRef() == 3)
				pDevice = pDeviceList->GetDevice(SAVIC_VENDOR_ID, SAVIC_DEVICE_ID, SAVIC_SUBSYSTEM_ID3);
			else if (theApp.GetCardRef() == 4)
				pDevice = pDeviceList->GetDevice(SAVIC_VENDOR_ID, SAVIC_DEVICE_ID, SAVIC_SUBSYSTEM_ID4);
		}
	}

	if (pDevice)
		m_pCodec = new CMySaVic(this, m_sIniFile, pDevice);
	else
		WK_TRACE(_T("SaVic-Board not detected\n"));

	if (!m_pCodec)
		return FALSE;
							    
	if (!m_pCodec->IsValid())
	{
		WK_TRACE_ERROR(_T("CSaVicUnitDlg::Create\tCCodec-Objekt konnte nicht angelegt werden.\n"));

		WK_DELETE(m_pCodec);

		return FALSE;
	}

	if (!m_pCodec->Open())
	{
		WK_TRACE_ERROR(_T("CSaVicUnitDlg::Create\tCCodec-Objekt konnte nicht initialisiert werden.\n"));
		WK_DELETE(m_pCodec);
		return FALSE;				  
	}						 

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSaVicUnitDlg::Create()
{
	if (!CDialog::Create(IDD))
	{
		return FALSE;
	}
	
	// Dieses Event veranlaßt das Beenden der einzelnen Threads
	m_hShutDownEvent = CreateEvent(0, TRUE, FALSE, NULL);
	if (!m_hShutDownEvent)
		WK_TRACE_ERROR(_T("Create m_hShutDownEvent failed\n"));
	
	if (!InitializeCodec())
		return FALSE;

	// Zeit die nach dem Ausfall einer kamera verstreichen soll bis diese erneut geprüft wird
	DWORD dwVideoRecheckTime  = ReadConfigurationInt(_T("General"), _T("VideoRecheckTime"),  30000, m_sIniFile);

	// Dies ist immer der Zeitpunkt des zuletzt eingetroffenen Bildes. Initialisiere ihn mit
	// der aktuellen Zeit.
	m_dwLastJpegArrivedTC = GetTickCount();
	
	// CVideoControl-Objekt anlegen
	m_pCVideoControl = new CVideoControl(m_pCodec, dwVideoRecheckTime);

	if (!m_pCVideoControl)
	{
		WK_TRACE_ERROR(_T("CSaVicUnitDlg::Create\tCVideoControl-Objekt konnte nicht angelegt werden.\n"));
		return FALSE;
	}		

	CString sSMName;

	// die CIPC-Schnittstellen aufbauen.
	sSMName = SM_SAVIC_INPUT;
	if (theApp.GetCardRef() != 1)
		sSMName += (_TCHAR)('0'+ theApp.GetCardRef());
	m_pInput = new CIPCInputSaVicUnit(this, m_pCodec, sSMName);
	if (!m_pInput)
		WK_TRACE_ERROR(_T("CSaVicUnitDlg::Create\tCIPCInputSaVicUnit-Objekt konnte nicht angelegt werden.\n"));

	sSMName = SM_SAVIC_MD_INPUT;
	if (theApp.GetCardRef() != 1)
		sSMName += (_TCHAR)('0'+ theApp.GetCardRef());
	m_pInputMDAlarm = new CIPCInputSaVicUnitMDAlarm(this, m_pCodec, sSMName);
	if (!m_pInputMDAlarm)
		WK_TRACE_ERROR(_T("CSaVicUnitDlg::Create\tCIPCInputSaVicUnitMDAlarm-Objekt konnte nicht angelegt werden.\n"));
	
	sSMName = SM_SAVIC_OUTPUT_CAMERAS;
	if (theApp.GetCardRef() != 1)
		sSMName += (_TCHAR)('0'+ theApp.GetCardRef());
	
	m_pOutputSaVicCamera = new CIPCOutputSaVicUnit(this, m_pCodec, OUTPUT_TYPE_SAVIC_CAMERA, sSMName, m_sIniFile);
	if (!m_pOutputSaVicCamera)
		WK_TRACE_ERROR(_T("CSaVicUnitDlg::Create\tCIPCOutputSaVicUnit-SaVicCamera-Objekt konnte nicht angelegt werden.\n"));
	
	sSMName = SM_SAVIC_OUTPUT_RELAYS;
	if (theApp.GetCardRef() != 1)
		sSMName += (_TCHAR)('0'+ theApp.GetCardRef());

	m_pOutputSaVicRelay = new CIPCOutputSaVicUnit(this, m_pCodec, OUTPUT_TYPE_SAVIC_RELAY, sSMName, m_sIniFile);
	if (!m_pOutputSaVicRelay)
		WK_TRACE_ERROR(_T("CSaVicUnitDlg::Create\tCIPCOutputSaVicUnit-Relay-Objekt konnte nicht angelegt werden.\n"));	

	if (!m_bStandalone) 
	{
		CString sEventName = _T("SaVicSyncEvent");
		if (theApp.GetCardRef() != 1)
			sEventName += (_TCHAR)('0'+ theApp.GetCardRef());

		WK_TRACE(_T("sync semaphore %s\n"),sEventName);

		m_hSyncSemaphore = CreateSemaphore(NULL, 0, 10, sEventName);
		if (m_hSyncSemaphore==NULL) 
		{
			WK_TRACE_ERROR(_T("Failed to create sync semaphore\n"));
		}
	}						    

	// PollProzessThread anlegen
	m_pPollProzessThread = AfxBeginThread(PollProzessThread, this);
	if (m_pPollProzessThread)
		m_pPollProzessThread->m_bAutoDelete = FALSE;

	if (m_bStandalone)
	{
 		m_pSwitchThread = AfxBeginThread(::SwitchThread, this);
		if (m_pSwitchThread)
			m_pSwitchThread->m_bAutoDelete = FALSE;
	}

	m_bOk  = TRUE;
	m_bRun = TRUE;

	if (m_bStandalone)
	{	
		m_dwCamMask = m_pCodec->ScanForCameras(0xffff);

		UpdateDlg();
		ShowWindow(SW_SHOW);
	}
	else
	{
		// Icon ins Systemtray
		NOTIFYICONDATA tnd;

		CString sTip = COemGuiApi::GetApplicationName(theApp.GetApplicationId());

		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID			= 1;

		tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
		tnd.uCallbackMessage = WM_TRAYCLICKED;
		tnd.hIcon		= theApp.LoadIcon(IDR_SAVICUNIT1);

		lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
		Shell_NotifyIcon(NIM_ADD, &tnd);
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::ClearAllJobs()
{
	ClearAllEncodeJobs();
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::ClearAllEncodeJobs()
{
	ClearCurrentEncodeJob();
	m_jobQueueEncode.SafeDeleteAll();
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::ClearCurrentEncodeJob()
{
	m_csEncode.Lock();

	if (m_pCurrentEncodeJob)
	 	m_jobQueueEncode.IncreaseSemaphore(); // Event auslösen -> Nächsten Encoderjob holen
	WK_DELETE(m_pCurrentEncodeJob);
	m_csEncode.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::RequestOutputReset()
{
	// WorkerThread benutzt MFC -> Deshalb Postmessage
	PostMessage(WM_SAVIC_REQUEST_OUTPUT_RESET, 0, 0); 
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::RequestInputReset()
{
	// WorkerThread benutzt MFC -> Deshalb Postmessage
	PostMessage(WM_SAVIC_REQUEST_INPUT_RESET, 0, 0); 
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::Dump( CDumpContext &dc ) const
{
	 CObject::Dump( dc );
	 dc << _T("CSaVicUnitDlg = ");
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSaVicUnitDlg::IsValid()
{
	return m_bOk;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSaVicUnitDlg::Login()
{
	CString sUser;
	CString sPassword;
	CSecID  idUser;
	CUserArray UserArray;
	BOOL bRet = FALSE;			 
	CWK_Profile prof;

	COEMLoginDialog dlg(this);
										   
	if (IDOK==dlg.DoModal())	  		 
		bRet = TRUE;		 

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
WORD CSaVicUnitDlg::GetNextSource(WORD wSource)
{
	WORD wI = wSource;
	do
	{
		wI++;
		wI %= 16;

	}while (!TSTBIT(m_dwCamMask, wI) && (wI != wSource));

	return wI;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::SwitchThread()
{				    
	static  int   nSource = VIDEO_SOURCE0;
	static  DWORD dwTC = WK_GetTickCount();
	static  float fSwitchPerSeconds = 0.0;

	_TCHAR	szText[255];
	int		nNextSource = 0;

	if (m_bOn == TRUE)
	{
		// Schaltet nur über die beim Start der Unit gescannten Kameras
		m_dwSourceSwitch++;
		nNextSource = GetNextSource(nSource);
		OnSource(nSource, nNextSource);
		nSource = nNextSource;

		if (m_bOk)
		{
			if ((m_dwSourceSwitch % 25) == 0)
			{
				fSwitchPerSeconds = (float)(25*1000) / (float)(WK_GetTickCount() - dwTC);
				dwTC = WK_GetTickCount();
				WK_STAT_LOG(_T("Reset"), fSwitchPerSeconds+0.5, _T("Switch fps"));

			}
			_stprintf(szText, _T("%lu/%3.1f fps"), m_dwSourceSwitch, fSwitchPerSeconds);
			SetDlgText(IDC_SWITCHES, szText);
		}
	}
	else
		Sleep(100);
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::PollProzess()
{
	static BOOL bOnlyOnce = FALSE;
	HANDLE hEvent[2];

	hEvent[0] = m_jobQueueEncode.GetSemaphore();
	hEvent[1] = m_hShutDownEvent;

	// Warte bis neuer Job verfügbar ist
	switch (WaitForMultipleObjects(2, hEvent, FALSE, m_dwStandbyTime))
	{
		case WAIT_OBJECT_0:
			ProzessManagement();
			bOnlyOnce = TRUE;
			break;
		case WAIT_OBJECT_0+1:
			break;
		case WAIT_TIMEOUT:
			if (bOnlyOnce)
			{
				WK_TRACE(_T("JobQueue empty\tEncoder stop\n"));
				if (m_pOutputSaVicCamera)
					m_pOutputSaVicCamera->EncoderStop(NULL);
				bOnlyOnce = FALSE;
				SetSaVicIcon(SAVIC_ICON_BLUE);
			}
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSaVicUnitDlg::VideoTimeOutCheck(WORD wCamSubID)
{
	// Soll das Videosignal überwacht werden
	if (!m_bVideoPresentCheck)
		return TRUE;

	if (!m_pCVideoControl)
		return TRUE;

	return m_pCVideoControl->TimeOutCheck(wCamSubID); 
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnReceivedImageData(const IMAGE* pImage)
{
	_TCHAR szText[16]		= {0};
//	WK_TRACE(_T("OnReceivedImageData  Source=%d\n"), pImage->wSource);

	// Gültiges Bild?
	if (!pImage)
		return;

	wsprintf(szText, _T("%lu\0"), m_dwEncodedFrames);
	if (m_bOk)
		SetDlgText(IDC_ENCODED_FRAMES, szText);
	m_dwEncodedFrames++;

	m_dwLastJpegArrivedTC = GetTickCount();

	if (pImage->wSource == m_nSource)
		m_evImage.SetEvent();

	if (m_bStandalone)
	{
		if (!m_pVideoInlayWindow)
			SaveImageToFile(pImage);
		else
		{
			BOOL bResult = FALSE;
			if (pImage->wCompressionType == COMPRESSION_JPEG)
			{
				bResult = m_Jpeg.DecodeJpegFromMemory((BYTE*)pImage->pImageData, pImage->dwImageLen);
			
			}
			else if (pImage->wCompressionType == COMPRESSION_YUV_422)
			{
				bResult = m_Jpeg.SetDIBData((LPBITMAPINFO)pImage->pImageData);
			}
			if (bResult)
			{
				CRect rcClient(0,0,0,0);
				m_pVideoInlayWindow->GetClientRect(rcClient);
				CRect rcWnd = rcClient;
				
				// Quadview
				if (m_bOn)
				{
					rcWnd.right = rcClient.right/2;
					rcWnd.bottom = rcClient.bottom/2;	
					if (pImage->wSource == 1)
						rcWnd.OffsetRect(CPoint(rcClient.Width()/2, 0));
					if (pImage->wSource == 2)
						rcWnd.OffsetRect(CPoint(0, rcClient.Height()/2));
					if (pImage->wSource == 3)
						rcWnd.OffsetRect(CPoint(rcClient.Width()/2, rcClient.Height()/2));
				}
				m_Jpeg.OnDraw(m_pVideoInlayWindow->GetDC(), rcWnd);
			}
		}
	}
	else
	{
		// Daten an die Outputgroup übergeben.
		m_csEncode.Lock();
			
		// Existiert ein Job ?
		if (m_pCurrentEncodeJob != NULL) 
		{
			// Stimmt der geforderte Prozeß mit dem gelieferten Prozeß überein.?
			if (m_pCurrentEncodeJob->m_dwUserData == pImage->dwProzessID) 
			{
				// Daten an die Outputgroup übergeben.
				if (m_pOutputSaVicCamera)
					m_pOutputSaVicCamera->IndicationDataReceived(pImage,TRUE);

				if (m_pCurrentEncodeJob)
					m_pCurrentEncodeJob->m_iOutstandingPics--;

				if (m_pCurrentEncodeJob->m_iOutstandingPics == 0) 
				{
					ClearCurrentEncodeJob(); // Letztes Bild erhalten -> Job beenden
				}
			}
			else
			{
	//			WK_TRACE(_T("Data without job (ProzessIDs: (%lu, %lu)\n"),	m_pCurrentEncodeJob->m_dwUserData,
	//																	pImage->dwProzessID);
				// ProzessID 0 gibt's nicht, wird nur von der SaVic intern verwendet
	//			if (pImage->dwProzessID == 0)
	//				ClearCurrentEncodeJob();
			}	
		}
		m_csEncode.Unlock();
	}
	return;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnReceivedEncoderError(int nErrorCode)
{
	if (nErrorCode == ERR_TIMEOUT)
	{
		// Es kamen keine Bilder. Dann mit dem nächsten Job fortfahren.
		if (m_pCurrentEncodeJob != NULL) 
		{
			CSecID		camID = m_pCurrentEncodeJob->m_camID;

			ClearCurrentEncodeJob();
			
			// Es sind mehr als 5 Sekunden lang keine neuen Bilder eingetroffen.
			if (GetTimeSpan(m_dwLastJpegArrivedTC) > 5*1000)
			{
				CString sText;
				sText.Format(_T("Restart Encoder (Current Source=%d)"), camID.GetSubID()+1);
				WK_STAT_PEAK(_T("Reset"), 1, sText);
				
				// Encoder neu starten
				if (m_pCodec)
					m_pCodec->RestartEncoder();
				m_dwLastJpegArrivedTC = GetTickCount();
			}
		}
	}
	return;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnReceivedAlarm(WORD wCurrentAlarmState)
{
	if (!m_bStandalone)
	{
 		if (m_pInput)
			m_pInput->AlarmStateChanged(wCurrentAlarmState);
	}
	else
	{
		WK_TRACE(_T("OnReceivedAlarm (0x%x)\n"), wCurrentAlarmState);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnReceivedMotionAlarm(WORD wSource, CMDPoints& Points)
{
	if (!m_bStandalone)
	{
		if (m_pInputMDAlarm)
			m_pInputMDAlarm->OnReceivedMotionAlarm(wSource, Points, TRUE);
 		if (m_pInputMDAlarm)
			m_pInputMDAlarm->OnReceivedMotionAlarm(wSource, Points, FALSE);
	}
	else
	{
//		WK_TRACE(_T("OnReceivedMotionAlarm (Source=%u (%d, %d))\n"), wSource+1, Point.x, Point.y);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnReceivedCameraStatus(WORD wSource, BOOL bVideoState)
{
//	WK_TRACE(_T("CameraStatus: Source=%d, Status=%d\n"), wSource+1, bVideoState);

	// Soll das Videosignal überwacht werden
	if (!m_bVideoPresentCheck)
		return;

	// Videostatus der Kamera geändert?
	if (m_pCVideoControl && m_pCVideoControl->HasVideoStateChanged(wSource, bVideoState))
	{
		// Statusänderung dem Server mitteilen
		if (m_pOutputSaVicCamera)
			m_pOutputSaVicCamera->IndicationVideoState(wSource, bVideoState);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnPowerOffButtonPressed() const
{	
	WK_TRACE(_T("PowerOff-Button pressed\n"));
	MessageBeep(0);
	CWnd* pWnd = FindWindow(_T("DVClient"), NULL);
	if (WK_IS_WINDOW(pWnd))
   		pWnd->PostMessage(WM_POWEROFF_PRESSED, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnResetButtonPressed() const
{
#if (0)
	WK_TRACE(_T("Reset-Button pressed\n"));
	MessageBeep(0);
	CWnd* pWnd = FindWindow(_T("DVClient"), NULL);
	if (WK_IS_WINDOW(pWnd))
   		pWnd->PostMessage(WM_RESET_PRESSED, 0, 0);
#else
	if (m_pOutputSaVicCamera)
		m_pOutputSaVicCamera->OnResetButtonPressed();

#endif
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnReceivedMotionDIB(WORD wSource, HANDLE hDIB)
{
	if (!m_bStandalone)				 
	{
		if (m_pOutputSaVicCamera)
		m_pOutputSaVicCamera->OnReceivedMotionDIB(wSource, hDIB);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::SaveImageToFile(const IMAGE* pImage)
{
	if (!pImage)
		return;

	LPCSTR	lpBuffer	= (char*)pImage->pImageData;	// Pointer auf Bilddaten
	DWORD	dwLen		= pImage->dwImageLen;			// Länge der Bilddaten
	DWORD	dwProzessID	= pImage->dwProzessID;
	CString sCamera;
	CString s;
	CString sExt;

	if (!lpBuffer)
		return;

	if (pImage->wCompressionType == COMPRESSION_JPEG)
		sExt = _T("jpg");
	else if (pImage->wCompressionType == COMPRESSION_YUV_422)
	{
		sExt = _T("yuv422");
		dwLen += sizeof(BITMAPINFO);
	}
	else
	{
		sExt = _T("Unknown");
		WK_TRACE_ERROR(_T("SaveImageToFile: Unknown Compressiontype (%d)\n"), pImage->wCompressionType);
	}

#if (0)
	WORD	wSource		= pImage->wSource;		// Videoquelle (0...7)
	DWORD	dwFieldID	= pImage->dwFieldID;		// Fieldcounter
	WORD	wField		= pImage->wField;		// EVEN_FIELD oder ODD_FIELD	

	if (wField == EVEN_FIELD)
	{
		s.Format(_T("%sJ_%04lu_%02u_%04lu_E.%s\0"),
						m_sJpegPath,
						m_dwEncodedFrames,
						wSource+1,
						dwFieldID,
						sExt);
	}
	else
	{
		s.Format(_T("%sJ_%04lu_%02u_%04lu_O.jpg\0"),
						m_sJpegPath,
						m_dwEncodedFrames,
						wSource+1,
						dwFieldID);
	}
#else
	sCamera.Format(_T("Camera%u"), pImage->wSource+1);

	s.Format(_T("%s\\%s\\J_%06lu.%s\0"), m_sJpegPath, sCamera,	m_dwFrameNumber[pImage->wSource], sExt);
#endif

	// Soll das Jpegfile gesichert werden, oder nur die Informationen ins Logfile
	if (m_bSaveInLog)
	{
		WK_TRACE(_T("%s\tProzeßID=%lu\tFieldID=%lu\tLen=%lu\n"),	s, dwProzessID, pImage->dwFieldID, dwLen);
	}
	else
	{
		CFile wFile(s,	CFile::modeCreate | CFile::modeWrite  |
						CFile::typeBinary | CFile::shareDenyWrite);

		wFile.WriteHuge(lpBuffer, dwLen); 
	}

	m_dwFrameNumber[pImage->wSource]++;

	return;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::ProzessManagement()
{
	m_csEncode.Lock();

	if (m_pCurrentEncodeJob == NULL)
	{
		// Aktuellen und nächsten Job holen
		m_pCurrentEncodeJob = m_jobQueueEncode.SafeGetAndRemoveHead();
		
		if (m_pCurrentEncodeJob)
		{
			CIPCOutputSaVicUnit* pOutput = (CIPCOutputSaVicUnit*)m_pCurrentEncodeJob->m_pCipc;
			SetSaVicIcon(SAVIC_ICON_RED);

			switch (m_pCurrentEncodeJob->m_action)
			{			  
				case VJA_ENCODE:
					if (pOutput)
				 		pOutput->EncoderStart(m_pCurrentEncodeJob);
					break;
				case VJA_STOP_ENCODE:
					SetSaVicIcon(SAVIC_ICON_BLUE);
					if (pOutput)       					    
						pOutput->EncoderStop(m_pCurrentEncodeJob);
					ClearCurrentEncodeJob();	// job done 
					break;
				case VJA_IDLE:
					Sleep(m_nTimePerFrame);		// Ein (Halb)bild warten
					ClearCurrentEncodeJob();	// job done 
					break;
				case VJA_SYNC:
					if (m_hSyncSemaphore)
					{
						ReleaseSemaphore(m_hSyncSemaphore, 1, NULL);
					}
					else
					{
						WK_TRACE_ERROR(_T("no sync semaphore to release\n"));
					}
					ClearCurrentEncodeJob();	// job done 
					break;
				default:	
					WK_TRACE_ERROR(_T("Invalid Encode job type\n"));
					ClearCurrentEncodeJob();	// job done 
			}	// end of job switch
		}	// end of job found		  
		else															 
		{
			SetSaVicIcon(SAVIC_ICON_GREEN);
		}
	}
	
	m_csEncode.Unlock();
}


/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnCancel() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnOK() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSaVicUnitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetSaVicIcon(SAVIC_ICON_BLUE, FALSE);	// Small Icon
	SetSaVicIcon(SAVIC_ICON_BLUE, TRUE);	// Large Icon

	if (theApp.GetCardRef() == 1)
		SetWindowText(WK_APP_NAME_SAVICUNIT1);
	else if (theApp.GetCardRef() == 2)
		SetWindowText(WK_APP_NAME_SAVICUNIT2);
	else if (theApp.GetCardRef() == 3)
		SetWindowText(WK_APP_NAME_SAVICUNIT3);
	else if (theApp.GetCardRef() == 4)
		SetWindowText(WK_APP_NAME_SAVICUNIT4);
	else
		WK_TRACE_ERROR(_T("Unknown Applicationname\n"));

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::SetSaVicIcon(WORD wColor, BOOL bType)
{
	static WORD wCurColor	= SAVIC_ICON_NOCOLOR;
	static BOOL bCurType	= FALSE;

	if ((wColor != wCurColor) || (bType != bCurType))
	{
		wCurColor = wColor;
	
		if (m_bStandalone)
		{
			switch (wColor)
			{
				case SAVIC_ICON_BLUE:
					if (m_hIcon1)
						::PostMessage(m_hWnd, WM_SETICON,	bType, (LPARAM)m_hIcon1);
		 			break;
				case SAVIC_ICON_GREEN:
					if (m_hIcon2)
						::PostMessage(m_hWnd, WM_SETICON,	bType, (LPARAM)m_hIcon2);
					break;
				case SAVIC_ICON_RED:
					if (m_hIcon3)
						::PostMessage(m_hWnd, WM_SETICON,	bType, (LPARAM)m_hIcon3);
					break;
				default:
					WK_TRACE(_T("Nicht definiertes Icon\n"));
			}
		}
		else
		{
			// Icon ins Systemtray
			NOTIFYICONDATA tnd;

			CString sTip = COemGuiApi::GetApplicationName(theApp.GetApplicationId());

			tnd.cbSize		= sizeof(NOTIFYICONDATA);
			tnd.hWnd		= m_hWnd;
			tnd.uID			= 1;

			tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
			tnd.uCallbackMessage = WM_TRAYCLICKED;
			tnd.hIcon		= theApp.LoadIcon(IDR_SAVICUNIT1);

			lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);

			switch (wColor)
			{
				case SAVIC_ICON_BLUE:
					tnd.hIcon =	m_hIcon1;
		 			break;
				case SAVIC_ICON_GREEN:
					tnd.hIcon =	m_hIcon2;
					break;
				case SAVIC_ICON_RED:
					tnd.hIcon =	m_hIcon3;
					break;
				default:
					WK_TRACE(_T("Nicht definiertes Icon\n"));
			}
			Shell_NotifyIcon(NIM_MODIFY, &tnd);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSaVicUnitDlg::OnQueryOpen()
{
	return m_bStandalone;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSaVicUnitDlg::OnQueryEndSession() 
{
	if (!CDialog::OnQueryEndSession())
		return FALSE;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnEndSession(BOOL bEnding) 
{
	CDialog::OnEndSession(bEnding);

	if (bEnding)
	{
		WK_TRACE(_T("System shutdown...\n"));

		DestroyWindow();
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CWnd::OnSysCommand(nID, lParam);
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnPaint() 
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
		dc.DrawIcon(x, y, m_hIcon1);
	}
	else
	{
		CDialog::OnPaint();
	}
}

/////////////////////////////////////////////////////////////////////////////
long CSaVicUnitDlg::OnWmRequestOutputReset(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	ClearAllJobs();

	return 0;					 
}

/////////////////////////////////////////////////////////////////////////////
long CSaVicUnitDlg::OnWmRequestInputReset(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	static BOOL bFirst = TRUE;

	// Gespeicherte Alarme bei einem Serverreset löschen
	if (m_pCodec)
		m_pCodec->ResetSavedAlarmState();
	
	// Serverreset beim Start der SaVicUnit ignorieren.  
	if (!bFirst)
	{
		// Die wichtigsten Einstellungen erneut aus der Registry lesen.
		if (m_pCodec)
			m_pCodec->LoadConfig();
	}
	bFirst = FALSE;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnDestroy() 
{
	WK_TRACE(_T("OnDestroy\n"));

	CDialog::OnDestroy();
	
	ClearAllJobs();

	// Threads beenden.
	m_bRun = FALSE;
 	m_bOk  = FALSE;

	// ShutDown-Event setzen
	if (m_hShutDownEvent)
		SetEvent(m_hShutDownEvent);
	
	// Warte bis _T('PollProzessThread') beendet ist.
	if (m_pPollProzessThread){
		WaitForSingleObject(m_pPollProzessThread->m_hThread, 2000);
	}

	WK_DELETE(m_pPollProzessThread); //Autodelete = FALSE;

	if (m_bStandalone)
	{
		// Warte bis _T('SwitchThread') beendet ist.
		if (m_pSwitchThread){
			WaitForSingleObject(m_pSwitchThread->m_hThread, 2000);
		}
	
		WK_DELETE(m_pSwitchThread); //Autodelete = FALSE;
	}
	else
	{
		// Nur erforderlich, wenn Icon in der Systemtaskleiste
		NOTIFYICONDATA tnd;

		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID			= 1;
		tnd.uFlags		= 0;

		Shell_NotifyIcon(NIM_DELETE, &tnd);
	}

	WK_DELETE(m_pInput);
	WK_DELETE(m_pInputMDAlarm);
	WK_DELETE(m_pOutputSaVicCamera);
	WK_DELETE(m_pOutputSaVicRelay);

	WK_DELETE(m_pCVideoControl);
	WK_DELETE(m_pVideoInlayWindow);

	if (m_pCodec)
		m_pCodec->Close();

	WK_DELETE(m_pCodec);
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnVideoInlay() 
{
	BOOL bPrewiew =	ReadConfigurationInt(_T("MotionDetection"), _T("Activate"), 1, m_sIniFile);
	if (!m_pVideoInlayWindow)
	{
		m_pVideoInlayWindow = new CVideoInlayWindow(m_pCodec, bPrewiew);	
		// Befindet sich die Unit im MD-Mode (Kein Inlay möglich) dann werden die
		// ankommenden JPegbilder dekomprimiert und im Inlayfenster dargestellt.
		// dazu muß der encoder gestartet werden
		if (bPrewiew)
			OnEncStart();
	}
	else
	{
		if (bPrewiew)
		{
			OnEncStop();
			Sleep(500);
		}
		WK_DELETE(m_pVideoInlayWindow);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnDlgMdConfig() 
{
	if (!m_pCodec)
		return;

	if (m_bStandalone)
		m_pCodec->MotionDetectionConfiguration(m_pCodec->GetInputSource());
	else
	{
		if (Login())
			m_pCodec->MotionDetectionConfiguration(0);
	}
}


/////////////////////////////////////////////////////////////////////////////
BOOL CSaVicUnitDlg::UpdateDlg()
{
	if (!IsValid())
		return FALSE;

	// Im Zusammenhang mit der Secutity soll der Dialog nicht aktualisiert werde
	if (!m_bStandalone)
		return TRUE;

	if (!m_pCodec)
		return TRUE;

	WORD wSource	= 0;
	WORD wRelais	= 0;

	wSource				= m_pCodec->GetInputSource();

	WORD  wVideoFormat	= m_pCodec->GetVideoFormat();
	WORD  wFilter		= m_pCodec->GetFilter();
	
	WORD  wFormat		= m_pCodec->EncoderGetFormat();
	DWORD dwBPF			= m_pCodec->EncoderGetBPF();

	wRelais				= m_pCodec->GetRelais();

	WORD wFPS			= 25;

	if (m_pCodec->GetFramerate(wFPS))
		m_sFPS.Format(_T("%d"), wFPS);
			
	BOOL bFullScreen	= m_pCodec->IsDoubleFieldMode(wSource);

	if (m_pCodec)
		m_pCodec->SetLEDState((m_bLED1State ? eLED1_On : eLED1_Blink));	

	if (m_pCodec)
		m_pCodec->SetLEDState((m_bLED1State ? eLED2_On : eLED2_Off));	

	switch (wFormat)
	{
		case ENCODER_RES_HIGH:
			CheckRadioButton(IDC_HIGH_RES, IDC_LOW_RES, IDC_HIGH_RES);
			break;
		case ENCODER_RES_LOW:
			CheckRadioButton(IDC_HIGH_RES, IDC_LOW_RES, IDC_LOW_RES);
			break;
	}
	
   switch(dwBPF)
   {
		case BPF_0:
			CheckRadioButton(IDC_COMPRESS_0, IDC_COMPRESS_5, IDC_COMPRESS_0);
			break;
		case BPF_1:
			CheckRadioButton(IDC_COMPRESS_0, IDC_COMPRESS_5, IDC_COMPRESS_1);
			break;
		case BPF_2:
			CheckRadioButton(IDC_COMPRESS_0, IDC_COMPRESS_5, IDC_COMPRESS_2);
			break;
		case BPF_3:
			CheckRadioButton(IDC_COMPRESS_0, IDC_COMPRESS_5, IDC_COMPRESS_3);
			break;
		case BPF_4:
			CheckRadioButton(IDC_COMPRESS_0, IDC_COMPRESS_5, IDC_COMPRESS_4);
			break;
		case BPF_5:
			CheckRadioButton(IDC_COMPRESS_0, IDC_COMPRESS_5, IDC_COMPRESS_5);
			break;
	}

	switch(wSource)
	{
		case VIDEO_SOURCE0:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE1);
			break;
		case VIDEO_SOURCE1:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE2);
			break;
		case VIDEO_SOURCE2:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE3);
			break;
		case VIDEO_SOURCE3:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE4);
			break;
	}

	switch(wVideoFormat)
	{
	 	case VFMT_PAL_CCIR:
			CheckRadioButton(IDC_FE_PAL_CCIR, IDC_FE_NTSC_CCIR, IDC_FE_PAL_CCIR);
			break;
		case VFMT_NTSC_CCIR:
			CheckRadioButton(IDC_FE_PAL_CCIR, IDC_FE_NTSC_CCIR, IDC_FE_NTSC_CCIR);
			break;
	}

	if (wFilter & LNOTCH_FILTER)
		CheckDlgButton(IDC_FE_LNOTCH, 1);
	else
		CheckDlgButton(IDC_FE_LNOTCH, 0);

	if (wFilter & LDEC_FILTER)
		CheckDlgButton(IDC_FE_LDEC, 1);
	else
		CheckDlgButton(IDC_FE_LDEC, 0);

	
	if (wRelais & 0x01)
		CheckDlgButton(IDC_RELAIS0, 1);
	else
		CheckDlgButton(IDC_RELAIS0, 0);

	if (ReadConfigurationInt(_T("MotionDetection"), _T("Activate"), 1, m_sIniFile) == 1)
	{
		if (m_nEncoderState == ENCODER_ON)
		{
			GetDlgItem(IDC_ENC_START)->EnableWindow(FALSE);
			GetDlgItem(IDC_ENC_STOP)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_ENC_START)->EnableWindow(TRUE);
			GetDlgItem(IDC_ENC_STOP)->EnableWindow(FALSE);
		}	
		GetDlgItem(IDC_DLG_MD_CONFIG)->EnableWindow(TRUE);
		GetDlgItem(IDC_VIDEOINLAY)->SetWindowText(_T("Preview"));
		if (m_pVideoInlayWindow)
			CheckDlgButton(IDC_NOISE_REDUCTION, m_bDoNoiseReduction);
		else
			CheckDlgButton(IDC_NOISE_REDUCTION, FALSE);
	}
	else
	{
		GetDlgItem(IDC_ENC_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_ENC_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_DLG_MD_CONFIG)->EnableWindow(FALSE);
		GetDlgItem(IDC_VIDEOINLAY)->SetWindowText(_T("Video inlay"));
		CheckDlgButton(IDC_NOISE_REDUCTION, FALSE);

	}
/*
	if (ReadConfigurationInt(_T("MotionDetection"), _T("Activate"), 1, m_sIniFile) == 0)
	{
		GetDlgItem(IDC_VIDEOINLAY)->EnableWindow(TRUE);
		GetDlgItem(IDC_DLG_MD_CONFIG)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_VIDEOINLAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_DLG_MD_CONFIG)->EnableWindow(TRUE);
	}
*/	
	CheckDlgButton(IDC_ENABLE_TWO_FIELDS, bFullScreen);

	if (m_eCompressionType == COMPRESSION_JPEG)
	{
		CheckRadioButton(IDC_RADIO_JPEG, IDC_RADIO_DIB, IDC_RADIO_JPEG);
	}
	else if (m_eCompressionType == COMPRESSION_YUV_422)
	{
		CheckRadioButton(IDC_RADIO_JPEG, IDC_RADIO_DIB, IDC_RADIO_DIB);
	}

	GetDlgItem(IDC_EDIT_FPS)->SendMessage(WM_SETTEXT, 0, (LPARAM)LPCTSTR(m_sFPS));

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnDlgLuminance() 
{
	m_pCodec->DlgLuminance();
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnFePalCCIR() 
{
	if (m_pCodec)
		m_pCodec->SetVideoFormat(VFMT_PAL_CCIR);	
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnFeNtscCCIR() 
{
	if (m_pCodec)
		m_pCodec->SetVideoFormat(VFMT_NTSC_CCIR);	
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnFeLNotch() 
{
	if (m_pCodec)
		m_pCodec->SetFilter(m_pCodec->GetFilter() ^LNOTCH_FILTER);	
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnFeLDec() 
{
	if (m_pCodec)
		m_pCodec->SetFilter(m_pCodec->GetFilter() ^LDEC_FILTER);	
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnSource0() 
{
	OnSource(VIDEO_SOURCE0, VIDEO_SOURCE1);
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnSource1() 
{
	OnSource(VIDEO_SOURCE1, VIDEO_SOURCE2);
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnSource2() 
{
	OnSource(VIDEO_SOURCE2, VIDEO_SOURCE3);
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnSource3() 
{
	OnSource(VIDEO_SOURCE3, VIDEO_SOURCE0);
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnSource(int nSource, int nNextSource)
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel;
		SrcSel.wSource			= nSource;
		SrcSel.wFormat			= m_wFormat;
		SrcSel.dwBPF			= m_dwBPF;
		SrcSel.dwProzessID		= 0;
		SrcSel.dwPics			= (DWORD)-1;
   		SrcSel.wCompressionType = m_eCompressionType;

		if (m_pCodec->GetEncoderState() == ENCODER_ON)
		{
			switch(WaitForSingleObject(m_evImage, 500))
			{
				case WAIT_TIMEOUT:
					WK_TRACE(_T("OnSource Timeout\n"));
					break;
			}
		}
		else
		{
			Sleep(20);
		}

		m_evImage.ResetEvent();
		m_pCodec->SetInputSource(SrcSel);

		m_nSource = nSource;
		
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnRelais0() 
{
	if (m_pCodec)
	{
		BYTE byRelais	= m_pCodec->GetRelais();
		byRelais = byRelais^0x01;
		m_pCodec->SetRelais(byRelais);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnEncStart() 
{
	m_dwEncodedFrames	= 0;
	m_dwFrameNumber[0]	= 0;
	m_dwFrameNumber[1]	= 0;
	m_dwFrameNumber[2]	= 0;
	m_dwFrameNumber[3]	= 0;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel;
		SrcSel.wSource			= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat			= m_wFormat;
		SrcSel.dwBPF			= m_dwBPF;
		SrcSel.dwProzessID		= 0;
		SrcSel.dwPics			= (DWORD)-1;
		SrcSel.wCompressionType = m_eCompressionType;

		m_pCodec->SetInputSource(SrcSel);

		if (m_pCodec->EncoderStart())
			m_nEncoderState = ENCODER_ON;

		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnEncStop() 
{
	if (m_pCodec)
	{
		if (m_pCodec->EncoderStop())
			m_nEncoderState = ENCODER_OFF;

		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnTest()
{
	m_bOn = !m_bOn;
	if (m_bOn)
		m_dwSourceSwitch	= 0;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnCompress0() 
{
	m_dwBPF = BPF_0;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel;
		SrcSel.wSource			= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat			= m_wFormat;
		SrcSel.dwBPF			= m_dwBPF;
		SrcSel.dwProzessID		= 0;
		SrcSel.dwPics			= (DWORD)-1;
   		SrcSel.wCompressionType = m_eCompressionType;

		m_pCodec->SetInputSource(SrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnCompress1() 
{
	m_dwBPF = BPF_1;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel;
		SrcSel.wSource			= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat			= m_wFormat;
		SrcSel.dwBPF			= m_dwBPF;
		SrcSel.dwProzessID		= 0;
		SrcSel.dwPics			= (DWORD)-1;
   		SrcSel.wCompressionType = m_eCompressionType;

		m_pCodec->SetInputSource(SrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnCompress2() 
{
	m_dwBPF = BPF_2;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel;
		SrcSel.wSource			= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat			= m_wFormat;
		SrcSel.dwBPF			= m_dwBPF;
		SrcSel.dwProzessID		= 0;
		SrcSel.dwPics			= (DWORD)-1;
   		SrcSel.wCompressionType = m_eCompressionType;

		m_pCodec->SetInputSource(SrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnCompress3() 
{
	m_dwBPF = BPF_3;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel;
		SrcSel.wSource			= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat			= m_wFormat;
		SrcSel.dwBPF			= m_dwBPF;
		SrcSel.dwProzessID		= 0;
		SrcSel.dwPics			= (DWORD)-1;
   		SrcSel.wCompressionType = m_eCompressionType;

		m_pCodec->SetInputSource(SrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnCompress4() 
{
	m_dwBPF = BPF_4;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel;
		SrcSel.wSource			= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat			= m_wFormat;
		SrcSel.dwBPF			= m_dwBPF;
		SrcSel.dwProzessID		= 0;
		SrcSel.dwPics			= (DWORD)-1;
   		SrcSel.wCompressionType = m_eCompressionType;

		m_pCodec->SetInputSource(SrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnCompress5() 
{
	m_dwBPF = BPF_5;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel;
		SrcSel.wSource			= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat			= m_wFormat;
		SrcSel.dwBPF			= m_dwBPF;
		SrcSel.dwProzessID		= 0;
		SrcSel.dwPics			= (DWORD)-1;
   		SrcSel.wCompressionType = m_eCompressionType;

		m_pCodec->SetInputSource(SrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSaVicUnitDlg::SetDlgText(UINT nID, LPCTSTR pszText)
{
	// Im Zusammenhang mit der Secutity soll der Dialog nicht aktualisiert werde
	if (!m_bStandalone)
		return TRUE;

	if (!pszText)
	{
		WK_TRACE_WARNING(_T("SaVicUnitDlg::SetDlgText\tpszText=NULL\n"));
		return FALSE;
	}

	if (!IsValid())
	{
		WK_TRACE_WARNING(_T("SaVicUnitDlg::SetDlgText\tUnValid\n"));
		return FALSE;
	}

	SetDlgItemText(nID, pszText);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnHighResolution() 
{
	m_wFormat = ENCODER_RES_HIGH;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel;
		SrcSel.wSource			= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat			= m_wFormat;
		SrcSel.dwBPF			= m_dwBPF;
		SrcSel.dwProzessID		= 0;
		SrcSel.dwPics			= (DWORD)-1;
   		SrcSel.wCompressionType = m_eCompressionType;

		m_pCodec->SetInputSource(SrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnLowResolution() 
{
	m_wFormat = ENCODER_RES_LOW;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel;
		SrcSel.wSource			= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat			= m_wFormat;
		SrcSel.dwBPF			= m_dwBPF;
		SrcSel.dwProzessID		= 0;
		SrcSel.dwPics			= (DWORD)-1;
   		SrcSel.wCompressionType = m_eCompressionType;

		m_pCodec->SetInputSource(SrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
long CSaVicUnitDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM /*lParam*/)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, theApp.GetApplicationId(), 0);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnEnableDoubleFieldMode() 
{
	if (m_pCodec)
	{
		WORD wSource = m_pCodec->GetInputSource();
		BOOL bEnable = m_pCodec->IsDoubleFieldMode(wSource);
		m_pCodec->EnableDoubleFieldMode(wSource, !bEnable);

		SOURCE_SELECT_STRUCT SrcSel;
		SrcSel.wSource			= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat			= m_wFormat;
		SrcSel.dwBPF			= m_dwBPF;
		SrcSel.dwProzessID		= 0;
		SrcSel.dwPics			= (DWORD)-1;
   		SrcSel.wCompressionType = m_eCompressionType;

		m_pCodec->SetInputSource(SrcSel);
	}
}
/////////////////////////////////////////////////////////////////////////////
long CSaVicUnitDlg::DoOpenMDDlg(WPARAM wParam, LPARAM /*lParam*/)
{
	if (m_pCodec)
		m_pCodec->MotionDetectionConfiguration(wParam);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CSaVicUnitDlg::OnTrayClicked(WPARAM /*wParam*/, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MAINFRAME);
			pM = menu.GetSubMenu(0);

			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnAbout()
{
	COemGuiApi::AboutDialog(this);
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnLed1() 
{
	m_bLED1State = !m_bLED1State;

	if (m_pCodec)
	{
		m_pCodec->SetLEDState((m_bLED1State ? eLED1_On : eLED1_Blink));	
	}


}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnLed2() 
{
	m_bLED2State = !m_bLED2State;

	if (m_pCodec)
		m_pCodec->SetLEDState((m_bLED2State ? eLED2_On : eLED2_Off));	
}

/////////////////////////////////////////////////////////////////////////////
long CSaVicUnitDlg::OnWmPowerBroadcast(WPARAM wParam, LPARAM /*lParam*/)
{
	switch (wParam)
	{
		case PBT_APMBATTERYLOW:
			WK_TRACE(_T("Battery power is low\n"));
			break;
		case PBT_APMOEMEVENT:
			WK_TRACE(_T("OEM-defined event occurred.\n"));
			break;
		case PBT_APMPOWERSTATUSCHANGE:
			WK_TRACE(_T("Power status has changed.\n"));
			break;
		case PBT_APMQUERYSUSPEND:
			WK_TRACE(_T("Request for permission to suspend.\n"));
			break;
		case PBT_APMQUERYSUSPENDFAILED:
			WK_TRACE(_T("Suspension request denied.\n"));
			break;
		case PBT_APMRESUMEAUTOMATIC:
			WK_TRACE(_T("Operation resuming automatically after event.\n"));
			break;
		case PBT_APMRESUMECRITICAL:
			WK_TRACE(_T("Operation resuming after critical suspension.\n"));
			break;
		case PBT_APMRESUMESUSPEND:
			WK_TRACE(_T("Operation resuming after suspension.\n"));	
			break;
		case PBT_APMSUSPEND:
			WK_TRACE(_T("System is suspending operation.\n"));	
			OnOK();			
			break;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnChangeEditFps() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if (m_pCodec)
	{
		UpdateData();
		WORD wFps = min(max(_ttoi(m_sFPS),0), 25);
		m_pCodec->SetFramerate(wFps);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnInitPll() 
{
	if (m_pCodec)
		m_pCodec->InitPll();
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnNoiseReduction() 
{
	m_bDoNoiseReduction = !m_bDoNoiseReduction;

	if (m_bDoNoiseReduction)
		m_Jpeg.EnableNoiseReduction();
	else
		m_Jpeg.DisableNoiseReduction();
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnSelectCompressionTypeJpeg() 
{
	m_eCompressionType = COMPRESSION_JPEG;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel;
		SrcSel.wSource			= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat			= m_wFormat;
		SrcSel.dwBPF			= m_dwBPF;
		SrcSel.dwProzessID		= 0;
		SrcSel.dwPics			= (DWORD)-1;
   		SrcSel.wCompressionType = m_eCompressionType;

		m_pCodec->SetInputSource(SrcSel);
	}

}

/////////////////////////////////////////////////////////////////////////////
void CSaVicUnitDlg::OnSelectCompressionTypeDIB() 
{
	m_eCompressionType = COMPRESSION_YUV_422;	

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel;
		SrcSel.wSource			= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat			= m_wFormat;
		SrcSel.dwBPF			= m_dwBPF;
		SrcSel.dwProzessID		= 0;
		SrcSel.dwPics			= (DWORD)-1;
   		SrcSel.wCompressionType = m_eCompressionType;

		m_pCodec->SetInputSource(SrcSel);
	}

}
