/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: CoCoUnitDlg.cpp $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/CoCoUnitDlg.cpp $
// CHECKIN:		$Date: 29.11.02 8:59 $
// VERSION:		$Revision: 66 $
// LAST CHANGE:	$Modtime: 29.11.02 8:55 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wk.h"
#include "wk_file.h"
#include "win16_32.h"
#include "CMegra.h"
#include "CCoCo.h"
#include "CoCoUnitApp.h"
#include "CoCoUnitDlg.h"
#include "message.h"
#include "CipcInputCoCoUnit.h"
#include "CipcOutputCoCoUnit.h"
#include "CipcExtraMemory.h"
#include "Settings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CCoCoUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitAppDlg message handlers
BEGIN_MESSAGE_MAP(CCoCoUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CCoCoUnitDlg)
	ON_WM_PAINT()
	ON_WM_ACTIVATEAPP( )	
	ON_BN_CLICKED(IDC_BE_NTSC, OnBeNtsc)
	ON_BN_CLICKED(IDC_BE_PAL, OnBePal)
	ON_BN_CLICKED(IDC_BE_SECAM, OnBeSecam)
	ON_BN_CLICKED(IDC_DLG_SETTINGS, OnDlgSettings)
	ON_BN_CLICKED(IDC_EXTCARD0, OnExtcard0)
	ON_BN_CLICKED(IDC_EXTCARD1, OnExtcard1)
	ON_BN_CLICKED(IDC_EXTCARD2, OnExtcard2)
	ON_BN_CLICKED(IDC_EXTCARD3, OnExtcard3)
	ON_BN_CLICKED(IDC_FE_FBAS, OnFeFbas)
	ON_BN_CLICKED(IDC_FE_NTSC, OnFeNtsc)
	ON_BN_CLICKED(IDC_FE_PAL, OnFePal)
	ON_BN_CLICKED(IDC_FE_SECAM, OnFeSecam)
	ON_BN_CLICKED(IDC_FE_SVHS, OnFeSvhs)
	ON_BN_CLICKED(IDC_SOURCE1, OnSource1)
	ON_BN_CLICKED(IDC_SOURCE2, OnSource2)
	ON_BN_CLICKED(IDC_SOURCE3, OnSource3)
	ON_BN_CLICKED(IDC_SOURCE4, OnSource4)
	ON_BN_CLICKED(IDC_SOURCE5, OnSource5)
	ON_BN_CLICKED(IDC_SOURCE6, OnSource6)
	ON_BN_CLICKED(IDC_SOURCE7, OnSource7)
	ON_BN_CLICKED(IDC_SOURCE8, OnSource8)
	ON_BN_CLICKED(IDC_SEND, OnSendData)
	ON_BN_CLICKED(IDC_EXTCARDMEGRA, OnExtcardMegra)
	ON_BN_CLICKED(IDC_BE_COMPOSITE, OnBeComposite)
	ON_BN_CLICKED(IDC_BE_RGB, OnBeRgb)
	ON_BN_CLICKED(IDC_FE_LNOTCH, OnFeLNotch)
	ON_BN_CLICKED(IDC_FE_LDEC, OnFeLDec)
	ON_BN_CLICKED(IDC_EDGE7, OnEdge7)
	ON_BN_CLICKED(IDC_EDGE6, OnEdge6)
	ON_BN_CLICKED(IDC_EDGE5, OnEdge5)
	ON_BN_CLICKED(IDC_EDGE4, OnEdge4)
	ON_BN_CLICKED(IDC_EDGE3, OnEdge3)
	ON_BN_CLICKED(IDC_EDGE2, OnEdge2)
	ON_BN_CLICKED(IDC_EDGE1, OnEdge1)
	ON_BN_CLICKED(IDC_EDGE0, OnEdge0)
	ON_BN_CLICKED(IDC_ACK0, OnAck0)
	ON_BN_CLICKED(IDC_ACK1, OnAck1)
	ON_BN_CLICKED(IDC_ACK2, OnAck2)
	ON_BN_CLICKED(IDC_ACK3, OnAck3)
	ON_BN_CLICKED(IDC_ACK4, OnAck4)
	ON_BN_CLICKED(IDC_ACK5, OnAck5)
	ON_BN_CLICKED(IDC_ACK6, OnAck6)
	ON_BN_CLICKED(IDC_ACK7, OnAck7)
	ON_BN_CLICKED(IDC_RELAIS0, OnRelais0)
	ON_BN_CLICKED(IDC_RELAIS1, OnRelais1)
	ON_BN_CLICKED(IDC_RELAIS2, OnRelais2)
	ON_BN_CLICKED(IDC_RELAIS3, OnRelais3)
	ON_BN_CLICKED(IDC_SEND_MCI, OnSendMci)
	ON_BN_CLICKED(IDC_SEND_SCRIPT, OnSendScript)
	ON_BN_CLICKED(IDC_COCOID0, OnCocoid0)
	ON_BN_CLICKED(IDC_COCOID1, OnCocoid1)
	ON_BN_CLICKED(IDC_COCOID2, OnCocoid2)
	ON_BN_CLICKED(IDC_COCOID3, OnCocoid3)
	ON_BN_CLICKED(IDC_DEC_START, OnDecStart)
	ON_BN_CLICKED(IDC_DEC_STOP, OnDecStop)
	ON_BN_CLICKED(IDC_ENC_START, OnEncStart)
	ON_BN_CLICKED(IDC_ENC_STOP, OnEncStop)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_BN_CLICKED(IDC_SET_DEC_BITRATE, OnSetDecBitrate)
	ON_BN_CLICKED(IDC_SET_ENC_BITRATE, OnSetEncBitrate)
	ON_BN_CLICKED(IDC_QCIF, OnSelectQcif)
	ON_BN_CLICKED(IDC_CIF, OnSelectCif)
	ON_BN_CLICKED(IDC_COMPRESS_1, OnCompress1)
	ON_BN_CLICKED(IDC_COMPRESS_2, OnCompress2)
	ON_BN_CLICKED(IDC_COMPRESS_3, OnCompress3)
	ON_BN_CLICKED(IDC_COMPRESS_4, OnCompress4)
	ON_BN_CLICKED(IDC_COMPRESS_5, OnCompress5)
	ON_WM_DESTROY()
	ON_WM_QUERYENDSESSION()
	ON_WM_ENDSESSION()
	ON_MESSAGE(WM_COCO_CHANGED_SOURCE,			OnChangedSource)
	ON_MESSAGE(WM_COCO_CHANGED_FE_SOURCE_TYPE,	OnChangedSourceType)
	ON_MESSAGE(WM_COCO_CHANGED_FE_VIDEO_FORMAT,	OnChangedFeVideoFormat)
	ON_MESSAGE(WM_COCO_CHANGED_FE_BRIGHTNESS,	OnChangedBrightness)
	ON_MESSAGE(WM_COCO_CHANGED_FE_CONTRAST,		OnChangedContrast)
	ON_MESSAGE(WM_COCO_CHANGED_FE_SATURATION,	OnChangedSaturation)
	ON_MESSAGE(WM_COCO_CHANGED_FE_HUE,			OnChangedHue)
	ON_MESSAGE(WM_COCO_CHANGED_FE_FILTER,		OnChangedFeFilter)
	ON_MESSAGE(WM_COCO_CHANGED_FE_HSCALE,		OnChangedHScale)
	ON_MESSAGE(WM_COCO_CHANGED_FE_VSCALE,		OnChangedVScale)
	ON_MESSAGE(WM_COCO_CHANGED_BE_VIDEO_FORMAT,	OnChangedBeVideoFormat)
	ON_MESSAGE(WM_COCO_CHANGED_BE_VIDEO_TYPE,	OnChangedBeVideoType)
	ON_MESSAGE(WM_COCO_CHANGED_ENC_FORMAT,		OnChangedEncoderFormat)
	ON_MESSAGE(WM_COCO_CHANGED_ENC_FRAMERATE,	OnChangedEncoderFramerate)
	ON_MESSAGE(WM_COCO_CHANGED_ENC_BITRATE,		OnChangedEncoderBitrate)
	ON_MESSAGE(WM_COCO_CHANGED_DEC_BITRATE,		OnChangedDecoderBitrate)
	ON_MESSAGE(WM_COPYDATA,						OnWmCopyData)
	ON_MESSAGE(WM_COCO_SLEEP,					OnWmSleep)
	ON_MESSAGE(WM_COCO_CHANGED_ALARM_EDGE,		OnChangedAlarmEdge)
	ON_MESSAGE(WM_COCO_CHANGED_ALARM_ACK,		OnChangedAlarmAck)
	ON_MESSAGE(WM_COCO_ALARM,					OnAlarm)
	ON_MESSAGE(WM_COCO_DEC_START,				OnWmDecStart)
	ON_MESSAGE(WM_COCO_DEC_STOP,				OnWmDecStop)
	ON_MESSAGE(WM_COCO_ENC_START,				OnWmEncStart)
	ON_MESSAGE(WM_COCO_ENC_STOP,				OnWmEncStop)
	ON_WM_QUERYOPEN()
	ON_WM_SYSCOMMAND()
	ON_COMMAND(IDC_DLG_SETTINGS,	OnDlgSettings)
	ON_COMMAND(ID_ABOUT,			OnAbout)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SELFCHECK,					DoConfirmSelfcheck)
	ON_MESSAGE(WM_TRAYCLICKED,					OnTrayClicked)
	
END_MESSAGE_MAP()


UINT Testing(LPVOID pData)
{
	// Pointer auf's MainDialog-Objekt
	CCoCoUnitDlg*	pAppDlg	 = (CCoCoUnitDlg*)pData;
	
	// Pointer auf das CoCo-Objekt
	CCoCo* pCoCo = pAppDlg->GetCoCo();
	
	static int nSource = COCO_SOURCE0;
	
	// Warte bis MainProzeß initialisiert ist.
	while (!pAppDlg->m_bRun)
	{
		Sleep(10);
	}

	// Poll bis Mainprozeß terminiert werden soll.
	while (pAppDlg->m_bOn)
	{
		Sleep(500);
		switch (nSource)
		{
			case COCO_SOURCE0:
				pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, COCO_SOURCE0);	
				nSource = COCO_SOURCE1;
				break;
			case COCO_SOURCE1:
				pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, COCO_SOURCE1);	
				nSource = COCO_SOURCE0;
				break;

/*
			case COCO_SOURCE2:
				pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, COCO_SOURCE2);	
				nSource = COCO_SOURCE3;
				break;
			case COCO_SOURCE3:
				pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, COCO_SOURCE3);	
				nSource = COCO_SOURCE0;
				break;
*/
			default:
				break;
		}
	}
	WK_TRACE("ExitTestThread\n");
	return 0;
}

UINT EncodeThread(LPVOID pData)
{
	CCoCoUnitDlg*	pAppDlg	 = (CCoCoUnitDlg*)pData;  // Pointer auf's App-Objekt

	// Warte bis MainProzeß initialisiert ist.
	
	while (!pAppDlg->m_bRun)
	{
		Sleep(10);
	}
	
	// Poll bis Mainprozeß terminiert werden soll.
	while (pAppDlg->m_bRun)
	{
		pAppDlg->PollEncode();
	}
	WK_TRACE("ExitEncodeThread\n");
	return 0;
}

UINT DecodeThread(LPVOID pData)
{
	CCoCoUnitDlg*	pAppDlg	 = (CCoCoUnitDlg*)pData;  // Pointer auf's App-Objekt

	// Warte bis MainProzeß initialisiert ist.
	
	while (!pAppDlg->m_bRun)
	{
		Sleep(10);
	}
	
	// Poll bis Mainprozeß terminiert werden soll.
	while (pAppDlg->m_bRun)
	{
		pAppDlg->PollDecode();
	}
	
	WK_TRACE("ExitDecodeThread\n");
	return 0;
}
				  
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::DoDataExchange
void CCoCoUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCoCoUnitDlg)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::CCoCoUnitDlg
CCoCoUnitDlg::CCoCoUnitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCoCoUnitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCoCoUnitDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	// TRACE("CCoCoUnitDlg::CCoCoUnitDlg\n");

	m_bOk				= FALSE;
	m_bRun				= FALSE;
	m_bOn				= FALSE;
	m_dwReCnt			= 0L;
	m_dwTrCnt			= 0L;
	m_hIcon				= AfxGetApp()->LoadIcon(IDR_COCOUNIT);
	m_pCoCo				= NULL;
	m_pMegra			= NULL;
	m_pRFile			= NULL;
	m_pEncodeThread		= NULL;
	m_pDecodeThread		= NULL;
	m_pUser				= NULL;
	m_pPermission		= NULL;
	m_dwDecBitrate		= 256000;
	m_dwEncBitrate		= 256000;

	m_PicFormat			= RESOLUTION_LOW;	
	m_Compress			= COMPRESSION_3;
	m_byCoCoID			= COCO_COCOID0;
  	m_pCurrentEncodeJob	= NULL;
	m_pCurrentDecodeJob	= NULL;
	m_dwEncodeCounter   = 0L;
	m_hSyncSemaphore	= NULL;
	m_bNewEncodeJob		= FALSE;
	m_dwLastWMCopyDataTime = 0L;
	m_dwEncoderJobTimeout = 0L;
	m_dwWatchDogTimeout = 0L;

	for (int nI=0;nI<COCO_MAX_CAM;nI++)
		m_dwDataReceivedTime[nI] = 0L;

	for (nI = 0; nI <=7 ; nI++)
		m_pWFile[nI]		= NULL;
	
	m_pOutputMegraCamera	= NULL;
	
	// Alle Pointer initialisieren.
	for (BYTE byCoCoID = 0; byCoCoID <= MAX_COCO_ID; byCoCoID++)
	{
		m_pOutputCoCoCamera[byCoCoID]	= NULL;
		m_pOutputCoCoRelay[byCoCoID]	= NULL;
		m_pInput[byCoCoID]				= NULL;
	}

	//m_bStandalone = 0-> CoCoUnit arbeitet mit Security, 1-> CoCoUnit ist Standalone
	m_bStandalone = GetPrivateProfileInt("COCOISA", "Mode",  0, COCO_INI);
	m_dwWatchDogTimeout   = 1000 * GetPrivateProfileInt("COCOISA", "TimeOut",  120, COCO_INI);
    m_dwEncoderJobTimeout = GetPrivateProfileInt("COCOISA", "JobTimeOut",  2000, COCO_INI);	
	
	// Als Standalone CFileobjekte zum Laden und Speichern von H261 Dateien anlegen
	if (m_bStandalone)
	{
		// ReadFile
		m_pRFile = new CFile;
		if (!m_pRFile)
			return;

		// WriteFile
		for (nI = 0; nI <= 7; nI++)
		{
			m_pWFile[nI] = new CFile;
			if (!m_pWFile[nI])
				return;
		}
	}

#if 0
	// NEW HEDU
	// NOT in util.h, hidden global to enable WK_STAT_LOG
	// with ms resolution
	extern DWORD WK_STAT_TICK_COUNT;
	WK_STAT_TICK_COUNT = GetTickCount();
#endif

	// Encode-Thread
	m_pEncodeThread = AfxBeginThread(EncodeThread, this);
	m_pEncodeThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
	m_pEncodeThread->m_bAutoDelete = FALSE;

	// Decode-Thread
	m_pDecodeThread = AfxBeginThread(DecodeThread, this);
	m_pDecodeThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
	m_pDecodeThread->m_bAutoDelete = FALSE;

}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::~CCoCoUnitDlg
CCoCoUnitDlg::~CCoCoUnitDlg()
{
// 	TRACE("CCoCoUnitDlg::~CCoCoUnitDlg\n");

	if (m_bStandalone)
	{
		WK_DELETE(m_pRFile);
		for (int nI = 0; nI <= 7; nI++)
			WK_DELETE(m_pWFile[nI]);
	}

	WK_DELETE(m_pUser);
	WK_DELETE(m_pPermission);

//	m_bOk  = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::PostNcDestroy
void CCoCoUnitDlg::PostNcDestroy() 
{
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCoCoUnitDlg::Create()
{
	if (!CDialog::Create(IDD))
	{
		return FALSE;
	}

	// dialog ist da, fenster handle ist gültig, 
	// OnInitDialog ist durchlaufen !!
	BYTE	byCoCoID		= 0;
	DWORD	dwHardwareState = 0L;	  
	DWORD	dwBit			= 0L;
	char	szSMName[128];

//	TRACE("CCoCoUnitDlg::Create\n");
	
	// Megra-Objekt anlegen.
	m_pMegra = new CMegra(m_hWnd);

	// Konnte Megraobjekt angelegt werden ?
	if (!m_pMegra)
	{
		WK_TRACE_ERROR("Megra-Objekt konnte nicht angelegt werden\n");

		return FALSE;
	}

	// Ist Megraobjekt korrekt initialisiert worden ?
	if (!m_pMegra->IsValid())
	{
		CString sError;
		sError.LoadString(IDS_RTE_MEGRA_INIT);
	
		WK_TRACE_WARNING(sError);

		// Der CoCo läuft nun auch ohne Megra (Softwaredekoding macht's möglich)
		// CWK_RunTimeError RTerr(WAI_COCOUNIT, REL_ERROR, RTE_CONFIGURATION, sError, 0, 0);
		// RTerr.Send();
	}

	if (m_pMegra)
	{
		CRect rect(0,0,0,0);
		m_pMegra->SetDisplayWindow((const LPRECT)&rect);
	}

	// CoCo-Objekt anlegen
	m_pCoCo = new CCoCo(m_hWnd, COCO_INI);
	
	if (!m_pCoCo)
	{
		WK_TRACE_ERROR("CCoCoUnitDlg::Create\tCCoCo-Objekt konnte nicht angelegt werden.\n");
		return FALSE;
	}		

	if (!m_pCoCo->IsValid())
	{
		WK_TRACE_ERROR("CCoCoUnitDlg::Create\tCCoCo-Objekt konnte nicht korrekt initialisiert werden.\n");
		WK_DELETE(m_pCoCo);
		return FALSE;
	}		
	if (!m_bStandalone)
	{
		// Wenn das Megraobjekt angelegt werden konnte, dann eine CIPC-Schnittstelle aufbauen 
		// Die Megra muß, wenn vorhanden immer am 1. CoCo 'hängen'
		if (m_pMegra)
		{
			m_pOutputMegraCamera = new CIPCOutputCoCoUnit(this, m_pCoCo, m_pMegra, COCO_COCOID0, OUTPUT_TYPE_MEGRA_CAMERA, "CoCoUnitOutputCameraMegra");
			if (!m_pOutputMegraCamera)
				WK_TRACE_ERROR("CCoCoUnitDlg::Create\tCIPCOutputCoCoUnit-MegraCamera-Objekt konnte nicht angelegt werden.\n");
		}

		// Hardwarekonfigureation abfragen und für jede 'Einheit' (1xCoCo, 1...3xSchwester)
		// die CIPC-Schnittstellen aufbauen.
		dwHardwareState = m_pCoCo->GetState();
		dwBit = 1L;
		for (byCoCoID = 0; byCoCoID <= MAX_COCO_ID; byCoCoID++)
		{
			if (dwHardwareState & dwBit)
			{
				wsprintf(szSMName, "CoCoUnitInput%u",(WORD)byCoCoID);
				m_pInput[byCoCoID] = new CIPCInputCoCoUnit(m_pCoCo, byCoCoID, szSMName);
				if (!m_pInput[byCoCoID])
					WK_TRACE_ERROR("CCoCoUnitDlg::Create\tCIPCInputCoCoUnit-Objekt konnte nicht angelegt werden.\n");

				wsprintf(szSMName, "CoCoUnitOutputCameraCoCo%u",(WORD)byCoCoID);
				m_pOutputCoCoCamera[byCoCoID] = new CIPCOutputCoCoUnit(this, m_pCoCo, m_pMegra, byCoCoID, OUTPUT_TYPE_COCO_CAMERA, szSMName);
				if (!m_pOutputCoCoCamera[byCoCoID])
					WK_TRACE_ERROR("CCoCoUnitDlg::Create\tCIPCOutputCoCoUnit-CoCoCamera-Objekt konnte nicht angelegt werden.\n");
				
				wsprintf(szSMName, "CoCoUnitOutputRelay%u",(WORD)byCoCoID);
				m_pOutputCoCoRelay[byCoCoID] = new CIPCOutputCoCoUnit(this, m_pCoCo, m_pMegra, byCoCoID, OUTPUT_TYPE_COCO_RELAY, szSMName);
				if (!m_pOutputCoCoRelay[byCoCoID])
					WK_TRACE_ERROR("CCoCoUnitDlg::Create\tCIPCOutputCoCoUnit-Relay-Objekt konnte nicht angelegt werden.\n");	
			}

			if (!m_bStandalone) {
				CString sEventName;
				sEventName.Format("CoCoSyncEvent");
// ML 19.3.99   m_hSyncEvent = CreateEvent(NULL, TRUE, FALSE, sEventName);
				m_hSyncSemaphore = CreateSemaphore(NULL, 0, 10, sEventName);
				if (m_hSyncSemaphore==NULL) {
					WK_TRACE_ERROR("Failed to create sync event\n");
				}
			}

			dwBit <<=4;
		}
	}
	
	if (m_bStandalone)
	{
		ShowWindow(SW_SHOW);
	}
	else
	{
		// Icon ins Systemtray
		NOTIFYICONDATA tnd;

		CString sTip = COemGuiApi::GetApplicationName(WAI_COCOUNIT);

		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID			= 1;

		tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
		tnd.uCallbackMessage = WM_TRAYCLICKED;
		tnd.hIcon		= theApp.LoadIcon(IDR_COCOUNIT);

		lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
		Shell_NotifyIcon(NIM_ADD, &tnd);
	}

	char	byBitrate[255];

	itoa(m_dwDecBitrate, byBitrate, 10);
	SetDlgText(IDC_DEC_BITRATE, byBitrate);

	itoa(m_dwEncBitrate, byBitrate, 10);
	SetDlgText(IDC_ENC_BITRATE, byBitrate);
					   
	m_pCoCo->DecoderSetBitrate(0, m_dwDecBitrate);
	m_pCoCo->EncoderSetBitrate(0, m_dwEncBitrate);

	m_bOk  = TRUE;
	m_bRun = TRUE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::ClearAllJobs
void CCoCoUnitDlg::ClearAllJobs()
{
	ClearAllEncodeJobs();
	ClearAllDecodeJobs();
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::ClearAllEncodeJobs
void CCoCoUnitDlg::ClearAllEncodeJobs()
{
	ClearCurrentEncodeJob("ClearAllEncodeJobs");
	m_jobQueueEncode.SafeDeleteAll();
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::ClearCurrentEncodeJob
void CCoCoUnitDlg::ClearCurrentEncodeJob(const CString &s)
{
	m_csEncode.Lock();
	if (s == "CancelCurrentEncodeJob" && m_pCurrentEncodeJob)
	{
		WK_TRACE("ClearCurrentEncodeJob\t%s CamID=%d OutPic=%d\n", (LPCSTR)s,
										m_pCurrentEncodeJob->m_camID.GetSubID(),
										m_pCurrentEncodeJob->m_iOutstandingPics);
	}
	WK_DELETE(m_pCurrentEncodeJob);
		
	m_csEncode.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::ClearAllDecodeJobs
void CCoCoUnitDlg::ClearAllDecodeJobs()
{
	ClearCurrentDecodeJob();
	m_jobQueueDecode.SafeDeleteAll();
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::ClearCurrentDecodeJob
void CCoCoUnitDlg::ClearCurrentDecodeJob()
{
	m_csDecode.Lock();
	WK_DELETE(m_pCurrentDecodeJob);
//	WK_TRACE("ClearCurrentDecodeJob\n");
	m_csDecode.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::Dump
void CCoCoUnitDlg::Dump( CDumpContext &dc ) const
{
	 CObject::Dump( dc );
	 dc << "CCoCoUnitDlg = ";
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::IsValid
BOOL CCoCoUnitDlg::IsValid()
{
	return m_bOk;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::Login
BOOL CCoCoUnitDlg::Login()
{
	CString sUser;
	CString sPassword;
	CSecID  idUser;
	CUserArray UserArray;
	BOOL bRet = FALSE;			 
	CWK_Profile prof;

	COEMLoginDialog dlg(this);
										   
	if (IDOK==dlg.DoModal())	  		 
	{									 
		CUser user	= dlg.GetUser();
		bRet = TRUE;		 
	} 			  

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::PollEncode
void CCoCoUnitDlg::PollEncode()
{
#if (1)
	WaitForSingleObject(m_jobQueueEncode.GetSemaphore(), 100);
#else
	Sleep(5);
#endif

	// Übernimmt die Prozeßbehandlung aller Encoderjobs
	ProzessEncodeJobs();

	// Wenn der Encoder läuft, aber keine Daten mehr eintreffen, die CoCoUnit
	// neu starten. m_dwDataReceivedTime wird beim Empfang der Message 'WM_COPYDATA'
	// gesetzt.
	if (m_pCoCo && (m_pCoCo->EncoderGetState(0) == ENCODER_ON)) // OOPS prüft nur CoCo 0
	{
		if (m_jobQueueEncode.GetCount() > 0)
		{
			EncoderTimeOutProzessing();
		}

		// Kommen überhaupt noch Daten vom CoCoSrv.exe 
		if (GetTickCount() > m_dwLastWMCopyDataTime + m_dwWatchDogTimeout)
		{
			WK_TRACE_WARNING("Encoder timeout exiting CocoUnit, hoping for restart by Launcher\n");
			Sleep(500);	// Zeit zum Logfile schreiben 
			exit(0);
		}								 
	}
	else
	{	
		m_dwLastWMCopyDataTime = GetTickCount();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::PollDecode
void CCoCoUnitDlg::PollDecode()
{
	if (m_bStandalone)
	{
		// Übernimmt den Transport zum Decoder und fordert Daten vom Encoder an
		VideoDataService();
		Sleep(10);
	}
	else
	{
#if (1)
		WaitForSingleObject(m_jobQueueDecode.GetSemaphore(), 100);
#else
		Sleep(10);
#endif
		// Übernimmt die Prozeßbehandlung aller Decoderjobs
		ProzessDecodeJobs();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::ProzessEncodeJobs
void CCoCoUnitDlg::ProzessEncodeJobs()
{
	static DWORD dwLastJobTime = GetTickCount();
	static BOOL	 bOnlyOnce = FALSE;

	m_csEncode.Lock();

	// no current job
	if (m_pCurrentEncodeJob == NULL)
	{
		// ############ Encoder Jobs
		m_pCurrentEncodeJob = m_jobQueueEncode.SafeGetAndRemoveHead();
		if (m_pCurrentEncodeJob)
		{
			int iSubID = m_pCurrentEncodeJob->m_camID.GetSubID();
			CIPCOutputCoCoUnit* pOutput = (CIPCOutputCoCoUnit*)m_pCurrentEncodeJob->m_pCipc;

			switch (m_pCurrentEncodeJob->m_action)
			{
				case VJA_ENCODE:
			//		WK_TRACE("ProzessEncodeJobs\tEncoderStart\tCamID=%d\tPicts=%d\n",
			//			m_pCurrentEncodeJob->m_camID.GetSubID(), m_pCurrentEncodeJob->m_iOutstandingPics);
					m_bNewEncodeJob = TRUE;
					bOnlyOnce		=  TRUE;
					dwLastJobTime	=  GetTickCount();
					
					if (pOutput)
						pOutput->EncoderStart(m_pCurrentEncodeJob);
					if ( (iSubID >= 0) && (iSubID < COCO_MAX_CAM))
					{
						m_dwDataReceivedTime[iSubID] = GetTickCount();
					}
					break;
				case VJA_STOP_ENCODE:
//					WK_TRACE("ProzessEncodeJobs\tEncoderStop\n");
					if (pOutput)
						pOutput->EncoderStop(m_pCurrentEncodeJob);
					//	Der Job wird erst in OnWmEncodeJob gelöscht.
// 					ClearCurrentEncodeJob("VJA_STOP_ENCODE"); // job done
					break;
				case VJA_VIDEO:
//					WK_TRACE("ProzessEncodeJobs\tLocalVideo\n");
					if (pOutput)
						pOutput->LocalVideo(m_pCurrentEncodeJob);
					ClearCurrentEncodeJob("VJA_VIDEO"); // job done
					break;
				case VJA_SYNC:	// NEW HEDU
					WK_TRACE("ProzessEncodeJobs\tSyncEvent %d\n",GetTickCount());

					if (m_hSyncSemaphore)
					{
// ML 18.3.99			SetEvent(m_hSyncEvent);
						ReleaseSemaphore(m_hSyncSemaphore, 1, NULL);
					}
					else
					{
						static BOOL bNoSyncDone=FALSE;	
						if (bNoSyncDone==FALSE)
						{	// msg only once
							WK_TRACE_WARNING("No Sync event\n");
						}
						bNoSyncDone=TRUE;
					}
					ClearCurrentEncodeJob("VJA_SYNC"); // job done
					break;
				default:	
					WK_TRACE_ERROR("Invalid Encode job type\n"); 
					ClearCurrentEncodeJob("Invalid Encode job"); // job done
			}	// end of job switch
		}	// end of job found
		else
		{
			// Wenn 30 Sekunden kein neuer Job kommt, Encoder stoppen
			if (GetTickCount() -  dwLastJobTime > 30000)
			{
				if (bOnlyOnce)
				{
					if (m_pOutputCoCoCamera[0])
					{
						m_pOutputCoCoCamera[0]->EncoderStop(NULL); // OOPS ML nur CoCo 0
						WK_TRACE("JobQueue empty\tEncoder stoped\tCurrentJob=%lu\n", (DWORD)m_pCurrentEncodeJob);
						bOnlyOnce = FALSE;
					}
				}
				dwLastJobTime = GetTickCount();
			}
		}
	}
	m_csEncode.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoUnitDlg::EncoderTimeOutProzessing()
{
	m_csEncode.Lock();

	if (m_pCurrentEncodeJob == NULL)
	{
		m_csEncode.Unlock();
		return;
	}

	if (m_pCurrentEncodeJob->m_action != VJA_ENCODE)
	{
		m_csEncode.Unlock();
		return;
	}

	int nCamID = m_pCurrentEncodeJob->m_camID.GetSubID();
	if ((nCamID<0) || (nCamID >= COCO_MAX_CAM) )
	{
		WK_TRACE_ERROR("EncoderTimeOutProzessing camSubID=%d out of range",nCamID);
		m_csEncode.Unlock();
		return;
	}

	DWORD dwCurTC = GetTickCount();
	DWORD dwDataReceivedTime = m_dwDataReceivedTime[nCamID];

	if (dwCurTC - dwDataReceivedTime > m_dwEncoderJobTimeout)
	{
		// this should indicate an inoperational camera
		WK_TRACE_WARNING("Encoder timeout=%ld, queue len=%d, cam=%lx\n",
			dwCurTC - dwDataReceivedTime, m_jobQueueEncode.GetCount(), m_pCurrentEncodeJob->m_camID);
		
		m_dwDataReceivedTime[nCamID] = dwCurTC;
		ClearCurrentEncodeJob("EncoderTimeOutProzessing");	// job done 
	}
	m_csEncode.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::ProzessDecodeJobs
void CCoCoUnitDlg::ProzessDecodeJobs()
{
	m_csDecode.Lock();

	// ############ Decoder Jobs
	if (m_pCurrentDecodeJob == NULL)
	{
		m_pCurrentDecodeJob = m_jobQueueDecode.SafeGetAndRemoveHead();
		if (m_pCurrentDecodeJob)
		{
			CIPCOutputCoCoUnit*	pOutput = (CIPCOutputCoCoUnit*)m_pCurrentDecodeJob->m_pCipc;

			switch (m_pCurrentDecodeJob->m_action)
			{
				case VJA_DECODE:
//					WK_TRACE("ProzessDecodeJobs\tDecoderStart\tCount=%lu\n", m_jobQueueDecode.GetCount());
					if (pOutput)
						pOutput->DecoderStart(m_pCurrentDecodeJob, m_jobQueueDecode.GetCount());
					ClearCurrentDecodeJob(); // job done
					break;
				case VJA_STOP_DECODE:
//					WK_TRACE("ProzessDecodeJobs\tDecoderStop\n");
					if (pOutput)
						pOutput->DecoderStop(m_pCurrentDecodeJob);
					ClearCurrentDecodeJob(); // job done
					break;
//				case VJA_VIDEO:
//					WK_TRACE("ProzessDecodeJobs\tLocalVideo\n");
//					if (pOutput)
//						pOutput->LocalVideo(m_pCurrentDecodeJob);
//					ClearCurrentDecodeJob(); // job done
//					break;
				default:
					WK_TRACE_ERROR("Invalid Decode job type\n");
					ClearCurrentDecodeJob(); // job done
					break;
			}	// end of job switch
		}	// end of job found
	}

	m_csDecode.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::VideoDataService
void CCoCoUnitDlg::VideoDataService()
{
	#define B_SIZE 16000
	static DWORD dwLastTime = GetCurrentTime();
	static BYTE szBuffer[B_SIZE+2];
	static char szText[16];
	static PARAMSTRUCT Param;
	DWORD  dwCurrentTime;
	DWORD  dwDelta		= 0;
	DWORD  dwLen		= 0;
	DWORD  dwBufferSize	= 0;

	if (!IsValid())
		return;

	dwCurrentTime	= GetCurrentTime();
	dwDelta			= dwCurrentTime - dwLastTime;
	dwLastTime		= dwCurrentTime;

	dwBufferSize = B_SIZE;

	// Alle CoCo's bedienen
	for (BYTE byCoCoID = 0; byCoCoID <= MAX_COCO_ID; byCoCoID++)
	{
		// CoCo vorhanden ?
		if (m_pCoCo && TSTBIT(m_pCoCo->GetState(), byCoCoID*4))
		{
			// Decoder an ?
			if (m_pCoCo && (m_pCoCo->DecoderGetState(byCoCoID) != DECODER_OFF))
			{
				if (m_pRFile)
					dwLen = m_pRFile->Read(szBuffer, dwBufferSize);

				if (dwLen != 0L)
				{
					PARAMSTRUCT Param;

					// Komprimierte Bilddaten verschicken
					Param.dwData	= COCO_DECODER_DATA;
					Param.lpData	= (LPCSTR)szBuffer;
					Param.dwLen		= dwLen;					 
					Param.wCoCoID	= (WORD)byCoCoID;
					Param.wIntra	= 0;
					DWORD dwCnt		= 0L;
					do{
						if (m_pCoCo && m_pCoCo->WriteData(Param))
						{
							dwCnt += Param.dwLen;
							Param.lpData = (LPCSTR)&szBuffer[dwCnt];
							Param.dwLen  = dwLen - dwCnt;
						}
 						if (m_pCoCo && (m_pCoCo->DecoderGetState(byCoCoID) == DECODER_OFF))
							break;
						if (!m_bRun)
							break;
						Sleep(5);	// Wegen der Performance
					}
					while (dwCnt != dwLen);

					m_dwTrCnt +=dwLen;
					wsprintf(szText, "%lu", m_dwTrCnt);
					SetDlgText(IDC_TR_DATA, szText);
				}					
				else if (m_pCoCo && (m_pCoCo->DecoderGetState(byCoCoID) == DECODER_ON))
					OnDecStop();
			}
		}
	}
	return;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnInitDialog
BOOL CCoCoUnitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	EnableToolTips(TRUE);

	if (m_bStandalone)
	{
		CenterWindow();
		// only for standalone
		UpdateDlg();
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnActivateApp
void CCoCoUnitDlg::OnActivateApp(BOOL bActivate, DWORD dwThreadID)
{
//	if (bActivate)
//		m_pCoCo->Activate(COCO_ACT_ALL);
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnSendData
void CCoCoUnitDlg::OnSendData() 
{
#define SIZE 255
	PARAMSTRUCT Param;

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SEND_DATA);
	HGLOBAL hMem  = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, SIZE);
	LPSTR	lpBuf =	(LPSTR)GlobalLock(hMem);

	if (!pEdit)
		return;

	if (lpBuf)
	{
		int nLen = pEdit->GetWindowText(lpBuf, SIZE) + 1;

		if (nLen > 0)
		{
			Param.dwData	= COCO_TEST_DATA;
			Param.lpData	= lpBuf;
			Param.dwLen		= (DWORD)nLen;
			Param.wCoCoID	= COCO_COCOID_CURRENT;
			
			if (m_pCoCo)
				m_pCoCo->WriteData(Param);
		}

		GlobalUnlock(hMem);
		GlobalFree(hMem);
	}
}

void CCoCoUnitDlg::OnSendMci() 
{
#define SIZE 255

	PARAMSTRUCT	Param;
    CEdit*		pEdit = (CEdit*)GetDlgItem(IDC_EDIT_SEND_MCI);
	HGLOBAL		hMem  = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, SIZE);
	LPSTR		lpBuf =	(LPSTR)GlobalLock(hMem);

	if (!pEdit)
		return;

	if (lpBuf)
	{
	    int nLen = pEdit->GetWindowText(lpBuf, SIZE) + 1;

		if (nLen > 0)
		{
			Param.dwData	= COCO_MCI_DATA;
			Param.lpData	= lpBuf;
			Param.dwLen		= (DWORD)nLen;
			Param.wCoCoID	= COCO_COCOID_CURRENT;
			
			if (m_pCoCo)
				m_pCoCo->WriteData(Param);
		}

		GlobalUnlock(hMem);
		GlobalFree(hMem);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::UpdateDlg
BOOL CCoCoUnitDlg::UpdateDlg()
{
	char szBuffer[255];

	if (!IsValid())
		return FALSE;

	// Im Zusammenhang mit der Secutity soll der Dialog nicht aktualisiert werde
	if (!m_bStandalone)
		return TRUE;

	if (!m_pCoCo)
		return FALSE;

	DWORD dwTemp 		= m_pCoCo->GetInputSource(COCO_COCOID_CURRENT);
	BYTE  byCoCoID		= m_pCoCo->GetCoCoID();
	WORD  wFeVideoFormat= m_pCoCo->GetFeVideoFormat();
	WORD  wFeSourceType = m_pCoCo->GetFeSourceType();
	WORD  wBeVideoFormat= m_pCoCo->GetBeVideoFormat();
	WORD  wBeVideoType  = m_pCoCo->GetBeVideoType();
	WORD  wFeFilter		= m_pCoCo->GetFeFilter();
	WORD  wSource 		= LOWORD(dwTemp);
	WORD  wCard   		= HIWORD(dwTemp);
	
	WORD  wEdge			= m_pCoCo->GetAlarmEdge(COCO_COCOID_CURRENT, wCard);
	WORD  wAck			= m_pCoCo->GetAlarmAck(COCO_COCOID_CURRENT, wCard);
	WORD  wRelais		= m_pCoCo->GetRelais(COCO_COCOID_CURRENT, wCard);
	WORD  wFormat		= m_pCoCo->EncoderGetFormat(COCO_COCOID_CURRENT);

	m_dwEncBitrate		= m_pCoCo->EncoderGetBitrate(COCO_COCOID_CURRENT);
	m_dwDecBitrate		= m_pCoCo->DecoderGetBitrate(COCO_COCOID_CURRENT);

	// Aktuelle Encoderbitrate setzen
	wsprintf(szBuffer,"%lu", m_dwEncBitrate);
	SetDlgItemText(IDC_ENC_BITRATE, szBuffer);

	// Aktuelle Decoderbitrate setzen
	wsprintf(szBuffer,"%lu", m_dwDecBitrate);
	SetDlgItemText(IDC_DEC_BITRATE, szBuffer);
	
	switch (wFormat)
	{
		case COCO_ENCODER_QCIF:
			CheckRadioButton(IDC_QCIF, IDC_CIF, IDC_QCIF);
			break;
		case COCO_ENCODER_CIF:
			CheckRadioButton(IDC_QCIF, IDC_CIF, IDC_CIF);
			break;
	}
	
   switch(m_Compress)
   {
		case COMPRESSION_1:
			CheckRadioButton(IDC_COMPRESS_1, IDC_COMPRESS_5, IDC_COMPRESS_1);
			break;
		case COMPRESSION_2:
			CheckRadioButton(IDC_COMPRESS_1, IDC_COMPRESS_5, IDC_COMPRESS_2);
			break;
		case COMPRESSION_3:
			CheckRadioButton(IDC_COMPRESS_1, IDC_COMPRESS_5, IDC_COMPRESS_3);
			break;
		case COMPRESSION_4:
			CheckRadioButton(IDC_COMPRESS_1, IDC_COMPRESS_5, IDC_COMPRESS_4);
			break;
		case COMPRESSION_5:
			CheckRadioButton(IDC_COMPRESS_1, IDC_COMPRESS_5, IDC_COMPRESS_5);
			break;
	}
	switch(wSource)
	{
		case COCO_SOURCE0:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE1);
			break;
		case COCO_SOURCE1:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE2);
			break;
		case COCO_SOURCE2:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE3);
			break;
		case COCO_SOURCE3:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE4);
			break;
		case COCO_SOURCE4:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE5);
			break;
		case COCO_SOURCE5:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE6);
			break;
		case COCO_SOURCE6:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE7);
			break;
		case COCO_SOURCE7:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE8);
			break;
	}

	switch(byCoCoID)
	{
		case COCO_COCOID0:
			CheckRadioButton(IDC_COCOID0, IDC_COCOID3, IDC_COCOID0);
			break;
		case COCO_COCOID1:
			CheckRadioButton(IDC_COCOID0, IDC_COCOID3, IDC_COCOID1);
			break;
		case COCO_COCOID2:
			CheckRadioButton(IDC_COCOID0, IDC_COCOID3, IDC_COCOID2);
			break;
		case COCO_COCOID3:
			CheckRadioButton(IDC_COCOID0, IDC_COCOID3, IDC_COCOID3);
			break;
	}

	switch(wCard)
	{
		case COCO_EXTCARD0:
			CheckRadioButton(IDC_EXTCARD0, IDC_EXTCARDMEGRA, IDC_EXTCARD0);
			break;
		case COCO_EXTCARD1:
			CheckRadioButton(IDC_EXTCARD0, IDC_EXTCARDMEGRA, IDC_EXTCARD1);
			break;
		case COCO_EXTCARD2:
			CheckRadioButton(IDC_EXTCARD0, IDC_EXTCARDMEGRA, IDC_EXTCARD2);
			break;
		case COCO_EXTCARD3:
			CheckRadioButton(IDC_EXTCARD0, IDC_EXTCARDMEGRA, IDC_EXTCARD3);
			break;
		case COCO_EXTCARD_MEGRA:
			CheckRadioButton(IDC_EXTCARD0, IDC_EXTCARDMEGRA, IDC_EXTCARDMEGRA);
			break;

	}

	switch(wFeVideoFormat)
	{
	 	case COCO_PAL:
			CheckRadioButton(IDC_FE_PAL, IDC_FE_NTSC, IDC_FE_PAL);
			break;
		case COCO_SECAM:
			CheckRadioButton(IDC_FE_PAL, IDC_FE_NTSC, IDC_FE_SECAM);
			break;
		case COCO_NTSC:
			CheckRadioButton(IDC_FE_PAL, IDC_FE_NTSC, IDC_FE_NTSC);
			break;
	}

	switch(wFeSourceType)
	{
	 	case COCO_FBAS:
			CheckRadioButton(IDC_FE_FBAS, IDC_FE_SVHS, IDC_FE_FBAS);
			break;
		case COCO_SVHS:
			CheckRadioButton(IDC_FE_FBAS, IDC_FE_SVHS, IDC_FE_SVHS);
			break;
	}

	switch(wBeVideoFormat)
	{
	 	case COCO_PAL:
			CheckRadioButton(IDC_BE_PAL, IDC_BE_NTSC, IDC_BE_PAL);
			break;
		case COCO_SECAM:
			CheckRadioButton(IDC_BE_PAL, IDC_BE_NTSC, IDC_BE_SECAM);
			break;
		case COCO_NTSC:
			CheckRadioButton(IDC_BE_PAL, IDC_BE_NTSC, IDC_BE_NTSC);
			break;
	}

	switch(wBeVideoType)
	{	
		case COCO_COMPOSITE:
			CheckRadioButton(IDC_BE_COMPOSITE, IDC_BE_RGB, IDC_BE_COMPOSITE);
			break;
	 	case COCO_RGB:
			CheckRadioButton(IDC_BE_COMPOSITE, IDC_BE_RGB, IDC_BE_RGB);
			break;
	}

	if (wFeFilter & COCO_LNOTCH)
		CheckDlgButton(IDC_FE_LNOTCH, 1);
	else
		CheckDlgButton(IDC_FE_LNOTCH, 0);

	if (wFeFilter & COCO_LDEC)
		CheckDlgButton(IDC_FE_LDEC, 1);
	else
		CheckDlgButton(IDC_FE_LDEC, 0);

	
	if (wEdge & 0x01)
		CheckDlgButton(IDC_EDGE0, 1);
	else
		CheckDlgButton(IDC_EDGE0, 0);

	if (wEdge & 0x02)
		CheckDlgButton(IDC_EDGE1, 1);
	else
		CheckDlgButton(IDC_EDGE1, 0);

	if (wEdge & 0x04)
		CheckDlgButton(IDC_EDGE2, 1);
	else
		CheckDlgButton(IDC_EDGE2, 0);

	if (wEdge & 0x08)
		CheckDlgButton(IDC_EDGE3, 1);
	else
		CheckDlgButton(IDC_EDGE3, 0);

	if (wEdge & 0x10)
		CheckDlgButton(IDC_EDGE4, 1);
	else
		CheckDlgButton(IDC_EDGE4, 0);

	if (wEdge & 0x20)
		CheckDlgButton(IDC_EDGE5, 1);
	else
		CheckDlgButton(IDC_EDGE5, 0);

	if (wEdge & 0x40)
		CheckDlgButton(IDC_EDGE6, 1);
	else
		CheckDlgButton(IDC_EDGE6, 0);

	if (wEdge & 0x80)
		CheckDlgButton(IDC_EDGE7, 1);
	else
		CheckDlgButton(IDC_EDGE7, 0);


	if (wAck & 0x01)
		CheckDlgButton(IDC_ACK0, 1);
	else
		CheckDlgButton(IDC_ACK0, 0);

	if (wAck & 0x02)
		CheckDlgButton(IDC_ACK1, 1);
	else
		CheckDlgButton(IDC_ACK1, 0);

	if (wAck & 0x04)
		CheckDlgButton(IDC_ACK2, 1);
	else
		CheckDlgButton(IDC_ACK2, 0);

	if (wAck & 0x08)
		CheckDlgButton(IDC_ACK3, 1);
	else
		CheckDlgButton(IDC_ACK3, 0);

	if (wAck & 0x10)
		CheckDlgButton(IDC_ACK4, 1);
	else
		CheckDlgButton(IDC_ACK4, 0);

	if (wAck & 0x20)
		CheckDlgButton(IDC_ACK5, 1);
	else
		CheckDlgButton(IDC_ACK5, 0);

	if (wAck & 0x40)
		CheckDlgButton(IDC_ACK6, 1);
	else
		CheckDlgButton(IDC_ACK6, 0);

	if (wAck & 0x80)
		CheckDlgButton(IDC_ACK7, 1);
	else
		CheckDlgButton(IDC_ACK7, 0);


	if (wRelais & 0x01)
		CheckDlgButton(IDC_RELAIS0, 1);
	else
		CheckDlgButton(IDC_RELAIS0, 0);

	if (wRelais & 0x02)
		CheckDlgButton(IDC_RELAIS1, 1);
	else
		CheckDlgButton(IDC_RELAIS1, 0);

	if (wRelais & 0x04)
		CheckDlgButton(IDC_RELAIS2, 1);
	else
		CheckDlgButton(IDC_RELAIS2, 0);
	
	if (wRelais & 0x08)
		CheckDlgButton(IDC_RELAIS3, 1);
	else
		CheckDlgButton(IDC_RELAIS3, 0);

	DWORD dwState = m_pCoCo->GetState();
	DWORD dwBit   = (DWORD)(byCoCoID)*4;

	if (TSTBIT(dwState,dwBit))
		GetDlgItem(IDC_EXTCARD0)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EXTCARD0)->EnableWindow(FALSE);

	dwBit++;
	
	if (TSTBIT(dwState,dwBit))
		GetDlgItem(IDC_EXTCARD1)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EXTCARD1)->EnableWindow(FALSE);
	
	dwBit++;

	if (TSTBIT(dwState,dwBit))
		GetDlgItem(IDC_EXTCARD2)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EXTCARD2)->EnableWindow(FALSE);

	dwBit++;

	if (TSTBIT(dwState,dwBit))
		GetDlgItem(IDC_EXTCARD3)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EXTCARD3)->EnableWindow(FALSE);
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnDestroy
void CCoCoUnitDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	// PollThread beenden.
	m_bRun = FALSE;
 	m_bOk  = FALSE;

	// Warte bis 'EncodeThread' beendet ist.
	if (m_pEncodeThread)
		WaitForSingleObject(m_pEncodeThread->m_hThread, 10000);

	WK_DELETE(m_pEncodeThread); //Autodelete = FALSE;

	// Warte bis 'DecodeThread' beendet ist.
	if (m_pDecodeThread)
		WaitForSingleObject(m_pDecodeThread->m_hThread, 10000);

	WK_DELETE(m_pDecodeThread); //Autodelete = FALSE;

	for (BYTE byCoCoID = 0; byCoCoID <= MAX_COCO_ID; byCoCoID++)
	{
		WK_DELETE(m_pInput[byCoCoID]);
		WK_DELETE(m_pOutputCoCoCamera[byCoCoID]);
		WK_DELETE(m_pOutputCoCoRelay[byCoCoID]);
	}
	WK_DELETE(m_pOutputMegraCamera);
	WK_DELETE(m_pCoCo);
	WK_DELETE(m_pMegra);

	// Nur erforderlich, wenn Icon in der Systemtaskleiste
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnQueryOpen
BOOL CCoCoUnitDlg::OnQueryOpen()
{
	return m_bStandalone;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnQueryEndSession
BOOL CCoCoUnitDlg::OnQueryEndSession() 
{
	if (!CDialog::OnQueryEndSession())
		return FALSE;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnEndSession
void CCoCoUnitDlg::OnEndSession(BOOL bEnding) 
{
	CDialog::OnEndSession(bEnding);
	
	if (bEnding)
	{
		WK_TRACE("System shutdown...\n");
		DestroyWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnSysCommand
void CCoCoUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CWnd::OnSysCommand(nID, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnPaint
void CCoCoUnitDlg::OnPaint() 
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
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnBePal
void CCoCoUnitDlg::OnBePal() 
{
	m_pCoCo->SetBeVideoFormat(COCO_PAL);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnBeSecam
void CCoCoUnitDlg::OnBeSecam() 
{
	m_pCoCo->SetBeVideoFormat(COCO_SECAM);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnBeNtsc
void CCoCoUnitDlg::OnBeNtsc() 
{
	m_pCoCo->SetBeVideoFormat(COCO_NTSC);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnBeComposite
void CCoCoUnitDlg::OnBeComposite() 
{
	m_pCoCo->SetBeVideoType(COCO_COMPOSITE);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnBeRgb
void CCoCoUnitDlg::OnBeRgb() 
{
	m_pCoCo->SetBeVideoType(COCO_RGB);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnDlgLuminance
void CCoCoUnitDlg::OnDlgSettings() 
{
	if (Login())
		m_pSettings = new CSettings(m_pCoCo, m_pMegra, this);
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoUnitDlg::OnAbout()
{
	COemGuiApi::AboutDialog(this);
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnExtcard0
void CCoCoUnitDlg::OnExtcard0() 
{
	m_pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD0, COCO_SOURCE_CURRENT);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnExtcard1
void CCoCoUnitDlg::OnExtcard1() 
{
	m_pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD1, COCO_SOURCE_CURRENT);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnExtcard2
void CCoCoUnitDlg::OnExtcard2() 
{
	m_pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD2, COCO_SOURCE_CURRENT);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnExtcard3
void CCoCoUnitDlg::OnExtcard3() 
{
	m_pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD3, COCO_SOURCE_CURRENT);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnExtcardMegra
void CCoCoUnitDlg::OnExtcardMegra() 
{
	m_pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD_MEGRA, COCO_SOURCE_CURRENT);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnFeFbas
void CCoCoUnitDlg::OnFeFbas() 
{
	m_pCoCo->SetFeSourceType(COCO_FBAS);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnFeSvhs
void CCoCoUnitDlg::OnFeSvhs() 
{
	m_pCoCo->SetFeSourceType(COCO_SVHS);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnFePal
void CCoCoUnitDlg::OnFePal() 
{
	m_pCoCo->SetFeVideoFormat(COCO_PAL);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnFeSecam
void CCoCoUnitDlg::OnFeSecam() 
{
	m_pCoCo->SetFeVideoFormat(COCO_SECAM);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnFeNtsc
void CCoCoUnitDlg::OnFeNtsc() 
{
	m_pCoCo->SetFeVideoFormat(COCO_NTSC);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnFeLNotch
void CCoCoUnitDlg::OnFeLNotch() 
{
	m_pCoCo->SetFeFilter(m_pCoCo->GetFeFilter() ^COCO_LNOTCH);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnFeLDec
void CCoCoUnitDlg::OnFeLDec() 
{
	m_pCoCo->SetFeFilter(m_pCoCo->GetFeFilter() ^COCO_LDEC);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnSource1
void CCoCoUnitDlg::OnSource1() 
{
	m_pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, COCO_SOURCE0);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnSource2
void CCoCoUnitDlg::OnSource2() 
{
	m_pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, COCO_SOURCE1);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnSource3
void CCoCoUnitDlg::OnSource3() 
{
	m_pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, COCO_SOURCE2);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnSource4
void CCoCoUnitDlg::OnSource4() 
{
	m_pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, COCO_SOURCE3);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnSource5
void CCoCoUnitDlg::OnSource5() 
{
	m_pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, COCO_SOURCE4);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnSource6
void CCoCoUnitDlg::OnSource6() 
{
	m_pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, COCO_SOURCE5);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnSource7
void CCoCoUnitDlg::OnSource7() 
{
	m_pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, COCO_SOURCE6);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnSource8
void CCoCoUnitDlg::OnSource8() 
{
	m_pCoCo->SetInputSource(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, COCO_SOURCE7);	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedSource
long CCoCoUnitDlg::OnChangedSource(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedSource (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
//	UpdateDlg();
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedSourceType
long CCoCoUnitDlg::OnChangedSourceType(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedSourceType (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
//	UpdateDlg();
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedFeVideoFormat
long CCoCoUnitDlg::OnChangedFeVideoFormat(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedFeVideoFormat (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedBrightness
long CCoCoUnitDlg::OnChangedBrightness(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedBrightness (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedContrast
long CCoCoUnitDlg::OnChangedContrast(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedContrast (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedSaturation
long CCoCoUnitDlg::OnChangedSaturation(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedSaturation (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedHue
long CCoCoUnitDlg::OnChangedHue(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedHue (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedBeVideoFormat
long CCoCoUnitDlg::OnChangedBeVideoFormat(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedBeVideoFormat (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedBeVideoType
long CCoCoUnitDlg::OnChangedBeVideoType(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedBeVideoType (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedEncoderFormat
long CCoCoUnitDlg::OnChangedEncoderFormat(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedEncoderFormat (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedEncoderFramerate
long CCoCoUnitDlg::OnChangedEncoderFramerate(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedEncoderFramerate (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedEncoderBitrate
long CCoCoUnitDlg::OnChangedEncoderBitrate(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedEncoderBitrate (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedDecoderBitrate
long CCoCoUnitDlg::OnChangedDecoderBitrate(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedDecoderBitrate (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedHScale
long CCoCoUnitDlg::OnChangedHScale(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedHScale (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedVScale
long CCoCoUnitDlg::OnChangedVScale(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedVScale (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedFeFilter
long CCoCoUnitDlg::OnChangedFeFilter(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedFeFilter (%u, %lu)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedAlarmEdge
long CCoCoUnitDlg::OnChangedAlarmEdge(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedAlarmEdge (%u, 0x%x)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}
    
/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedAlarmAck
long CCoCoUnitDlg::OnChangedAlarmAck(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedAlarmAck (%u, 0x%x)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnChangedRelais
long CCoCoUnitDlg::OnChangedRelais(WPARAM wParam, LPARAM lParam)
{
//	WK_TRACE("CCoCoUnitDlg::OnChangedRelais (%u, 0x%x)\n",(WORD)wParam, (DWORD) lParam);
	UpdateDlg();
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnAlarm

long CCoCoUnitDlg::OnAlarm(WPARAM wParam, LPARAM lParam)
{
	BYTE byCoCoID = (BYTE)wParam;
	WORD wExtCard = LOWORD(lParam);
	WORD wAlarm	  = HIWORD(lParam);

	if (byCoCoID > MAX_COCO_ID)
		return -1L;

//	WK_TRACE("CCoCoUnitDlg::OnAlarm\tCoCoID%u\tExtCard%u\tAlarm%u\n", (WORD)byCoCoID, wExtCard, wAlarm);

	if (m_pInput[byCoCoID])
		m_pInput[byCoCoID]->AlarmStateChanged(wExtCard, wAlarm);

	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitDlg::OnWmCopyData
long CCoCoUnitDlg::OnWmCopyData(WPARAM wParam, LPARAM lParam)
{
	static DWORD	dwReCnt = 0;
	static DWORD	dwLastTime = 0;
	char			szText[128];
	PARAMSTRUCT		Param;
	DWORD			dwCurrentTime = GetCurrentTime();
	DWORD			dwDeltaTime;

	if (!m_pCoCo)
		return 0L;

	if (!m_pCoCo->ReadData(lParam, Param))
		return 0L;

	BYTE	byCoCoID	= (BYTE)Param.wCoCoID;
	BOOL	bIntra		= (BOOL)Param.wIntra; 
	DWORD	dwData		= Param.dwData;
	LPCSTR	lpMem		= Param.lpData;
	DWORD	dwLen		= Param.dwLen;
	WORD	wExtCard	= LOWORD(Param.dwExtra1);
	WORD	wSource		= HIWORD(Param.dwExtra1);

	int		iCamSubID = wSource +  wExtCard * (COCO_MAX_SOURCE + 1);
	m_dwLastWMCopyDataTime = GetTickCount();

   	//	WK_TRACE("OnWmCopyData\tSource=%u\tIntra=%u\n", wSource, (WORD)bIntra);
 
	// Sind die Daten vom VideoEncoder ?
	if (dwData != COCO_ENCODER_DATA)
	{
		WK_TRACE_ERROR("CCoCoUnitDlg::OnWmCopyData\tError: Unknowed Data\n");
		return 0;
	}

#if (0)	
	CString sMsg;
	sMsg.Format("PictLen|CamID%d", wSource);
	WK_STAT_LOG("Reset", dwLen, sMsg);
#endif

#if (0)	// Schreibt H263 Einzelbilder ins Tempverzeichnis
	char	strFileName[255];
	if (bIntra)
		wsprintf(strFileName, "C:\\TEMP\\FrameI%u.%03lu", wSource, m_dwEncodeCounter++);
	else
		wsprintf(strFileName, "C:\\TEMP\\FrameP%u.%03lu", wSource, m_dwEncodeCounter++);
	
	CFile Write(strFileName, CFile::modeCreate | CFile::modeReadWrite);
	Write.WriteHuge(lpMem, dwLen);
	Write.Close();
#endif
	
	// CoCo im Standalone-Betrieb ?
	if (m_bStandalone)
	{
		if (m_pWFile[wSource])
			m_pWFile[wSource]->Write(lpMem, dwLen); 

		wsprintf(szText, "%lu", m_dwReCnt+=dwLen);
		SetDlgText(IDC_RE_DATA, szText);
		
		dwDeltaTime = dwCurrentTime - dwLastTime;
		dwLastTime = dwCurrentTime;

		if (dwDeltaTime != 0L)
		{
			wsprintf(szText, "%lu CPS   ", (1000L * dwLen) / dwDeltaTime);
			SetDlgText(IDC_RE_SPEED, szText);
		}
		return dwLen;
	}

	// Critical Section
	m_csEncode.Lock();
		
	// Existiert die Outputgruppe ?
	if (m_pOutputCoCoCamera[byCoCoID] == NULL)
	{
//		WK_TRACE_ERROR("OnWmCopyData\tOutputguppe existiert nicht\n");
		m_csEncode.Unlock();
		return dwLen;
	}

	// Existiert ein Job ?
	if (m_pCurrentEncodeJob == NULL) 
	{
 //		WK_TRACE_ERROR("OnWmCopyData\tKein aktueller Job\n");
		m_csEncode.Unlock();
		return dwLen;
	}

	// Da nicht alle anfallenden P-Bilder für einen Encoder-Job verwendet werden,
	// muß das 1. Bild eines jeden neuen Jobs ein I-Bild sein!!
	if (m_bNewEncodeJob && !bIntra)
	{
//		WK_TRACE_WARNING("OnWmCopyData\tPicture droped\tCamID=%d\n", iCamSubID);
		m_csEncode.Unlock();
		return dwLen;
	}

	m_bNewEncodeJob = FALSE;
  
	// Stimmt die geforderte Kamera mit der gelieferten Kamera überein ?
	if (m_pCurrentEncodeJob->m_camID.GetSubID() != iCamSubID) 
	{
//		WK_TRACE_WARNING("OnWmCopyData\tFalsche Kamera\tencoding job cam %d != wished %d, outpict= %d\n",
//					m_pCurrentEncodeJob->m_camID.GetSubID(), iCamSubID, m_pCurrentEncodeJob->m_iOutstandingPics);

		m_csEncode.Unlock();
		return dwLen;
	}

	// Daten an die Outputgroup übergeben.
	m_pOutputCoCoCamera[byCoCoID]->IndicationDataReceived(wExtCard, wSource, lpMem, dwLen, m_pCurrentEncodeJob->m_dwUserData,bIntra);
	m_pCurrentEncodeJob->m_iOutstandingPics--;

	//	WK_TRACE("WM_COPYDATA\tOutstandigPics=%u\n", (WORD)m_pCurrentEncodeJob->m_iOutstandingPics);

	if ((iCamSubID >= 0) && (iCamSubID < COCO_MAX_CAM) ) 
	{
		m_dwDataReceivedTime[iCamSubID] = GetTickCount();
	}
	else 
	{
		WK_TRACE_ERROR("WM_COPYDATA cam=%d out of range",iCamSubID);
	}

	// Wenn dies das letze Bild war, dann bringt es einen zeitlichen Vorteil, sofort das Prozeßmanagement aufzurufen
	if (m_pCurrentEncodeJob->m_iOutstandingPics == 0) 
	{
		ClearCurrentEncodeJob("Outstanding Pics = 0"); // Job done
		ProzessEncodeJobs();
	}											   

	m_csEncode.Unlock();
	
	return dwLen;
}

void CCoCoUnitDlg::OnEdge7() 
{
	WORD wAlarmEdge = m_pCoCo->GetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT);

	wAlarmEdge = wAlarmEdge^0x80;
	m_pCoCo->SetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, wAlarmEdge);
}

void CCoCoUnitDlg::OnEdge6() 
{
	WORD wAlarmEdge = m_pCoCo->GetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT);

	wAlarmEdge = wAlarmEdge^0x40;
	m_pCoCo->SetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, wAlarmEdge);
}

void CCoCoUnitDlg::OnEdge5() 
{
	WORD wAlarmEdge = m_pCoCo->GetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT);

	wAlarmEdge = wAlarmEdge^0x20;
	m_pCoCo->SetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, wAlarmEdge);
}

void CCoCoUnitDlg::OnEdge4() 
{
	WORD wAlarmEdge = m_pCoCo->GetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT);

	wAlarmEdge = wAlarmEdge^0x10;
	m_pCoCo->SetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, wAlarmEdge);
}

void CCoCoUnitDlg::OnEdge3() 
{
	WORD wAlarmEdge = m_pCoCo->GetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT);

	wAlarmEdge = wAlarmEdge^0x08;
	m_pCoCo->SetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, wAlarmEdge);
}
	    
void CCoCoUnitDlg::OnEdge2() 
{
	WORD wAlarmEdge = m_pCoCo->GetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT);

	wAlarmEdge = wAlarmEdge^0x04;
	m_pCoCo->SetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, wAlarmEdge);
}

void CCoCoUnitDlg::OnEdge1() 
{
	WORD wAlarmEdge = m_pCoCo->GetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT);

	wAlarmEdge = wAlarmEdge^0x02;
	m_pCoCo->SetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, wAlarmEdge);
}

void CCoCoUnitDlg::OnEdge0() 
{
	WORD wAlarmEdge = m_pCoCo->GetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT);

	wAlarmEdge = wAlarmEdge^0x01;
	m_pCoCo->SetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT, wAlarmEdge);
}

void CCoCoUnitDlg::OnAck0() 
{
	WORD wAlarmAck  = m_pCoCo->GetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT);
	
	wAlarmAck = wAlarmAck^0x01;
	m_pCoCo->SetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT, wAlarmAck);
}

void CCoCoUnitDlg::OnAck1() 
{
	WORD wAlarmAck  = m_pCoCo->GetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT);
	
	wAlarmAck = wAlarmAck^0x02;
	m_pCoCo->SetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT, wAlarmAck);
}

void CCoCoUnitDlg::OnAck2() 
{
	WORD wAlarmAck  = m_pCoCo->GetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT);
	
	wAlarmAck = wAlarmAck^0x04;
	m_pCoCo->SetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT, wAlarmAck);
}

void CCoCoUnitDlg::OnAck3() 
{
	WORD wAlarmAck  = m_pCoCo->GetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT);
	
	wAlarmAck = wAlarmAck^0x08;
	m_pCoCo->SetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT, wAlarmAck);
}

void CCoCoUnitDlg::OnAck4() 
{
	WORD wAlarmAck  = m_pCoCo->GetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT);
	
	wAlarmAck = wAlarmAck^0x10;
	m_pCoCo->SetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT, wAlarmAck);
}

void CCoCoUnitDlg::OnAck5() 
{
	WORD wAlarmAck  = m_pCoCo->GetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT);
	
	wAlarmAck = wAlarmAck^0x20;
	m_pCoCo->SetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT, wAlarmAck);
}

void CCoCoUnitDlg::OnAck6() 
{
	WORD wAlarmAck  = m_pCoCo->GetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT);
	
	wAlarmAck = wAlarmAck^0x40;
	m_pCoCo->SetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT, wAlarmAck);
}

void CCoCoUnitDlg::OnAck7() 
{
	WORD wAlarmAck  = m_pCoCo->GetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT);
	
	wAlarmAck = wAlarmAck^0x80;
	m_pCoCo->SetAlarmAck(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT, wAlarmAck);
}

void CCoCoUnitDlg::OnRelais0() 
{
	WORD wRelais	= m_pCoCo->GetRelais(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT);
	
	wRelais = wRelais^0x01;
	m_pCoCo->SetRelais(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT, wRelais);
}

void CCoCoUnitDlg::OnRelais1() 
{
	WORD wRelais	= m_pCoCo->GetRelais(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT);
	
	wRelais = wRelais^0x02;
	m_pCoCo->SetRelais(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT, wRelais);
}

void CCoCoUnitDlg::OnRelais2() 
{
	WORD wRelais	= m_pCoCo->GetRelais(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT);
	
	wRelais = wRelais^0x04;
	m_pCoCo->SetRelais(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT, wRelais);
}

void CCoCoUnitDlg::OnRelais3() 
{
	WORD wRelais	= m_pCoCo->GetRelais(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT);
	
	wRelais = wRelais^0x08;
	m_pCoCo->SetRelais(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT, wRelais);
}

/////////////////////////////////////////////////////////////////////////////
// CPlayFile::OnSendScript
void CCoCoUnitDlg::OnSendScript()
{

	CFileDialog FileDlg(TRUE, "mcs", "*.mcs", OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"Script files (*.mcs) | All files (*.*) | *.* ||");

	if (FileDlg.DoModal() == IDCANCEL)
		return;

	CString sFileName = FileDlg.GetPathName();
	//BOOL bRet =
	m_pCoCo->SendMCIScript(COCO_COCOID_CURRENT, sFileName);
}

void CCoCoUnitDlg::OnCocoid0() 
{
	m_pCoCo->SetInputSource(COCO_COCOID0, COCO_EXTCARD_CURRENT, COCO_SOURCE_CURRENT);	
	m_byCoCoID = COCO_COCOID0;
}

void CCoCoUnitDlg::OnCocoid1() 
{
	m_pCoCo->SetInputSource(COCO_COCOID1, COCO_EXTCARD_CURRENT, COCO_SOURCE_CURRENT);	
	m_byCoCoID = COCO_COCOID1;
}

void CCoCoUnitDlg::OnCocoid2() 
{
	m_pCoCo->SetInputSource(COCO_COCOID2, COCO_EXTCARD_CURRENT, COCO_SOURCE_CURRENT);	
	m_byCoCoID = COCO_COCOID2;
}

void CCoCoUnitDlg::OnCocoid3() 
{
	m_pCoCo->SetInputSource(COCO_COCOID3, COCO_EXTCARD_CURRENT, COCO_SOURCE_CURRENT);	
	m_byCoCoID = COCO_COCOID3;
}

void CCoCoUnitDlg::OnDecStart() 
{
	if (m_bStandalone)
	{
		CFileException fileException;
	
		CString sType = "263";

		CFileDialog FileDlg(TRUE, sType, "*."+ sType, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
			"H" + sType + " files (*."+ sType +") | All files (*.*) | *.* ||");

		if (FileDlg.DoModal() == IDCANCEL)
			return;

		CString sFileName = FileDlg.GetPathName();

		if ( !m_pRFile->Open(sFileName, CFile::modeRead		|
										CFile::typeBinary	|
										CFile::shareDenyRead, 
										&fileException ))
		{
			TRACE( "Can't open file %s, error = %u\n","Test." + sType, fileException.m_cause );
			return;
		}
	}

	// Momentan eingestellte Bitrate setzen
	OnSetDecBitrate();
	m_pCoCo->DecoderStart(m_byCoCoID);	
}

void CCoCoUnitDlg::OnDecStop() 
{
	if (m_bStandalone)
	{
		m_pCoCo->DecoderStop(m_byCoCoID);	
	}
}

void CCoCoUnitDlg::OnEncStart() 
{
	m_dwEncodeCounter = 0;
	if (m_bStandalone)
	{
		CFileException fileException;
		CString sType="263";

		CFileDialog FileDlg(TRUE, sType, "*." + sType, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
			"H"+ sType +" files (*."+ sType +") | All files (*.*) | *.* ||");

		if (FileDlg.DoModal() == IDCANCEL)
			return;

		CString sFileName = FileDlg.GetPathName();

		for (int nI = 0; nI <= 7 ; nI++)
		{
			CString s = sFileName.Left(sFileName.GetLength() - 4);
			s = s + (char)(48 + nI) + "." + sType;

			if ( !m_pWFile[nI]->Open(s, CFile::modeCreate |
											CFile::modeWrite  |
											CFile::typeBinary |
											CFile::shareDenyWrite,
											&fileException ))
			{
				TRACE( "Can't open file %s, error = %u\n","Test." + sType, fileException.m_cause );
				return;
			}
		}
		// Momentan eingestellte Bitrate aus dem Editfeld lesen
		CString sEncBitrate;
		GetDlgItem(IDC_ENC_BITRATE)->GetWindowText(sEncBitrate);
		m_dwEncBitrate = atoi(sEncBitrate);
	//	CSecID sID;
	//	m_pOutputCoCoCamera[0]->OnRequestStartH263Encoding(sID, m_PicFormat, m_Compress, m_dwEncBitrate,0);
		
		m_pCoCo->EncoderSetBitrate(COCO_COCOID_CURRENT, m_dwEncBitrate);
		m_pCoCo->EncoderStart(COCO_COCOID_CURRENT);
	}
}

void CCoCoUnitDlg::OnEncStop() 
{
	if (m_bStandalone)
	{
		m_pCoCo->EncoderStop(COCO_COCOID_CURRENT);
	}
}

long CCoCoUnitDlg::OnWmDecStart(WPARAM wParam, LPARAM lParam) 
{
	BYTE byCoCoID = (BYTE)wParam;
	
	// Videoausgang auf Decoderprozeß legen.
	if (m_pCoCo)
		m_pCoCo->ConnectVideoOutput(byCoCoID, COCO_DECODER);

	// Security informieren
	if (m_pOutputCoCoCamera[byCoCoID])
		m_pOutputCoCoCamera[byCoCoID]->IndicationDecoderStarted();

	if (m_bStandalone)
	{
		GetDlgItem(IDC_DEC_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_DEC_STOP)->EnableWindow(TRUE);
	}
	return 0L;
}

long CCoCoUnitDlg::OnWmDecStop(WPARAM wParam, LPARAM lParam) 
{
	BYTE byCoCoID = (BYTE)wParam;

	// Wenn der Encoder läuft, dann auf Encoderbild belassen
	if (m_pCoCo)
		m_pCoCo->ConnectVideoOutput(byCoCoID, COCO_PREP);

	// Security informieren
	if (m_pOutputCoCoCamera[byCoCoID])
		m_pOutputCoCoCamera[byCoCoID]->IndicationDecoderStoped();

	if (m_bStandalone)
	{
		GetDlgItem(IDC_DEC_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_DEC_STOP)->EnableWindow(FALSE);
		m_pRFile->Close();
	}

	SetDlgText(IDC_TR_DATA, "0");
	SetDlgText(IDC_TR_SPEED, "");

	m_dwTrCnt	= 0L;

	return 0L;
}

long CCoCoUnitDlg::OnWmEncStart(WPARAM wParam, LPARAM lParam) 
{
	BYTE byCoCoID = (BYTE)wParam;

	// Videoausgang auf Livebild legen, falls der Decoder nicht läuft.
	if (m_pCoCo && (m_pCoCo->DecoderGetState(byCoCoID) != DECODER_ON))
		m_pCoCo->ConnectVideoOutput(byCoCoID, COCO_PREP);

	// Security informieren
	if (m_pOutputCoCoCamera[byCoCoID])
		m_pOutputCoCoCamera[byCoCoID]->IndicationEncoderStarted();
	
	if (m_bStandalone)
	{
		GetDlgItem(IDC_ENC_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_ENC_STOP)->EnableWindow(TRUE);
	}
	
	return 0L;
}

long CCoCoUnitDlg::OnWmEncStop(WPARAM wParam, LPARAM lParam) 
{
	BYTE byCoCoID = (BYTE)wParam;

	if (!m_pCoCo)
		return 0;

	// Wenn der Decoder läuft, dann auf Decoderbild belassen
	if (m_pCoCo->DecoderGetState(byCoCoID) == DECODER_ON)
		m_pCoCo->ConnectVideoOutput(byCoCoID, COCO_DECODER);
	else
		m_pCoCo->ConnectVideoOutput(byCoCoID, COCO_PREP);

	// Security informieren
	if (m_pOutputCoCoCamera[byCoCoID])
		m_pOutputCoCoCamera[byCoCoID]->IndicationEncoderStoped();

	if (m_bStandalone)
	{
		GetDlgItem(IDC_ENC_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_ENC_STOP)->EnableWindow(FALSE);

		for (int nI = 0; nI <= 7; nI++)
			m_pWFile[nI]->Close();
	}

	SetDlgText(IDC_RE_DATA, "0");
	SetDlgText(IDC_RE_SPEED, "");

	m_dwReCnt	= 0L;

	// Encoder steht. Job beenden.
	ClearCurrentEncodeJob("VJA_STOP_ENCODE"); // job done
	return 0L;
}

long CCoCoUnitDlg::OnWmSleep(WPARAM wParam, LPARAM lParam) 
{
	return 0;
}

void CCoCoUnitDlg::OnSetDecBitrate() 
{
	CString sDecBitrate;
	GetDlgItem(IDC_DEC_BITRATE)->GetWindowText(sDecBitrate);
	m_dwDecBitrate = atoi(sDecBitrate);
	m_pCoCo->DecoderSetBitrate(COCO_COCOID_CURRENT, m_dwDecBitrate); 
}

void CCoCoUnitDlg::OnSetEncBitrate() 
{
	// Bitrate setzen
	CString sEncBitrate;
	GetDlgItem(IDC_ENC_BITRATE)->GetWindowText(sEncBitrate);
	m_dwEncBitrate = atoi(sEncBitrate);
	SetEncoderMode();
}

void CCoCoUnitDlg::OnTest()
{
#if(0)

	HGLOBAL hBitmap = NULL;
	DWORD dwLen = 0L;

//	if (m_pMegra)
//		m_pMegra->GetBitmap(DIBTRUECOLOR, hBitmap, dwLen);
	if (m_pMegra)
		m_pMegra->GetBitmap(DIB8BIT, CRect(0,0,0,0), hBitmap, dwLen);

	if (!hBitmap || dwLen == 0L)
		return;

	CString sFileName ="123Martin.dib";

	CFile file(sFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyNone);
	char* pBitmap = (char*)GlobalLock(hBitmap);

	TRY
	{
		if (pBitmap)
			file.Write(pBitmap, dwLen/2);
	}
	CATCH( CFileException, e )
	{
		TRACE("File %s could not be written! Cause %i\n", (LPCSTR)sFileName, e->m_cause);
	}
	END_CATCH


	file.Close();

	GlobalUnlock(hBitmap);
	GlobalFree(hBitmap);
#else	
	m_bOn = !m_bOn;

	if (m_bOn == TRUE)
		m_pTestThread = AfxBeginThread(Testing, this);
#endif	   
}

void CCoCoUnitDlg::OnSelectQcif() 
{
	m_PicFormat = RESOLUTION_LOW;	
	SetEncoderMode();
}

void CCoCoUnitDlg::OnSelectCif() 
{
	m_PicFormat = RESOLUTION_HIGH;
	SetEncoderMode();
}

void CCoCoUnitDlg::OnCompress1() 
{
	m_Compress = COMPRESSION_1;
	SetEncoderMode();
}

void CCoCoUnitDlg::OnCompress2() 
{
	m_Compress = COMPRESSION_2;
	SetEncoderMode();
}

void CCoCoUnitDlg::OnCompress3() 
{
	m_Compress = COMPRESSION_3;
	SetEncoderMode();
}

void CCoCoUnitDlg::OnCompress4() 
{
	m_Compress = COMPRESSION_4;
	SetEncoderMode();
}

void CCoCoUnitDlg::OnCompress5() 
{
	m_Compress = COMPRESSION_5;
	SetEncoderMode();
}

BOOL CCoCoUnitDlg::SetEncoderMode()
{
	DWORD	dwBytesPerFrame;
	WORD	wFormat;

	// Bytes pro Frame den Kompressionsfaktoren zuordnen 
	switch(m_Compress)
	{
		case COMPRESSION_1:
				dwBytesPerFrame = COCO_BPF_1;
				break;
		case COMPRESSION_2:
				dwBytesPerFrame = COCO_BPF_2;
				break;
		case COMPRESSION_3:
				dwBytesPerFrame = COCO_BPF_3;
				break;
		case COMPRESSION_4:
				dwBytesPerFrame = COCO_BPF_4;
				break;
		case COMPRESSION_5:
				dwBytesPerFrame = COCO_BPF_5;
				break;
		default:
				dwBytesPerFrame = COCO_BPF_3;
	}	  

	// Der RESOLUTION eine Bildgröße zuordnen
	switch (m_PicFormat)
	{
			case RESOLUTION_LOW:
				wFormat = COCO_ENCODER_QCIF;
				dwBytesPerFrame /= 4; // QCIF hat nur ein viertel
				break;
			case RESOLUTION_MID:
				wFormat = COCO_ENCODER_QCIF;
				dwBytesPerFrame /= 4; // QCIF hat nur ein viertel
				break;
			case RESOLUTION_HIGH:
			default:
				wFormat = COCO_ENCODER_CIF;
				dwBytesPerFrame /= 1;
				break;
	}

	// nMPI = 8 * SourceFrameRate * BytesPerFrame / EncoderBitrate
	int nMPI = (int)min(max((8*25*dwBytesPerFrame) / m_dwEncBitrate, COCO_MIN_MPI), COCO_MAX_MPI);
	//nMPI = 1;
	m_pCoCo->EncoderSetMode(COCO_COCOID_CURRENT, wFormat, (WORD)nMPI, m_dwEncBitrate);
	return TRUE;
}

BOOL CCoCoUnitDlg::SetDlgText(UINT nID, const char *pszText)
{
	// Im Zusammenhang mit der Secutity soll der Dialog nicht aktualisiert werde
	if (!m_bStandalone)
		return TRUE;

	if (!pszText)
	{
		WK_TRACE_WARNING("CoCoUnitDlg::SetDlgText\tpszText=NULL\n");
		return FALSE;
	}

	if (!IsValid())
	{
		WK_TRACE_WARNING("CoCoUnitDlg::SetDlgText\tUnValid\n");
		return FALSE;
	}

	SetDlgItemText(nID, pszText);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoUnitDlg::OnOK() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoUnitDlg::OnCancel() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
long CCoCoUnitDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_COCOUNIT, 0);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CCoCoUnitDlg::OnTrayClicked(WPARAM wParam, LPARAM lParam)
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

