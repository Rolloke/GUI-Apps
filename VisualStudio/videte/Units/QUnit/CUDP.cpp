#include "StdAfx.h"
#include ".\cudp.h"
#include "QUnitDlg.h"
#include "CMotionDetection.h"
#include <systemtime.h>
#include <VImage\CJpeg.h>
#include "Cap5BoardLibEx.h"

CWK_Timer	theTimer;

CMN_HWIDnSN g_HWIDnSN[] = {
// Model Id,Activation code... pairs....
// This is just a example..
// Use your company's own id and code pairs..
	{0x0116, { 0x12, 0x1b, 0xe5, 0xb0, 0x64, 0x47, 0xf9, 0x74, 0x62, 0x09, 0xbc, 0x81, 0xf5, 0xdf, 0x85, 0xfa}},
	{0x0121, { 0x51, 0x36, 0x76, 0xc7, 0xc6, 0x59, 0x5d, 0xab, 0x4d, 0xe4, 0x99, 0x23, 0x12, 0x41, 0x41, 0x28}}, 
	{0x0003, { 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03}},
	{0x0004, { 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}}, 
	{0x0005, { 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}}, 
	{0x0006, { 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06}}, 
};

////////////////////////////////////////////////////////////////////////////////////////////////////
CUDP::CUDP(CQUnitDlg* pMainDlg, const CString& sIniFile, int nVideoFormat, int nWidth, int nHeight)
{
	m_bRunCaptureThread		= FALSE;
	m_bRunExternEventThread	= FALSE;
	m_bValid				= FALSE;
	m_pMainDlg				= pMainDlg;
	m_sIniFile				= sIniFile;
	m_nVideoFormat			= nVideoFormat;
	m_nImgWidth				= nWidth;
	m_nImgHeight			= nHeight;
	
	m_pBdInfo				= NULL;
	m_dwCompleteSensorMask	= 0;
	m_dwCompleteEdgeMask	= 0;

	m_nAvailableCameras		= 0;
	m_nAvailableRelays		= 0;
	m_nAvailableInputs		= 0;

	m_nMaxChannelFps		= 25;
	m_pCaptureThreadProc	= NULL;					// Holt Bilddaten, Videostatus und Alarmstatus
	m_pExternEventThreadProc= NULL;					// Warte auf Watchdog trigger und Piezo

	m_bConversionTablesReady= FALSE;
	m_nColorFormat			= CAP5_COLOR_FORMAT_YUY2;
	
	m_dwTestImageLen		= 0;
	m_lpTestImage			= NULL;

	ZeroMemory(&m_SystemInfo, sizeof(m_SystemInfo));

	if (m_nVideoFormat == CMN5_VIDEO_FORMAT_PAL_B)
		m_nMaxChannelFps	= 25;					// PAL=25
	else if (m_nVideoFormat == CMN5_VIDEO_FORMAT_NTSC_M)
		m_nMaxChannelFps	= 30;					// NTSC=30
	else
		WK_TRACE_WARNING(_T("Unsupported Videoformat (%d)\n"), m_nVideoFormat);

	m_nMaxBoardFps		= 4*m_nMaxChannelFps;	// Fps pro Board.

	m_nActiveCamera		= 0;

	m_pMotionDetection = NULL;

	// Kleines Testbild aus der Resource laden
	HRSRC hRc = FindResource(NULL, MAKEINTRESOURCE(IDR_BIN_TESTIMAGE), _T("BINRES"));
	if (hRc)
	{
		HGLOBAL hJpegResource	= LoadResource(NULL, hRc);
		if (hJpegResource)
		{
			m_dwTestImageLen	= SizeofResource(NULL, hRc);
			m_lpTestImage		= (BYTE*)LockResource(hJpegResource);
		}
		else
			WK_TRACE_ERROR(_T("Kleines Testbild konnte nicht geladen werden\n"));
	}
	else
		WK_TRACE_ERROR(_T("Testbild konnte nicht geladen werden\n"));

	// Ermittele Anzahl der Boards, Anzahl der Channel/Board, Anzahl der Inputs/Outputs etc...
	GetDriverInformation();

	// Die Frameratenliste zunächst einmal löschen.
	for (int nBoardID = 0; nBoardID < (int)m_SystemInfo.uNumOfBoard; nBoardID++)
		m_pFpsList[nBoardID] = NULL;

	// Den Videostatus ersteinmal zurücksetzen.
	for (int nCamera = 0; nCamera < GetAvailableCameras(); nCamera++)
		m_bVideoStatus[nCamera]	= TRUE;

	WK_TRACE(_T("Supported resolutions:\n"));
	const CMN5_RESOLUTION_INFO* pResInfo = m_pBdInfo[0].pResInfo;
	for (int nI = 0; nI < (int)pResInfo->uSizeCount; nI++)
	{
		int nWidth  = CMN5_GETIMGWIDTH(pResInfo->aImgDesc[nI].uImageSize);
		int nHeight = CMN5_GETIMGHEIGHT(pResInfo->aImgDesc[nI].uImageSize);
		WK_TRACE(_T("\tBxH = %dx%d\n"),nWidth ,nHeight);
	}

	// CMotion Objekt anlegen und initialisieren.
	m_pMotionDetection = new CMotionDetection(this, m_sIniFile);
	if (m_pMotionDetection)	
	{
		if (m_pMotionDetection->Init())
		{
			if (LoadConfig())
			{
				if (InitializeDriver())
					m_bValid = StartExternEventThread();
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
CUDP::~CUDP(void)
{
	BOOL bResult = FALSE;

	StopCaptureThread();

	StopExternEventThread();

	// Watchdog aus.
	Cap5SetWatchdog(0, CMN5_WC_DISABLE, 0, 0);
	Sleep(500);

	if (Cap5Stop())
		bResult = Cap5Endup();

	WK_DELETE(m_pMotionDetection);

	for (int nBoardID = 0; nBoardID < (int)m_SystemInfo.uNumOfBoard; nBoardID++)
		WK_DELETE(m_pFpsList[nBoardID]);

	m_bValid = FALSE;
	WK_DELETE(m_pBdInfo);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::IsValid()
{
	return (m_bValid && m_bRunCaptureThread);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::InitializeDriver()
{
	BOOL bResult = FALSE;
	BOOL bError  = FALSE;

	UCHAR sn[16] = {0};

	if (m_pBdInfo)
	{
		// Alle gefundenen Boards aktivieren.
		for (int nBoardID = 0; nBoardID < (int)m_SystemInfo.uNumOfBoard; nBoardID++)
		{
			if (GetActCodeFromModelID(m_pBdInfo[nBoardID].uModelID, sn))
			{
				if (!Cap5Activate(nBoardID, sn))
					bError = TRUE;
			}
		}
	}
	
	// In den Setup Status wechseln.
	if (!bError && Cap5Setup())
	{
		// Läuft der Thread noch? dann diesen Beenden.
		if (m_bRunCaptureThread)
		{
			StopCaptureThread();

			Cap5Stop();
			Cap5Endup();
		}

		// Alle Boards mit Defaultwerten initialisieren.
		for (int nBoardID = 0; nBoardID < (int)m_SystemInfo.uNumOfBoard; nBoardID++)
		{
			if (Cap5SetVideoFormat(nBoardID, m_nVideoFormat))
			{
				if (Cap5SetColorFormat(nBoardID, m_nColorFormat))
				{		
					for (int nChannel = 0; nChannel < (int)m_pBdInfo[nBoardID].uMaxChannel; nChannel++)
					{
						if (Cap5SetImageSize(nBoardID, nChannel, CMN5_MAKEIMGSIZE(m_nImgWidth, m_nImgHeight)))
						{
							if (Cap5VideoEnable(nBoardID, nChannel, TRUE))
							{
								if (!Cap3SetSWAGCMode(nBoardID, nChannel, CAP_SW_AGC_AUTO))
								{
									WK_TRACE_ERROR(_T("Cap3SetSWAGCMode failed\n"));
									bError = TRUE;
								}
							}
							else
							{
								WK_TRACE_ERROR(_T("Cap5VideoEnable failed\n"));
								bError = TRUE;
							}
						}
						else
						{
							WK_TRACE_ERROR(_T("Cap5SetImageSize failed\n"));
							bError = TRUE;
						}
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("Cap5SetColorFormat failed\n"));
					bError = TRUE;
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("Cap5SetVideoFormat failed\n"));
				bError = TRUE;
			}
		}

		if (!bError)
		{
			// Capturemethoden festlegen.
			if (Cap5SetCaptureMethod(CMN5_CAPTURE_TYPE_ON_DATA, CMN5_CAPTURE_METHOD_QUERY))
			{
				if (Cap5SetCaptureMethod(CMN5_CAPTURE_TYPE_ON_CHANNEL, CMN5_CAPTURE_METHOD_QUERY))
				{
					if (Cap5SetCaptureMethod(CMN5_CAPTURE_TYPE_ON_SENSOR, CMN5_CAPTURE_METHOD_QUERY))
					{
						// In den 'Run'-Status wechseln.
						if (Cap5Run())
						{
							// Capturethread starten.
							if (StartCaptureThread())
							{
								// Watchdog initialisieren.
								if (Cap5SetWatchdog(0, CMN5_WC_SET_TIMEOUT_VALUE, 30, 0))
                                    bResult = Cap5SetWatchdog(0, CMN5_WC_ENABLE, 0, 0);
								else
									WK_TRACE_ERROR(_T("Cap5SetWatchdog failed\n"));
							}
							else
								WK_TRACE_ERROR(_T("StartCaptureThread failed\n"));
						}
						else
							WK_TRACE_ERROR(_T("Cap5Run failed\n"));
					}
					else
						WK_TRACE_ERROR(_T("Cap5SetCaptureMethod (CMN5_CAPTURE_TYPE_ON_SENSOR) failed\n"));
				}
				else
					WK_TRACE_ERROR(_T("Cap5SetCaptureMethod (CMN5_CAPTURE_TYPE_ON_CHANNEL) failed\n"));
			}
			else
				WK_TRACE_ERROR(_T("Cap5SetCaptureMethod (CMN5_CAPTURE_TYPE_ON_DATA) failed\n"));
		}
	}

	return bResult;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::GetActCodeFromModelID(DWORD hwid,UCHAR * sn) 
{
	for(int i=0;i<sizeof(g_HWIDnSN)/sizeof(CMN_HWIDnSN);i++) {
		if(g_HWIDnSN[i].HWID == hwid) {
			CopyMemory(sn,g_HWIDnSN[i].SN,16);
			return TRUE;
		}
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::GetDriverInformation()
{
	BOOL bResult = FALSE;

	bResult = Cap5GetSystemInfo(&m_SystemInfo);

	if (bResult)
	{
		if (!m_pBdInfo)
			m_pBdInfo = new CAP5_BOARD_INFO[m_SystemInfo.uNumOfBoard];
		
		if (m_pBdInfo)
		{
			WK_TRACE(_T("The number of installed Board [%d]\n"), m_SystemInfo.uNumOfBoard);
			for (int nBoardID = 0; nBoardID < (int)m_SystemInfo.uNumOfBoard; nBoardID++)
			{
				CMN5_BOARD_INFO_DESC desc;
				ZeroMemory(&desc, sizeof(CMN5_BOARD_INFO_DESC));
				desc.uInfoSize = sizeof(CAP5_BOARD_INFO);
				desc.uInfoVersion = CMN5_BOARD_INFO_VERSION;

				Cap5GetBoardInfo(nBoardID, &desc, &m_pBdInfo[nBoardID]); // TODO: Liefert immer FALSE!?
			}
			bResult &= InitConversionTables();
		}
	}
	else
		WK_TRACE_ERROR(_T("Capture Driver does not installed\n"));

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::StartCaptureThread()
{
	if (!m_pCaptureThreadProc)
	{
		WK_TRACE(_T("CaptureThreadProcedure starts\n"));
		m_bRunCaptureThread = TRUE;

		m_evStopCaptureThread.ResetEvent();
		m_pCaptureThreadProc = AfxBeginThread(OnCaptureThreadProcedure, this);
		if (m_pCaptureThreadProc)
		{
			//m_pCaptureThreadProc->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);	
			m_pCaptureThreadProc->SetThreadPriority(THREAD_PRIORITY_HIGHEST);	
			m_pCaptureThreadProc->m_bAutoDelete = FALSE;	
		}
	}

	return m_bRunCaptureThread;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::StopCaptureThread()
{
	// Warte bis 'ReadDataThread' beendet ist.
	if (m_pCaptureThreadProc)
	{
		m_evStopCaptureThread.SetEvent();
		if (WaitForSingleObject(m_pCaptureThreadProc->m_hThread, 2000) == WAIT_OBJECT_0)
		{
			WK_DELETE(m_pCaptureThreadProc);
		}
		else
			WK_TRACE_WARNING(_T("CUDP::StopCaptureThread() timeout\n"));
	}

	return !m_bRunCaptureThread;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
UINT CUDP::OnCaptureThreadProcedure(LPVOID pData)
{
	BOOL bResult = FALSE;

	CUDP* pThis = (CUDP*)pData;
	if (pThis)
	{
		XTIB::SetThreadName(_T("OnCaptureThreadProcedure"));
		WK_TRACE(_T("OnCaptureThreadProcedure\n"));
	
		HANDLE hEvents[4] = {NULL, NULL, NULL, NULL};
		hEvents[0] = pThis->m_evStopCaptureThread;
		Cap5GetEventHandle(CMN5_DT_VSTATUS, &hEvents[1]);
		Cap5GetEventHandle(CMN5_DT_SENSOR, &hEvents[2]);
		Cap5GetEventHandle(CMN5_DT_VIDEO, &hEvents[3]);

		CAP5_DATA_INFO ImgInfo;
		ZeroMemory(&ImgInfo, sizeof(CAP5_DATA_INFO));

		CMN5_SENSOR_STATUS_INFO SensorStatusInfo; 
		ZeroMemory(&SensorStatusInfo, sizeof(CMN5_VIDEO_STATUS_INFO));
		
		CMN5_VIDEO_STATUS_INFO VideoStatusInfo; 
		ZeroMemory(&VideoStatusInfo, sizeof(CMN5_VIDEO_STATUS_INFO));

		while(pThis->m_bRunCaptureThread)
		{
			DWORD dwObj = WaitForMultipleObjects(4, hEvents, FALSE, 1000);
			switch (dwObj)
			{
				case WAIT_OBJECT_0:
					pThis->m_bRunCaptureThread = FALSE;
					break;
				case WAIT_OBJECT_0+1:
					if (Cap5GetEventData(CMN5_DT_VSTATUS, &VideoStatusInfo))
					{
						pThis->OnVideoState(VideoStatusInfo);
						Cap5ReleaseData(&VideoStatusInfo);
					}
					break;
				case WAIT_OBJECT_0+2:
					if (Cap5GetEventData(CMN5_DT_SENSOR, &SensorStatusInfo))
					{
						pThis->OnSensor(SensorStatusInfo);
						Cap5ReleaseData(&SensorStatusInfo);
					}
					break;
				case WAIT_OBJECT_0+3:
					do
					{
						if ((bResult = Cap5GetEventData(CMN5_DT_VIDEO, &ImgInfo)))
						{
							pThis->OnCapture(ImgInfo);
							Cap5ReleaseData(&ImgInfo);
						}

					}while(bResult && ImgInfo.uHasNextData);

					break;
				case WAIT_TIMEOUT:
					WK_TRACE(_T("OnCaptureThreadProcedure timeout"));
					break;
			}
		}
	
		WK_TRACE(_T("OnCaptureThreadProcedure Stop\n"));
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnCapture(CAP5_DATA_INFO& ImgInfo)
{
	BOOL bResult = FALSE;

	QIMAGE QImage;
	ZeroMemory(&QImage, sizeof(QIMAGE));

	if (ImgInfo.pDataBuffer && (ImgInfo.uDataType == CMN5_DT_VIDEO))
	{
		int nBoardID = ImgInfo.uBoardNum;
		int nChannel  = ImgInfo.uChannelNum;

		GetCameraFromBoardIDandChannel(nBoardID, nChannel, QImage.nCamera);

		// Framerate ermitteln
		CalculateFramerate(QImage.nCamera, QImage.fFramerate);
	
		CMDPoints MDPoints;

		// imgStruktur ergänzen.
		QImage.TimeStamp.GetLocalTime();
		QImage.nWidth		= CMN5_GETIMGWIDTH(ImgInfo.uImageSize);
		QImage.nHeight		= CMN5_GETIMGHEIGHT(ImgInfo.uImageSize);
		QImage.pPMDoints	= &MDPoints;
    
		// Bitmap erzeugen.
		DWORD	dwImageLen	= 2 * QImage.nWidth * QImage.nHeight;
		QImage.lpBMI = (LPBITMAPINFO)new BYTE[dwImageLen+sizeof(BITMAPINFO)+1024];
		if (QImage.lpBMI)
		{
			ZeroMemory(QImage.lpBMI, sizeof(BITMAPINFO));
			QImage.lpBMI->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
			QImage.lpBMI->bmiHeader.biWidth			= QImage.nWidth;
			QImage.lpBMI->bmiHeader.biHeight		= QImage.nHeight;
			QImage.lpBMI->bmiHeader.biPlanes		= 1;
			QImage.lpBMI->bmiHeader.biBitCount		= 16; 
			QImage.lpBMI->bmiHeader.biCompression	= mmioFOURCC('Y','4','2','2');
			QImage.lpBMI->bmiHeader.biSizeImage		= dwImageLen;
		
			CopyMemory(QImage.lpBMI->bmiColors, (BYTE*)ImgInfo.pDataBuffer, dwImageLen);
		
			QImage.ct = COMPRESSION_YUV_422;
		}
		
		// Bewegungserkennung durchführen.
		if (m_pMotionDetection && m_pMotionDetection->MotionCheck(&QImage))
			OnReceivedMotionAlarm(QImage.nCamera, MDPoints);

		// OnCapture aufrufen
		if (m_pMainDlg)
			bResult = m_pMainDlg->OnCapture(&QImage);
		
		// Bitmap wieder zerstören.
		WK_DELETE(QImage.lpBMI);
		
		bResult = TRUE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnVideoState(CMN5_VIDEO_STATUS_INFO& VideoStatusInfo)
{
	int nBoardID = VideoStatusInfo.uBoardNum;

	if (nBoardID)
		Beep(1000, 100);
	for (int nChannel = 0; nChannel < (int)m_pBdInfo[nBoardID].uMaxChannel; nChannel++)
	{
		if (TSTBIT(VideoStatusInfo.VideoStatusMask[0], nChannel))
		{
			int nCamera = 0;
			GetCameraFromBoardIDandChannel(nBoardID, nChannel, nCamera);
			m_bVideoStatus[nCamera] = TSTBIT(VideoStatusInfo.VideoStatus[0], nChannel);
			if (!m_bVideoStatus[nCamera])
				SendTestImage(nCamera);
			m_pMainDlg->OnVideoState(nCamera, m_bVideoStatus[nCamera]);
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Funktioniert nur bis zu 32 Eingängen, da intern eine DWORD Maske verwendet wird.
BOOL CUDP::OnSensor(CMN5_SENSOR_STATUS_INFO& SensorStatusInfo)
{
	BOOL bResult = FALSE;

	if (m_pMainDlg)
	{
		m_dwCompleteSensorMask = 0;
		DWORD dwSensorCounter = 0;
		for (int nBoardID = 0; nBoardID < (int)m_SystemInfo.uNumOfBoard; nBoardID++)
		{
			if (nBoardID < (int)SensorStatusInfo.uBoardNum)
				dwSensorCounter += m_pBdInfo[nBoardID].uMaxDO;
			else
			{
				m_dwCompleteSensorMask |= SensorStatusInfo.SensorStatus[0]<<dwSensorCounter;
				break;
			}
		}

		m_pMainDlg->OnSensor(m_dwCompleteSensorMask^m_dwCompleteEdgeMask);
		bResult = TRUE;
	}

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::CheckResolution(int nCamera, CSize ImageSize)
{
	BOOL bResult = FALSE;
	if (m_pBdInfo)
	{
		int nBoardID = 0;
		int nChannel = 0;
		if (GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel))
		{
			const CMN5_RESOLUTION_INFO* pResInfo = m_pBdInfo[nBoardID].pResInfo;
			if (pResInfo)
			{
				for (int nI = 0; nI < (int)pResInfo->uSizeCount; nI++)
				{
					int nWidth  = CMN5_GETIMGWIDTH(pResInfo->aImgDesc[nI].uImageSize);
					int nHeight = CMN5_GETIMGHEIGHT(pResInfo->aImgDesc[nI].uImageSize);
					if (ImageSize == CSize(nWidth, nHeight))
					{
						bResult = TRUE;
						break;
					}
				}
			}
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void CUDP::OnReceivedMotionAlarm(int nChannel, CMDPoints& Points) const
{
	if (m_pMainDlg)
		m_pMainDlg->OnReceivedMotionAlarm(nChannel, Points);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::StartExternEventThread()
{
	if (!m_pExternEventThreadProc)
	{
		WK_TRACE(_T("ExternEventThreadProcedure starts\n"));
	
		// Mit Hilfe diese Event läßt sich der Watchdog von Außen triggern
		m_hWatchDogTriggerEvent = CreateEvent(NULL,FALSE,FALSE,EV_Q_TRIGGER_WATCHDOG);
		if (m_hWatchDogTriggerEvent == NULL)
			WK_TRACE_ERROR(_T("Create WatchDogTriggerEvent failed\n"));

		// Mit Hilfe diese Event läßt sich der Störmelder von Außen triggern
		m_hPiezoEvent = CreateEvent(NULL, FALSE, FALSE, EV_Q_PIEZO);
		if (m_hPiezoEvent==NULL) 
			WK_TRACE_ERROR(_T("Create PiezoOnOffEvent failed\n"));
		
		m_bRunExternEventThread = TRUE;
		m_pExternEventThreadProc = AfxBeginThread(OnExternEventThreadProcedure, this);
		if (m_pExternEventThreadProc)
			m_pExternEventThreadProc->m_bAutoDelete = FALSE;	
	}

	return m_bRunExternEventThread;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::StopExternEventThread()
{
	if (m_pExternEventThreadProc)
	{
		m_bRunExternEventThread = FALSE;
		if (WaitForSingleObject(m_pExternEventThreadProc->m_hThread, 2000) == WAIT_OBJECT_0)
		{
			WK_DELETE(m_pExternEventThreadProc);
		}
		else
			WK_TRACE_WARNING(_T("CUDP::StopCaptureThread() timeout\n"));
	}

	return !m_bRunExternEventThread;
}

/////////////////////////////////////////////////////////////////////////////
UINT CUDP::OnExternEventThreadProcedure(LPVOID pData)
{	 
	CUDP* pThis = (CUDP*)pData;
	if (pThis)
	{
		XTIB::SetThreadName(_T("OnExternEventThreadProcedure"));
	
		while (pThis->m_bRunExternEventThread)
		{
			HANDLE hEvent[2];
			
			hEvent[0] = pThis->m_hWatchDogTriggerEvent;
			hEvent[1] = pThis->m_hPiezoEvent;

			// Warte auf externe Events
			switch (WaitForMultipleObjects(2, hEvent, FALSE, 100))
			{
				case WAIT_TIMEOUT:
				case WAIT_FAILED:
					break;			
				case WAIT_OBJECT_0:
					pThis->TriggerWatchDog();
					break;			
				case WAIT_OBJECT_0+1:
					pThis->TriggerPiezo();
					break;			
				case WAIT_OBJECT_0+2:		
					return 0;	
				default:
					break;			
			}
		}
	}
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::CalculateFramerate(int nCamera, float& fFramerate)
{
	BOOL bResult = FALSE;
	static DWORD dwLastFrameTC[MAX_CAMERAS] = {GetTickCount()};
	static DWORD dwCounter[MAX_CAMERAS]	= {0};
	static float fFramerates[MAX_CAMERAS] = {0.0};

	dwCounter[nCamera]++;
	if (dwCounter[nCamera] >= 25)
	{
		fFramerates[nCamera] = (float)(1000.0*dwCounter[nCamera]) / (float)(GetTickCount() - dwLastFrameTC[nCamera]);
		dwCounter[nCamera] = 0;
		dwLastFrameTC[nCamera] = GetTickCount();
		bResult = TRUE;
	}

	fFramerate = fFramerates[nCamera];

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::SetActiveCamera(int nActiveCamera)
{
	m_nActiveCamera = nActiveCamera;

	return UpdateFramerateCalculation(nActiveCamera, m_nMaxChannelFps);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int CUDP::GetActiveCamera()
{
	return m_nActiveCamera;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::DoRequestSetBrightness(int nCamera, int nValue)
{
	BOOL bResult = FALSE;
	
	int nBoardID = 0;
	int nChannel = 0;
	if (GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel))
	{
		if (Cap5SetVideoAdjust(nBoardID, nChannel, CAP5_VAC_BRIGHTNESS, nValue, 0, 0, 0))
		{
			_TCHAR szBuffer[512];
			ZeroMemory(szBuffer, sizeof(szBuffer));
			wsprintf(szBuffer, _T("Mapping\\Source%02d"), nCamera);
			WriteConfigurationInt(szBuffer, _T("Brightness"), nValue, m_sIniFile);
			bResult = TRUE;	
			OnConfirmSetBrightness(nCamera, nValue);
		}
	}

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmSetBrightness(int nCamera, int nValue)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmSetBrightness(nCamera,  nValue);
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::DoRequestSetContrast(int nCamera, int nValue)
{
	BOOL bResult = FALSE;
	
	int nBoardID = 0;
	int nChannel = 0;
	if (GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel))
	{
		if (Cap5SetVideoAdjust(nBoardID, nChannel, CAP5_VAC_CONTRAST, nValue, 0, 0, 0))
		{
			_TCHAR szBuffer[512];
			ZeroMemory(szBuffer, sizeof(szBuffer));
			wsprintf(szBuffer, _T("Mapping\\Source%02d"), nCamera);
			WriteConfigurationInt(szBuffer, _T("Contrast"), nValue, m_sIniFile);
			bResult = TRUE;	
			OnConfirmSetContrast(nCamera, nValue);
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmSetContrast(int nCamera, int nValue)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmSetContrast(nCamera,  nValue);
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::DoRequestSetSaturation(int nCamera, int nValue)
{
	BOOL bResult = FALSE;

	int nBoardID = 0;
	int nChannel = 0;
	if (GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel))
	{
		if (Cap5SetVideoAdjust(nBoardID, nChannel, CAP5_VAC_SATURATION_U, nValue, 0, 0, 0))
		{
			if (Cap5SetVideoAdjust(nBoardID, nChannel, CAP5_VAC_SATURATION_V, nValue, 0, 0, 0))
			{
				_TCHAR szBuffer[512];
				ZeroMemory(szBuffer, sizeof(szBuffer));
				wsprintf(szBuffer, _T("Mapping\\Source%02d"), nCamera);
				WriteConfigurationInt(szBuffer, _T("Saturation"), nValue, m_sIniFile);
				bResult = TRUE;	
				OnConfirmSetSaturation(nCamera,  nValue);
			}
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmSetSaturation(int nCamera, int nValue)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmSetSaturation(nCamera,  nValue);
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::DoRequestSetHue(int nCamera, int nValue)
{
	BOOL bResult = FALSE;
	
	int nBoardID = 0;
	int nChannel = 0;
	if (GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel))
	{
		if (Cap5SetVideoAdjust(nBoardID, nChannel, CAP5_VAC_HUE, nValue, 0, 0, 0))
		{
			_TCHAR szBuffer[512];
			ZeroMemory(szBuffer, sizeof(szBuffer));
			wsprintf(szBuffer, _T("Mapping\\Source%02d"), nCamera);
			WriteConfigurationInt(szBuffer, _T("Hue"), nValue, m_sIniFile);
			bResult = TRUE;	
			OnConfirmSetHue(nCamera, nValue);
		}
	}

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmSetHue(int nCamera, int nValue)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmSetHue(nCamera,  nValue);
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::DoRequestGetBrightness(int nCamera, int& nValue)
{
	BOOL bResult = FALSE;
	
	int nBoardID = 0;
	int nChannel = 0;
	DWORD dwParam = 0;
	if (GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel))
	{
		if (Cap5GetVideoAdjust(nBoardID, nChannel, CAP5_VAC_BRIGHTNESS, (DWORD*)&nValue, &dwParam, &dwParam, &dwParam))
			bResult = OnConfirmGetBrightness(nCamera,  nValue);
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmGetBrightness(int nCamera, int nValue)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmGetBrightness(nCamera,  nValue);
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::DoRequestGetContrast(int nCamera, int& nValue)
{
	BOOL bResult = FALSE;
	
	int nBoardID = 0;
	int nChannel = 0;
	DWORD dwParam = 0;
	if (GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel))
	{
		if (Cap5GetVideoAdjust(nBoardID, nChannel, CAP5_VAC_CONTRAST, (DWORD*)&nValue, &dwParam, &dwParam, &dwParam))
			bResult = OnConfirmGetContrast(nCamera,  nValue);
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmGetContrast(int nCamera, int nValue)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmGetContrast(nCamera,  nValue);
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::DoRequestGetSaturation(int nCamera, int& nValue)
{
	BOOL bResult = FALSE;
	
	int nBoardID = 0;
	int nChannel = 0;
	DWORD dwParam = 0;
	if (GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel))
	{
		if (Cap5GetVideoAdjust(nBoardID, nChannel, CAP5_VAC_SATURATION_U, (DWORD*)&nValue, &dwParam, &dwParam, &dwParam))
			bResult = OnConfirmGetSaturation(nCamera,  nValue);
	}

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmGetSaturation(int nCamera, int nValue)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmGetSaturation(nCamera,  nValue);
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::DoRequestGetHue(int nCamera, int& nValue)
{
	BOOL bResult = FALSE;
	
	int nBoardID = 0;
	int nChannel = 0;
	DWORD dwParam = 0;
	if (GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel))
	{
		if (Cap5GetVideoAdjust(nBoardID, nChannel, CAP5_VAC_HUE, (DWORD*)&nValue, &dwParam, &dwParam, &dwParam))
			bResult = OnConfirmGetHue(nCamera,  nValue);
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmGetHue(int nCamera, int nValue)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmGetBrightness(nCamera,  nValue);
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::DoRequestGetVideoState(int nCamera, BOOL& bPresent)
{
	BOOL bResult = FALSE;
	int nBoardID = 0;
	int nChannel = 0;
	if (GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel))
	{
		DWORD dwVideoMask = 0;
		if (Cap5GetVideoStatus(nBoardID, m_pBdInfo[nBoardID].uMaxChannel, &dwVideoMask))
		{
			bPresent = TSTBIT(dwVideoMask, nChannel);
			bResult = OnConfirmGetVideoState(nCamera, bPresent);
		}
	}

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmGetVideoState(int nCamera, BOOL bPresent)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmGetVideoState(nCamera, bPresent);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Funktioniert nur bis zu 32 Relays, da intern eine DWORD Maske verwendet wird.
BOOL CUDP::DoRequestSetRelay(DWORD dwMask)
{
	DWORD dwTempMask = dwMask^0xffffffff;

	for (int  nBoardID = 0; nBoardID < (int)m_SystemInfo.uNumOfBoard; nBoardID++)
	{
		Cap5SetDO(nBoardID, m_pBdInfo[nBoardID].uMaxDO, &dwTempMask);
		dwTempMask=dwTempMask>>m_pBdInfo[nBoardID].uMaxDO;
	}
	return OnConfirmSetRelay(dwMask);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Funktioniert nur bis zu 32 Relays, da intern eine DWORD Maske verwendet wird.
BOOL CUDP::OnConfirmSetRelay(DWORD dwMask)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmSetRelay(dwMask);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Funktioniert nur bis zu 32 Relays, da intern eine DWORD Maske verwendet wird.
BOOL CUDP::DoRequestGetRelay(DWORD& dwCompleteMask)
{
	DWORD dwMask = 0;
	DWORD dwRelayCounter = 0;
	for (int  nBoardID = 0; nBoardID < (int)(int)m_SystemInfo.uNumOfBoard; nBoardID++)
	{
		Cap5GetDO(nBoardID, m_pBdInfo[nBoardID].uMaxDO, &dwMask);
		dwMask = (dwMask^0xffffffff); // 0: Relay geschlossen, 1=Relay offen
		dwCompleteMask |= dwMask<<dwRelayCounter;
		dwRelayCounter += m_pBdInfo[nBoardID].uMaxDO;
	}
	return OnConfirmGetRelay(dwMask);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmGetRelay(DWORD dwMask)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmGetRelay(dwMask);

	return TRUE;
}

// Setzt die aktive Kamera auf 'nActiveFps' Framerate. Der Rest der 100 Frames/Sekunde wird gleichmäßig
// auf alle Kameras mit Videosignal aufgeteilt. 
// Bei den Board, die nicht die aktive kamera enthalten, werden die gesamten verfügbaren 100 Frames/Sekunde
// gleichgmäßig aufgeteilt.
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::UpdateFramerateCalculation(int nActiveCamera, int nActiveFps)
{
	BOOL bResult = FALSE;

	if ((nActiveCamera >= 0) && (nActiveCamera < MAX_CAMERAS))
	{
		int nActiveBoardID = 0;
		int nActiveChannel = 0;

		// BoardID und Channel der aktiven Kamera ermitteln
		GetBoardIDandChannelFromCamera(nActiveCamera, nActiveBoardID, nActiveChannel);
		
		// Framerate für alle vorhandenen Boards neu berechnen
		for (int nBoardID = 0; nBoardID < (int)m_SystemInfo.uNumOfBoard; nBoardID++)
		{
			// Ermittle alle aktiven Kameras (Kameras mit Videosignal) eines Boards
			int nActiveCameras = 0;
			for (int nChannel = 0; nChannel < (int)m_pBdInfo[nBoardID].uMaxChannel; nChannel++)
			{
				int nCamera = 0;
				GetCameraFromBoardIDandChannel(nBoardID, nChannel, nCamera);
				if (m_bVideoStatus[nCamera])
					nActiveCameras++;
			}

			int nRemainingFps = 0;
			int nRemainingCams = 0;

			// Das Board, das die aktive Kamera enthält muß extra behandelt werden.
			if (nBoardID == nActiveBoardID)
			{
				nRemainingFps = m_nMaxBoardFps-nActiveFps;
				nRemainingCams = nActiveCameras-1;
			}
			else
			{
				nRemainingFps = m_nMaxBoardFps;
				nRemainingCams = nActiveCameras;
			}

			if (m_pFpsList[nBoardID] == NULL)
			{
				m_pFpsList[nBoardID] = new BYTE[m_pBdInfo[nBoardID].uMaxChannel];
				ZeroMemory(m_pFpsList[nBoardID], m_pBdInfo[nBoardID].uMaxChannel);
			}

			// Frameraten Tabelle berechnen
			for (int nChannel = 0; nChannel < (int)m_pBdInfo[nBoardID].uMaxChannel; nChannel++)
			{
				int nCamera = 0;
				GetCameraFromBoardIDandChannel(nBoardID, nChannel, nCamera);
				if (m_bVideoStatus[nCamera] && (nCamera != nActiveCamera))
				{
					double fFps = (double)nRemainingFps / (double)nRemainingCams;
					m_pFpsList[nBoardID][nChannel] = (BYTE)(int)min((fFps+0.5), m_nMaxChannelFps);
					nRemainingFps -= m_pFpsList[nBoardID][nChannel];
					nRemainingCams--;
					if (nRemainingFps <= 0)
						break;
				}
			}

			// Der aktive Channel soll mit 25fps angezeigt werden.
			if (nBoardID == nActiveBoardID)
				m_pFpsList[nBoardID][nActiveChannel] = (BYTE)nActiveFps;
	
			// Frameraten neu setzen.
			bResult = Cap5SetFrameRate(nBoardID, CAP5_FRC_USER_FIXED, m_pFpsList[nBoardID], m_pBdInfo[nBoardID].uMaxChannel);

			// Berechnete Frameraten zur Kontrolle ausgeben.
			int nSum = 0;
			CString sList;
			CString sValue;
			WK_TRACE(_T("Liste der Frameraten für Board %d\n"), nBoardID);
			for (int nChannel= 0; nChannel< (int)m_pBdInfo[nBoardID].uMaxChannel; nChannel++)
			{
				sValue.Format(_T("%d "), (BYTE)m_pFpsList[nBoardID][nChannel]);
				sList += sValue;

				nSum+=(BYTE)m_pFpsList[nBoardID][nChannel];

				int nCamera = 0;
				GetCameraFromBoardIDandChannel(nBoardID, nChannel, nCamera);
				OnConfirmSetFramerate(nCamera, m_pFpsList[nBoardID][nChannel]);

			}
			WK_TRACE(_T("%s Summe=%d\n"), sList, nSum);
		}
	}

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::DoRequestSetFramerate(int nCamera, int nFps)
{
	BOOL bResult = FALSE;

	int nBoardID = 0;
	int nChannel = 0;

	// BoardID und Channel der Kamera ermitteln
	GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel);
	
	// Tabelle mit den Frameraten anlegen und mit '0' initialisieren.
	if (m_pFpsList[nBoardID] == NULL)
	{
		m_pFpsList[nBoardID] = new BYTE[m_pBdInfo[nBoardID].uMaxChannel];
		ZeroMemory(m_pFpsList[nBoardID], m_pBdInfo[nBoardID].uMaxChannel);
	}

	m_pFpsList[nBoardID][nChannel] = (BYTE)nFps;

	// Frameraten neu setzen.
	bResult = Cap5SetFrameRate(nBoardID, CAP5_FRC_USER_FIXED, m_pFpsList[nBoardID], m_pBdInfo[nBoardID].uMaxChannel);

	// Berechnete Frameraten zur Kontrolle ausgeben.
	int nSum = 0;
	CString sList;
	CString sValue;
	WK_TRACE(_T("Liste der Frameraten für Board %d\n"), nBoardID);
	for (int nChannel= 0; nChannel< (int)m_pBdInfo[nBoardID].uMaxChannel; nChannel++)
	{
		sValue.Format(_T("%d "), (BYTE)m_pFpsList[nBoardID][nChannel]);
		sList += sValue;

		nSum+=(BYTE)m_pFpsList[nBoardID][nChannel];

		int nCamera = 0;
		GetCameraFromBoardIDandChannel(nBoardID, nChannel, nCamera);
		OnConfirmSetFramerate(nCamera, m_pFpsList[nBoardID][nChannel]);

	}
	WK_TRACE(_T("%s Summe=%d\n"), sList, nSum);


	if (bResult)
		OnConfirmSetFramerate(nCamera, nFps);

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::ResetFramerates()
{
	BOOL bResult = FALSE;

	// Framerate für alle vorhandenen Boards erneut setzen
	for (int nBoardID = 0; nBoardID < (int)m_SystemInfo.uNumOfBoard; nBoardID++)
	{
		if (m_pFpsList[nBoardID] != NULL)
			bResult = Cap5SetFrameRate(nBoardID, CAP5_FRC_USER_FIXED, m_pFpsList[nBoardID], m_pBdInfo[nBoardID].uMaxChannel);
	}

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmSetFramerate(int nCamera, int nFps)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmSetFramerate(nCamera, nFps);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::DoRequestGetFramerate(int nCamera, int& nFps)
{
	BOOL bResult = FALSE;

	int nBoardID = 0;
	int nChannel = 0;

	// BoardID und Channel der Kamera ermitteln
	GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel);
	
	// Tabelle mit den Frameraten anlegen und mit '0' initialisieren.
	if (m_pFpsList[nBoardID] != NULL)
	{
		nFps = m_pFpsList[nBoardID][nChannel];
		bResult = OnConfirmGetFramerate(nCamera, nFps);
	}

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmGetFramerate(int nCamera, int nFps)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmGetFramerate(nCamera, nFps);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CUDP::OnDlgMdConfig(HWND hWnd)
{
	if (m_pMotionDetection)
		m_pMotionDetection->OpenConfigDlg(hWnd);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUDP::LoadConfig()
{
	_TCHAR szBuffer[512];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	// Angaben über zu protokollierene Funktionen aus der Registry
//	ReadDebugConfiguration();

	// Den physikalischen Portanschluß aller Kameras einlesen
	for (int  nCamera = 0; nCamera < GetAvailableCameras(); nCamera++)
	{
		wsprintf(szBuffer, _T("Mapping\\Source%02d"), nCamera);
				
		// Jede Kamera hat ihre eigenen Einstellungen
		DoRequestSetBrightness(nCamera, ReadConfigurationInt(szBuffer, _T("Brightness"), 127, m_sIniFile));
		DoRequestSetContrast(nCamera, ReadConfigurationInt(szBuffer, _T("Contrast"), 127, m_sIniFile));
		DoRequestSetSaturation(nCamera, ReadConfigurationInt(szBuffer, _T("Saturation"), 127, m_sIniFile));
		DoRequestSetHue(nCamera, ReadConfigurationInt(szBuffer, _T("Hue"), 127, m_sIniFile));
	}

	WK_TRACE(_T("\n"));
 
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CUDP::DoRequestSetAlarmEdge(DWORD dwEdgeMask)
{
	m_dwCompleteEdgeMask = dwEdgeMask;

	return OnConfirmSetAlarmEdge(m_dwCompleteEdgeMask);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmSetAlarmEdge(DWORD dwEdgeMask)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmSetAlarmEdge(dwEdgeMask);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
DWORD CUDP::DoRequestGetAlarmEdge()
{
	OnConfirmGetAlarmEdge(m_dwCompleteEdgeMask);

	return m_dwCompleteEdgeMask;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmGetAlarmEdge(DWORD dwEdgeMask)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmGetAlarmEdge(dwEdgeMask);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CUDP::GetCurrentAlarmState()
{
	return m_dwCompleteSensorMask;
}

/////////////////////////////////////////////////////////////////////////////
__int64 CUDP::ScanForCameras()
{
	__int64 i64CompleteState = 0;
	
	for (int nCamera = 0; nCamera < GetAvailableCameras(); nCamera++)
	{
		if (m_bVideoStatus[nCamera])
			i64CompleteState = SETBIT(i64CompleteState, nCamera); 
		else	
			i64CompleteState = CLRBIT(i64CompleteState, nCamera); 
	}
	
	return i64CompleteState;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUDP::ActivateMotionDetection(int nCamera)
{
	BOOL bResult = FALSE;

	if (m_pMotionDetection)
		bResult = m_pMotionDetection->ActivateMotionDetection((WORD)nCamera);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUDP::DeactivateMotionDetection(int nCamera)
{
	BOOL bResult = FALSE;

	if (m_pMotionDetection)
		bResult = m_pMotionDetection->DeactivateMotionDetection((WORD)nCamera);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUDP::SetMDMaskSensitivity(int nCamera, const CString &sLevel)
{
	if (!m_pMotionDetection)
		return FALSE;

	return m_pMotionDetection->SetMDMaskSensitivity(nCamera, sLevel);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUDP::SetMDAlarmSensitivity(int nCamera, const CString &sLevel)
{
	if (!m_pMotionDetection)
		return FALSE;

	return m_pMotionDetection->SetMDAlarmSensitivity((WORD)nCamera, sLevel);
}

/////////////////////////////////////////////////////////////////////////////
CString CUDP::GetMDMaskSensitivity(int nCamera)
{
	CString sLevel = _T("Unknown");

	if (m_pMotionDetection)
	{
		sLevel = m_pMotionDetection->GetMDMaskSensitivity((WORD)nCamera);
	}

	return sLevel;
}

/////////////////////////////////////////////////////////////////////////////
CString CUDP::GetMDAlarmSensitivity(int nCamera)
{
	CString sLevel = _T("Unknown");

	if (m_pMotionDetection)
	{
		sLevel = m_pMotionDetection->GetMDAlarmSensitivity((WORD)nCamera);
	}

	return sLevel;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::GetMask(int nCamera, CIPCActivityMask& mask)
{
	if (!m_pMotionDetection)
		return FALSE;

	return m_pMotionDetection->GetMask((WORD)nCamera, mask);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::SetMask(int nCamera, const CIPCActivityMask& mask)
{
	if (!m_pMotionDetection)
		return FALSE;

	return m_pMotionDetection->SetMask((WORD)nCamera, mask);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUDP::SetCameraToAnalogOut(int nCamera)
{
	int nBoardID = 0;
	int nChannel = 0;

	GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel);
	return Cap5SetExtVideoOut(nBoardID, nChannel);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUDP::SetLEDState(eLEDState ledState)
{
	BOOL bResult = FALSE;

	WK_TRACE_WARNING(_T("CCodec::SetLEDState Not yet implemented\n"));

	switch (ledState)
	{
		case eLED1_On:
			bResult = TRUE;
			break;
		case eLED1_Blink:
			bResult = TRUE;
			break;
		case eLED2_On:
			bResult = TRUE;
			break;
		case eLED2_Off:
			bResult = TRUE;
			break;
	}

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
CSize CUDP::DoRequestGetImageSize(int nCamera)
{
	int nBoardID = 0;
	int nChannel = 0;
	DWORD dwImgeSize = 0;

	if (GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel))
		Cap5GetImageSize(nBoardID, nChannel, &dwImgeSize);
	
	CSize ImageSize(CMN5_GETIMGWIDTH(dwImgeSize), CMN5_GETIMGHEIGHT(dwImgeSize));
	OnConfirmGetImageSize(nCamera, ImageSize);

	return ImageSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmGetImageSize(int nCamera, CSize ImageSize)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmGetImageSize(nCamera, ImageSize);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUDP::DoRequestSetImageSize(int nCamera, CSize ImageSize)
{
	BOOL bResult = FALSE;

	int nBoardID = 0;
	int nChannel = 0;

	if (CheckResolution(nCamera, ImageSize))
	{
		if (GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel))
		{
			bResult = Cap5Stop();
			bResult &= Cap5SetImageSize(nBoardID, nChannel, CMN5_MAKEIMGSIZE(ImageSize.cx, ImageSize.cy));
			bResult &= Cap5Run();
		
			if (bResult)
				OnConfirmSetImageSize(nCamera, ImageSize);
		}

		ResetFramerates();
	}
	else // Diese Auflösung wird nicht unterstützt, es soll dann im Confirm die aktuelle Auflösung zurückgeliefert werden.
	{
		DWORD dwImgeSize = 0;
		if (GetBoardIDandChannelFromCamera(nCamera, nBoardID, nChannel))
			Cap5GetImageSize(nBoardID, nChannel, &dwImgeSize);
		
		CSize ImageSize(CMN5_GETIMGWIDTH(dwImgeSize), CMN5_GETIMGHEIGHT(dwImgeSize));
		OnConfirmSetImageSize(nCamera, ImageSize);
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUDP::OnConfirmSetImageSize(int nCamera, CSize ImageSize)
{
	if (m_pMainDlg)
		m_pMainDlg->OnConfirmSetImageSize(nCamera, ImageSize);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::TriggerWatchDog()
{
	int nTi = ReadConfigurationInt(_T("General"), _T("WatchDogTimeout"), -1, m_sIniFile);

	BOOL bResult = FALSE;

	if (nTi != -1)
	{
		if (Cap5SetWatchdog(0, CMN5_WC_ENABLE, 0, 0))
			bResult = Cap5SetWatchdog(0, CMN5_WC_SET_TIMEOUT_VALUE, nTi, 0);
	}
	else	// Watchdog ausschalten
		bResult = Cap5SetWatchdog(0, CMN5_WC_DISABLE, 0, 0); 
	
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::TriggerPiezo()
{
	BOOL bResult = FALSE;
	int nState = ReadConfigurationInt(_T("General"), _T("PiezoState"), -1, m_sIniFile);

	switch (nState)
	{
		case 0:
			Cap5SetWatchdog(0, CMN5_WC_SET_BUZZER_TIMEOUT_VALUE, 0, 0);
			break;
		case 1:
			Cap5SetWatchdog(0, CMN5_WC_SET_BUZZER_TIMEOUT_VALUE, 1, 0);
			break;
		default:
			break;
	}
	
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::InitConversionTables()
{
	BOOL bResult = FALSE;
	int nBDId= 0;
	int nChannel = 0;
	int nCamera = 0;

	m_bConversionTablesReady = FALSE;
	for (int nCam = 0; nCam < MAX_BOARDS*MAX_CHANNEL_PER_BOARD; nCam++)
	{	
		if (GetBoardIDandChannelFromCamera(nCam, nBDId, nChannel))
		{
			m_nBoardID[nCam] = nBDId;
			m_nChannel[nCam] = nChannel;

			if (GetCameraFromBoardIDandChannel(nBDId, nChannel, nCamera))
			{
				m_nCamera[nBDId][nChannel] = nCamera;
				TRACE(_T("Camera=%d -> BordID=%d Channel=%d ==> Camera=%d\n"), nCam, nBDId, nChannel, nCamera);
			}
		}
	}
	// Anzahl der Kameras, Relays und Alarmeingänge ermitteln.
	m_nAvailableCameras = 0;
	m_nAvailableRelays  = 0;
	m_nAvailableInputs  = 0;
	for (int nBoardID = 0; nBoardID < (int)m_SystemInfo.uNumOfBoard; nBoardID++)
	{
		m_nAvailableCameras += m_pBdInfo[nBoardID].uMaxChannel;
		m_nAvailableRelays  += m_pBdInfo[nBoardID].uMaxDO;
		m_nAvailableInputs  += m_pBdInfo[nBoardID].uMaxDI;
	}

	bResult = TRUE;
	m_bConversionTablesReady = TRUE;

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::GetBoardIDandChannelFromCamera(int nCamera, int& nBoardID, int& nChannel)
{
	BOOL bResult = FALSE;
	if (m_bConversionTablesReady)
	{
		nBoardID = m_nBoardID[nCamera];
		nChannel = m_nChannel[nCamera];
		bResult  = TRUE;
	}
	else
	{
		DWORD dwBd = 0;
		DWORD dwI = 0;

		int nCameraCounter = 0;
		for (dwBd = 0; dwBd < m_SystemInfo.uNumOfBoard && !bResult; dwBd++)
		{
			for (dwI = 0; dwI < m_pBdInfo[dwBd].uMaxChannel && !bResult; dwI++)
			{
				if (nCameraCounter++ == nCamera)
					bResult = TRUE;
			}
		}
		if (bResult)
		{
			nBoardID	= dwBd-1;
			nChannel	= dwI-1;
		}
	}
	
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::GetCameraFromBoardIDandChannel(int nBoardID, int nChannel, int& nCamera)
{
	BOOL bResult = FALSE;
	if (m_bConversionTablesReady)
	{
		nCamera = m_nCamera[nBoardID][nChannel];
		bResult = TRUE;
	}
	else
	{
		int nCameraCounter = 0;

		for (int nBID = 0; nBID < (int)m_SystemInfo.uNumOfBoard; nBID++)
		{
			if (nBID == nBoardID)
			{
				nCameraCounter += nChannel;
				bResult = TRUE;
				break;
			}
			else
				nCameraCounter += m_pBdInfo[nBID].uMaxChannel;
		}

		if (bResult)
			nCamera = nCameraCounter;
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int CUDP::GetAvailableCameras()
{
	return m_nAvailableCameras;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int CUDP::GetAvailableRelays()
{
	return m_nAvailableRelays;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int CUDP::GetAvailableInputs()
{
	return m_nAvailableInputs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CUDP::SendTestImage(int nCamera)
{
	BOOL bResult = FALSE; 

	QIMAGE QImage;

	// imgStruktur ergänzen.
	QImage.TimeStamp.GetLocalTime();
	QImage.nWidth					= 352;
	QImage.nHeight					= 288;
	QImage.pPMDoints				= NULL;

	QImage.lpBMI = (LPBITMAPINFO)new BYTE[m_dwTestImageLen+sizeof(BITMAPINFO)+1024];
	if (QImage.lpBMI)
	{
		QImage.nCamera = nCamera;
		ZeroMemory(QImage.lpBMI, sizeof(BITMAPINFO));
		QImage.lpBMI->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
		QImage.lpBMI->bmiHeader.biWidth			= QImage.nWidth;
		QImage.lpBMI->bmiHeader.biHeight		= QImage.nHeight;
		QImage.lpBMI->bmiHeader.biPlanes		= 1;
		QImage.lpBMI->bmiHeader.biBitCount		= 16; 
		QImage.lpBMI->bmiHeader.biCompression	= mmioFOURCC('J','P','E','G');
		QImage.lpBMI->bmiHeader.biSizeImage		= m_dwTestImageLen;
	
		CopyMemory(QImage.lpBMI->bmiColors, m_lpTestImage, m_dwTestImageLen);
	
		QImage.ct = COMPRESSION_JPEG;
		QImage.TimeStamp.GetLocalTime();
	}

	// OnCapture aufrufen
	if (m_pMainDlg)
		bResult = m_pMainDlg->OnCapture(&QImage);

	// Bitmap wieder zerstören.
	WK_DELETE(QImage.lpBMI);

	return bResult;
}
