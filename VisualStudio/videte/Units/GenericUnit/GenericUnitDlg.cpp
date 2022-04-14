/////////////////////////////////////////////////////////////////////////////
// PROJECT:		GenericUnit
// FILE:		$Workfile: GenericUnitDlg.cpp $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/GenericUnitDlg.cpp $
// CHECKIN:		$Date: 20.01.06 12:40 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 20.01.06 12:38 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <direct.h>
#include "GenericUnitApp.h"
#include "GenericUnitDlg.h"
#include "CipcInputGenericUnit.h"
#include "CipcInputGenericUnitMDAlarm.h"
#include "CipcOutputGenericUnit.h"
#include "CipcExtraMemory.h"
#include "oemgui\oemguiapi.h"
#include "WK_Msg.h"

#ifdef _DEBUG
#undef THIS_FILE											   
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
						    
extern CGenericApp theApp;

/////////////////////////////////////////////////////////////////////////////
UINT PollProzessThread(LPVOID pData)
{
	CGenericUnitDlg*	pAppDlg	 = (CGenericUnitDlg*)pData;  // Pointer auf's App-Objekt

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
	WK_TRACE("Exit PollProzessManagementThread\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CGenericUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CGenericUnitDlg)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_FE_FBAS, OnFeFbas)
	ON_BN_CLICKED(IDC_FE_NTSC_CCIR, OnFeNtscCCIR)
	ON_BN_CLICKED(IDC_FE_PAL_CCIR, OnFePalCCIR)
	ON_BN_CLICKED(IDC_FE_SVHS, OnFeSvhs)
	ON_BN_CLICKED(IDC_SOURCE1, OnSource1)
	ON_BN_CLICKED(IDC_SOURCE2, OnSource2)
	ON_BN_CLICKED(IDC_SOURCE3, OnSource3)
	ON_BN_CLICKED(IDC_SOURCE4, OnSource4)
	ON_BN_CLICKED(IDC_SOURCE5, OnSource5)
	ON_BN_CLICKED(IDC_SOURCE6, OnSource6)
	ON_BN_CLICKED(IDC_SOURCE7, OnSource7)
	ON_BN_CLICKED(IDC_SOURCE8, OnSource8)
	ON_BN_CLICKED(IDC_RELAIS0, OnRelais0)
	ON_BN_CLICKED(IDC_RELAIS1, OnRelais1)
	ON_BN_CLICKED(IDC_RELAIS2, OnRelais2)
	ON_BN_CLICKED(IDC_RELAIS3, OnRelais3)
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
	ON_BN_CLICKED(IDC_COMPRESS_0, OnCompress0)
	ON_BN_CLICKED(IDC_SOURCE9, OnSource9)
	ON_BN_CLICKED(IDC_SOURCE10, OnSource10)
	ON_BN_CLICKED(IDC_SOURCE11, OnSource11)
	ON_BN_CLICKED(IDC_SOURCE12, OnSource12)
	ON_BN_CLICKED(IDC_SOURCE13, OnSource13)
	ON_BN_CLICKED(IDC_SOURCE14, OnSource14)
	ON_BN_CLICKED(IDC_SOURCE15, OnSource15)
	ON_BN_CLICKED(IDC_SOURCE16, OnSource16)
	ON_MESSAGE(WM_GENERIC_REQUEST_OUTPUT_RESET,	OnWmRequestOutputReset)
	ON_MESSAGE(WM_GENERIC_REQUEST_INPUT_RESET,	OnWmRequestInputReset)
	ON_WM_ACTIVATEAPP( )	
	ON_WM_QUERYOPEN()
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_SETMYTEXT, OnSetDlgText)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_BN_CLICKED(IDC_CONFIG_MD, OnDlgMdConfig)
	ON_COMMAND(ID_SETTINGS, OnDlgMdConfig)
	ON_COMMAND(ID_ABOUT,	OnAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenericUnitDlg)
	//}}AFX_DATA_MAP
}
  
/////////////////////////////////////////////////////////////////////////////
CGenericUnitDlg::CGenericUnitDlg() : CDialog(CGenericUnitDlg::IDD)
{
	//{{AFX_DATA_INIT(CGenericUnitDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
 	
	CWK_Profile			prof;

	m_bOk				= FALSE;
	m_bRun				= FALSE;
	m_bOn				= FALSE;

	m_hIcon1			= AfxGetApp()->LoadIcon(IDR_GENERICUNIT1);	// Blue
	m_hIcon2			= AfxGetApp()->LoadIcon(IDR_GENERICUNIT2);	// Green
	m_hIcon3			= AfxGetApp()->LoadIcon(IDR_GENERICUNIT3);	// Red
	
	m_pCodec			= NULL;
	m_pPollProzessThread= NULL;
  	m_pCurrentEncodeJob	= NULL;

	m_dwEncodedFrames	= 0L;
	m_wFormat			= ENCODER_RES_HIGH;
	m_dwBPF				= BPF_3;
	m_nEncoderState		= ENCODER_OFF;
	m_nTimePerFrame		= 40;
	CString sIniFile	= "GenericUnit\\Device";
	m_bStandalone		= TRUE;

	m_dwSourceSwitch	= 0;
	m_bSaveInLog		= FALSE;

	// Alle Pointer initialisieren.
	m_pOutputGenericCamera= NULL;
	m_pOutputGenericRelay	= NULL;			
	m_pInput			= NULL;
	m_pInputMDAlarm		= NULL;

	m_hSyncSemaphore	= NULL;
	
	m_bStandalone		= TRUE;
	m_sJpegPath			= "C:\\Jpeg";

	m_sIniFile.Format(sIniFile + "%u", theApp.GetCardRef());

	CString sJpegPath;
	sJpegPath.Format("E:\\Jpeg%u\\", theApp.GetCardRef());
	m_sJpegPath = prof.GetString(m_sIniFile+"\\Debug", "JpegPath", sJpegPath);
	prof.WriteString(m_sIniFile+"\\Debug", "JpegPath", m_sJpegPath);

	//m_bStandalone = 0-> GenericUnit arbeitet mit Security, 1-> GenericUnit ist Standalone
	//m_bStandalone = 0-> GenericUnit arbeitet mit Security, 1-> GenericUnit ist Standalone
	m_bStandalone = prof.GetInt(m_sIniFile+"\\Debug", "Mode", m_bStandalone);
	prof.WriteInt(m_sIniFile+"\\Debug", "Mode", m_bStandalone);

	// Im Standalonebetrieb kann entweder ins Logfile gespeichert werden, oder
	// aber die realen Jpeg-bilder.
	m_bSaveInLog = prof.GetInt(m_sIniFile+"\\Debug", "SaveInLog", m_bStandalone);
	prof.WriteInt(m_sIniFile+"\\Debug", "SaveInLog", m_bSaveInLog);

	if (m_bStandalone){
		WK_TRACE("Running as 'standalone'\n");
	}

	
	if (m_bStandalone){
		_mkdir(m_sJpegPath);
	}
}

/////////////////////////////////////////////////////////////////////////////
CGenericUnitDlg::~CGenericUnitDlg()
{
	WK_TRACE("~CGenericUnitDlg\n");
	if (m_hShutDownEvent)
	{	
		CloseHandle(m_hShutDownEvent);
		m_hShutDownEvent = NULL;
	}

	WK_DELETE(m_pCodec);
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::PostNcDestroy() 
{
	WK_TRACE("PostNcDestroy\n");
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGenericUnitDlg::Create()
{
	if (!CDialog::Create(IDD))
	{
		return FALSE;
	}
	
	m_pCodec = new CCodec(this);
	if (!m_pCodec)
		return FALSE;

	// Dieses Event veranlaßt das Beenden der einzelnen Threads
	m_hShutDownEvent = CreateEvent(0, TRUE, FALSE, NULL);
	if (!m_hShutDownEvent)
		WK_TRACE_ERROR("Create m_hShutDownEvent failed\n");

	CString sSMName;

	// die CIPC-Schnittstellen aufbauen.
	CString sCardRef;
	sCardRef.Format(_T("%s"), theApp.GetCardRef());

	sSMName = SM_MICO_INPUT;
	if (theApp.GetCardRef() != 1)
		sSMName += sCardRef;
	m_pInput = new CIPCInputGenericUnit(this, m_pCodec, sSMName);
	if (!m_pInput)
		WK_TRACE_ERROR("CGenericUnitDlg::Create\tCIPCInputGenericUnit-Objekt konnte nicht angelegt werden.\n");

	sSMName = SM_MICO_MD_INPUT;
	if (theApp.GetCardRef() != 1)
		sSMName += sCardRef;
	m_pInputMDAlarm = new CIPCInputGenericUnitMDAlarm(this, m_pCodec, sSMName);
	if (!m_pInputMDAlarm)
		WK_TRACE_ERROR("CGenericUnitDlg::Create\tCIPCInputGenericUnitMDAlarm-Objekt konnte nicht angelegt werden.\n");
	
	sSMName = SM_MICO_OUTPUT_CAMERAS;
	if (theApp.GetCardRef() != 1)
		sSMName += sCardRef;
	
	m_pOutputGenericCamera = new CIPCOutputGenericUnit(this, m_pCodec, OUTPUT_TYPE_GENERIC_CAMERA, sSMName, m_sIniFile);
	if (!m_pOutputGenericCamera)
		WK_TRACE_ERROR("CGenericUnitDlg::Create\tCIPCOutputGenericUnit-GenericCamera-Objekt konnte nicht angelegt werden.\n");
	
	sSMName = SM_MICO_OUTPUT_RELAYS;
	if (theApp.GetCardRef() != 1)
		sSMName += sCardRef;

	m_pOutputGenericRelay = new CIPCOutputGenericUnit(this, m_pCodec, OUTPUT_TYPE_GENERIC_RELAY, sSMName, m_sIniFile);
	if (!m_pOutputGenericRelay)
		WK_TRACE_ERROR("CGenericUnitDlg::Create\tCIPCOutputGenericUnit-Relay-Objekt konnte nicht angelegt werden.\n");	

	if (!m_bStandalone) 
	{
		CString sEventName = "MiCoSyncEvent";
		if (theApp.GetCardRef() != 1)
			sEventName += sCardRef;

		m_hSyncSemaphore = CreateSemaphore(NULL, 0, 10, sEventName);
		if (m_hSyncSemaphore==NULL) 
		{
			WK_TRACE_ERROR("Failed to create sync semaphore\n");
		}
	}						    

	// PollProzessThread anlegen
	m_pPollProzessThread = AfxBeginThread(PollProzessThread, this);
	if (m_pPollProzessThread)
		m_pPollProzessThread->m_bAutoDelete = FALSE;

	m_bOk  = TRUE;
	m_bRun = TRUE;

	if (m_bStandalone)
	{
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
		tnd.hIcon		= theApp.LoadIcon(IDR_GENERICUNIT1);

		lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
		Shell_NotifyIcon(NIM_ADD, &tnd);
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::ClearAllJobs()
{
	ClearAllEncodeJobs();
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::ClearAllEncodeJobs()
{
	ClearCurrentEncodeJob();
	m_jobQueueEncode.SafeDeleteAll();
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::ClearCurrentEncodeJob()
{
	m_csEncode.Lock();

	if (m_pCurrentEncodeJob)
	 	m_jobQueueEncode.IncreaseSemaphore(); // Event auslösen -> Nächsten Encoderjob holen
	WK_DELETE(m_pCurrentEncodeJob);
	m_csEncode.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::RequestOutputReset()
{
	// WorkerThread benutzt MFC -> Deshalb Postmessage
	PostMessage(WM_GENERIC_REQUEST_OUTPUT_RESET, 0, 0); 
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::RequestInputReset()
{
	// WorkerThread benutzt MFC -> Deshalb Postmessage
	PostMessage(WM_GENERIC_REQUEST_INPUT_RESET, 0, 0); 
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::Dump( CDumpContext &dc ) const
{
	 CObject::Dump( dc );
	 dc << "CGenericUnitDlg = ";
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGenericUnitDlg::IsValid()
{
	return m_bOk;
}

////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::PollProzess()
{
	static BOOL bOnlyOnce = FALSE;
	HANDLE hEvent[2];

	hEvent[0] = m_jobQueueEncode.GetSemaphore();
	hEvent[1] = m_hShutDownEvent;
	// Warte bis neuer Job verfügbar ist
	
	switch (WaitForMultipleObjects(2, hEvent, FALSE, 30*1000))
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
				WK_TRACE("JobQueue empty\tEncoder stop\n");
				if (m_pOutputGenericCamera)
					m_pOutputGenericCamera->EncoderStop(NULL);
				bOnlyOnce = FALSE;
				SetGenericIcon(GENERIC_ICON_BLUE);
			}
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnReceivedJpegData(const JPEG* pJpeg)
{
	// Gültiges Bild?
	if (!pJpeg)
		return;

	if (m_bStandalone)		   
	{	
		SaveJpegFile(pJpeg);
		return;
	}

	// Daten an die Outputgroup übergeben.
	m_csEncode.Lock();
		
	// Existiert ein Job ?
	if (m_pCurrentEncodeJob != NULL) 
	{
		// Stimmt der geforderte Prozeß mit dem gelieferten Prozeß überein.?
		if (m_pCurrentEncodeJob->m_dwUserData == pJpeg->dwProzessID) 
		{
			// Daten an die Outputgroup übergeben.
			if (m_pOutputGenericCamera)
				m_pOutputGenericCamera->IndicationDataReceived(pJpeg,TRUE);

			if (m_pCurrentEncodeJob)
				m_pCurrentEncodeJob->m_iOutstandingPics--;

			if (m_pCurrentEncodeJob->m_iOutstandingPics == 0) 
			{
				ClearCurrentEncodeJob(); // Letztes Bild erhalten -> Job beenden
			}
		}
		else
		{
			WK_TRACE("Data without job (ProzessIDs: (%lu, %lu)\n",	m_pCurrentEncodeJob->m_dwUserData,
																	pJpeg->dwProzessID);
			// ProzessID 0 gibt's nicht, wird nur von der Generic intern verwendet
			if (pJpeg->dwProzessID == 0)
				ClearCurrentEncodeJob();
		}	
	}

	m_csEncode.Unlock();

	return;
}

/////////////////////////////////////////////////////////////////////////////
// Wird ein Alarm festgestellt, so muß diese Funktion aufgerufen werden.
void CGenericUnitDlg::OnReceivedAlarm(WORD wCurrentAlarmState)
{
	if (!m_bStandalone)
	{
 		if (m_pInput)
			m_pInput->AlarmStateChanged(wCurrentAlarmState);
	}
	else
	{
		WK_TRACE("OnReceivedAlarm (0x%x)\n", wCurrentAlarmState);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnReceivedMotionAlarm(WORD wSource, CPoint Point)
{
	if (!m_bStandalone)
	{
		if (m_pInputMDAlarm)
			m_pInputMDAlarm->OnReceivedMotionAlarm(wSource, Point, TRUE);
 		if (m_pInputMDAlarm)
			m_pInputMDAlarm->OnReceivedMotionAlarm(wSource, Point, FALSE);
	}
	else
	{
		WK_TRACE("OnReceivedMotionAlarm (Source=%u (%d, %d))\n", wSource, Point.x, Point.y);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::SaveJpegFile(const JPEG* pJpeg)
{
	if (!pJpeg)
		return;

	LPCSTR	lpBuffer	= (char*)pJpeg->Buffer.pLinAddr;// Pointer auf Jpegdaten
	DWORD	dwLen		= pJpeg->dwLen;			// Länge der Jpegdaten
	DWORD	dwProzessID	= pJpeg->dwProzessID;

	char* pszText = new char[16];
	ZeroMemory(pszText,16);

	CString s;
	s.Format("%sJ_%06lu.jpg\0",m_sJpegPath, m_dwEncodedFrames);

	// Soll das Jpegfile gesichert werden, oder nur die Informationen ins Logfile
	if (m_bSaveInLog)
	{
		WK_TRACE("%s\tProzeßID=%lu\t%lu\n",	s, dwProzessID, dwLen);
	}
	else
	{
		CFile wFile(s,	CFile::modeCreate | CFile::modeWrite  |
						CFile::typeBinary | CFile::shareDenyWrite);

		wFile.Write(lpBuffer, dwLen); 
	}

	wsprintf(pszText, "%lu\0", m_dwEncodedFrames);
	if (m_bOk)
		PostMessage(WM_SETMYTEXT, IDC_ENCODED_FRAMES, (LPARAM)pszText);

	m_dwEncodedFrames++;

	return;
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::ProzessManagement()
{
	m_csEncode.Lock();

	if (m_pCurrentEncodeJob == NULL)
	{
		// Aktuellen und nächsten Job holen
		m_pCurrentEncodeJob = m_jobQueueEncode.SafeGetAndRemoveHead();
		
		if (m_pCurrentEncodeJob)
		{
			CIPCOutputGenericUnit* pOutput = (CIPCOutputGenericUnit*)m_pCurrentEncodeJob->m_pCipc;
			SetGenericIcon(GENERIC_ICON_RED);

			switch (m_pCurrentEncodeJob->m_action)
			{			  
				case VJA_ENCODE:
					if (pOutput)
				 		pOutput->EncoderStart(m_pCurrentEncodeJob);
					break;
				case VJA_STOP_ENCODE:
					SetGenericIcon(GENERIC_ICON_BLUE);
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
						ReleaseSemaphore(m_hSyncSemaphore, 1, NULL);
					ClearCurrentEncodeJob();	// job done 
					break;
				default:	
					WK_TRACE_ERROR("Invalid Encode job type\n");
					ClearCurrentEncodeJob();	// job done 
			}	// end of job switch
		}	// end of job found		  
		else															 
		{
			SetGenericIcon(GENERIC_ICON_GREEN);
		}
	}
	
	m_csEncode.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnCancel() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnOK() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGenericUnitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetGenericIcon(GENERIC_ICON_BLUE, FALSE);	// Small Icon
	SetGenericIcon(GENERIC_ICON_BLUE, TRUE);	// Large Icon

	if (theApp.GetCardRef() == 1)
		SetWindowText("GenericUnit1");
	else if (theApp.GetCardRef() == 2)
		SetWindowText("GenericUnit2");
	else if (theApp.GetCardRef() == 3)
		SetWindowText("GenericUnit3");
	else if (theApp.GetCardRef() == 4)
		SetWindowText("GenericUnit4");
	else
		WK_TRACE_ERROR("Unknown Applicationname\n");

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::SetGenericIcon(WORD wColor, BOOL bType)
{
	static WORD wCurColor	= GENERIC_ICON_NOCOLOR;
	static BOOL bCurType	= FALSE;

	if ((wColor != wCurColor) || (bType != bCurType))
	{
		wCurColor = wColor;
	
		if (m_bStandalone)
		{
			switch (wColor)
			{
				case GENERIC_ICON_BLUE:
					if (m_hIcon1)
						::PostMessage(m_hWnd, WM_SETICON,	bType, (LPARAM)m_hIcon1);
		 			break;
				case GENERIC_ICON_GREEN:
					if (m_hIcon2)
						::PostMessage(m_hWnd, WM_SETICON,	bType, (LPARAM)m_hIcon2);
					break;
				case GENERIC_ICON_RED:
					if (m_hIcon3)
						::PostMessage(m_hWnd, WM_SETICON,	bType, (LPARAM)m_hIcon3);
					break;
				default:
					WK_TRACE("Nicht definiertes Icon\n");
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
			tnd.hIcon		= theApp.LoadIcon(IDR_GENERICUNIT1);

			lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);

			switch (wColor)
			{
				case GENERIC_ICON_BLUE:
					tnd.hIcon =	m_hIcon1;
		 			break;
				case GENERIC_ICON_GREEN:
					tnd.hIcon =	m_hIcon2;
					break;
				case GENERIC_ICON_RED:
					tnd.hIcon =	m_hIcon3;
					break;
				default:
					WK_TRACE("Nicht definiertes Icon\n");
			}
			Shell_NotifyIcon(NIM_MODIFY, &tnd);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGenericUnitDlg::OnQueryOpen()
{
	return m_bStandalone;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGenericUnitDlg::OnQueryEndSession() 
{
	if (!CDialog::OnQueryEndSession())
		return FALSE;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnEndSession(BOOL bEnding) 
{
	CDialog::OnEndSession(bEnding);

	if (bEnding)
	{
		WK_TRACE("System shutdown...\n");

		DestroyWindow();
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CWnd::OnSysCommand(nID, lParam);
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnPaint() 
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
long CGenericUnitDlg::OnWmRequestOutputReset(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	ClearAllJobs();

	return 0;					 
}

/////////////////////////////////////////////////////////////////////////////
long CGenericUnitDlg::OnWmRequestInputReset(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnDestroy() 
{
	WK_TRACE("OnDestroy\n");

	CDialog::OnDestroy();
	
	ClearAllJobs();

	// Threads beenden.
	m_bRun = FALSE;
 	m_bOk  = FALSE;

	// ShutDown-Event setzen
	if (m_hShutDownEvent)
		SetEvent(m_hShutDownEvent);
	
	// Warte bis 'PollProzessThread' beendet ist.
	if (m_pPollProzessThread){
		WaitForSingleObject(m_pPollProzessThread->m_hThread, 2000);
	}

	// Nur erforderlich, wenn Icon in der Systemtaskleiste
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);

	WK_DELETE(m_pPollProzessThread); //Autodelete = FALSE;
	WK_DELETE(m_pInput);
	WK_DELETE(m_pInputMDAlarm);
	WK_DELETE(m_pOutputGenericCamera);
	WK_DELETE(m_pOutputGenericRelay);

	WK_DELETE(m_pCodec);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGenericUnitDlg::UpdateDlg()
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
	WORD  wSourceType	= m_pCodec->GetSourceType();
	
	WORD  wFormat		= m_pCodec->EncoderGetFormat();
	DWORD dwBPF			= m_pCodec->EncoderGetBPF();

	wRelais				= m_pCodec->GetRelais();

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
		case VIDEO_SOURCE4:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE5);
			break;
		case VIDEO_SOURCE5:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE6);
			break;
		case VIDEO_SOURCE6:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE7);
			break;
		case VIDEO_SOURCE7:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE8);
			break;
		case VIDEO_SOURCE8:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE9);
			break;
		case VIDEO_SOURCE9:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE10);
			break;
		case VIDEO_SOURCE10:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE11);
			break;
		case VIDEO_SOURCE11:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE12);
			break;
		case VIDEO_SOURCE12:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE13);
			break;
		case VIDEO_SOURCE13:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE14);
			break;
		case VIDEO_SOURCE14:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE15);
			break;
		case VIDEO_SOURCE15:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE16, IDC_SOURCE16);
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

	switch(wSourceType)
	{
	 	case SRCTYPE_FBAS:
			CheckRadioButton(IDC_FE_FBAS, IDC_FE_SVHS, IDC_FE_FBAS);
			break;
		case SRCTYPE_SVHS:
			CheckRadioButton(IDC_FE_FBAS, IDC_FE_SVHS, IDC_FE_SVHS);
			break;
	}

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
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnFeFbas() 
{
	if (m_pCodec)
		m_pCodec->SetSourceType(SRCTYPE_FBAS);	
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnFeSvhs() 
{
	if (m_pCodec)
		m_pCodec->SetSourceType(SRCTYPE_SVHS);	
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnFePalCCIR() 
{
	if (m_pCodec)
		m_pCodec->SetVideoFormat(VFMT_PAL_CCIR);	
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnFeNtscCCIR() 
{
	if (m_pCodec)
		m_pCodec->SetVideoFormat(VFMT_NTSC_CCIR);	
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource1() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE0;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE1;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource2() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE1;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE2;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource3() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE2;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE3;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource4() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE3;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE4;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource5() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE4;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE5;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource6() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE5;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE6;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource7() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE6;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE7;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource8() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE7;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE8;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource9() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE8;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE9;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource10() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE9;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE10;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource11() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE10;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE11;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource12() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE11;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE12;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource13() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE12;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE13;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource14() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE13;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE14;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnSource15() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE14;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE15;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

void CGenericUnitDlg::OnSource16() 
{
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel,NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE15;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE0;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnRelais0() 
{
	if (m_pCodec)
	{
		BYTE byRelais	= m_pCodec->GetRelais();
		byRelais = byRelais^0x01;
		m_pCodec->SetRelais(byRelais);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnRelais1() 
{
	if (m_pCodec)
	{
		BYTE byRelais	= m_pCodec->GetRelais();
		byRelais = byRelais^0x02;
		m_pCodec->SetRelais(byRelais);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnRelais2() 
{
	if (m_pCodec)
	{
		BYTE byRelais	= m_pCodec->GetRelais();
		byRelais = byRelais^0x04;
		m_pCodec->SetRelais(byRelais);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnRelais3() 
{
	if (m_pCodec)
	{
		BYTE byRelais	= m_pCodec->GetRelais();
		byRelais = byRelais^0x08;
		m_pCodec->SetRelais(byRelais);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnEncStart() 
{
	m_dwEncodedFrames = 0L;
	m_dwSourceSwitch  = 0L;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel, NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE_NO;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);

		if (m_pCodec->EncoderStart())
			m_nEncoderState = ENCODER_ON;

		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnEncStop() 
{
	if (m_pCodec)
	{
		if (m_pCodec->EncoderStop())
			m_nEncoderState = ENCODER_OFF;

		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnTest()
{
	m_bOn = !m_bOn;
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnCompress0() 
{
	m_dwBPF = BPF_0;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel, NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE_NO;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnCompress1() 
{
	m_dwBPF = BPF_1;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel, NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE_NO;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnCompress2() 
{
	m_dwBPF = BPF_2;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel, NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE_NO;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnCompress3() 
{
	m_dwBPF = BPF_3;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel, NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE_NO;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnCompress4() 
{
	m_dwBPF = BPF_4;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel, NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE_NO;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnCompress5() 
{
	m_dwBPF = BPF_5;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel, NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE_NO;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
long CGenericUnitDlg::OnSetDlgText(WPARAM wParam, LPARAM lParam)
{
	int nID			= (int)wParam;
	char* pszText	= (char*)lParam;

	if (pszText)
		SetDlgItemText(nID, pszText);

	WK_DELETE(pszText);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CGenericUnitDlg::SetDlgText(UINT nID, const char *pszText)
{
	// Im Zusammenhang mit der Secutity soll der Dialog nicht aktualisiert werde
	if (!m_bStandalone)
		return TRUE;

	if (!pszText)
	{
		WK_TRACE_WARNING("GenericUnitDlg::SetDlgText\tpszText=NULL\n");
		return FALSE;
	}

	if (!IsValid())
	{
		WK_TRACE_WARNING("GenericUnitDlg::SetDlgText\tUnValid\n");
		return FALSE;
	}

	SetDlgItemText(nID, pszText);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnHighResolution() 
{
	m_wFormat = ENCODER_RES_HIGH;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel, NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE_NO;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnLowResolution() 
{
	m_wFormat = ENCODER_RES_LOW;

	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSel, NextSrcSel;
		SrcSel.wSource		= VIDEO_SOURCE_CURRENT;
		SrcSel.wFormat		= m_wFormat;
		SrcSel.dwBPF		= m_dwBPF;
		SrcSel.dwProzessID	= 0;
		SrcSel.dwPics		= (DWORD)-1;
		NextSrcSel.wSource	= VIDEO_SOURCE_NO;

		m_pCodec->SetInputSource(SrcSel, NextSrcSel);
	}
}

/////////////////////////////////////////////////////////////////////////////
long CGenericUnitDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM /*lParam*/)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, theApp.GetApplicationId(), 0);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnDlgMdConfig() 
{
	if (m_pCodec)
		m_pCodec->MotionDetectionConfiguration();	
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CGenericUnitDlg::OnTrayClicked(WPARAM wParam, LPARAM lParam)
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
long CGenericUnitDlg::DoOpenMDDlg(WPARAM wParam, LPARAM lParam)
{
	if (m_pCodec)
		m_pCodec->MotionDetectionConfiguration();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CGenericUnitDlg::OnAbout()
{
	COemGuiApi::AboutDialog(this);
}
