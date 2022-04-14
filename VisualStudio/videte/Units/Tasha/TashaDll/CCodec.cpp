/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CCodec.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaDll/CCodec.cpp $
// CHECKIN:		$Date: 11.02.05 9:04 $
// VERSION:		$Revision: 252 $
// LAST CHANGE:	$Modtime: 11.02.05 8:57 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CCodec.h"
#include "CircularBuffer.h"
#include "CEEProm.h"
#include "CSerialNumberDlg.h"
#include "TashaErrors.h"

#include "resource.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CWK_Timer theTimer;

CCriticalSection CCodec::m_csSendMessage;
CCriticalSection CCodec::m_csReceiveMessage;

LARGE_INTEGER CWK_Timer::m_CyclesPerMicroSecond = {0};

// Buffersize
#define SIZE 255 		

/////////////////////////////////////////////////////////////////////////////
inline __int64 ReadRDTSC()
{
	static LARGE_INTEGER lRDTSC;
	_asm {             
		_asm push eax  
		_asm push edx  
		_asm _emit 0Fh 
		_asm _emit 31h 
		_asm mov lRDTSC.LowPart, eax 
		_asm mov lRDTSC.HighPart, edx
		_asm pop edx            
		_asm pop eax            
	} 
	return (__int64)lRDTSC.QuadPart;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CCodec::CCodec(int nBoardID, const CString &sAppIniName)
{
#ifdef USE_MULTITHREADED_MESSAGE_HANDLING
	ML_TRACE(_T("Using multi threaded messagehandling!\n"));
#else
	ML_TRACE(_T("Using single threaded messagehandling!\n"));
#endif

	m_hDSP							= NULL;
	m_nDIPState						= 0;
	m_nBoardID						= nBoardID;
	m_nPendingBF535Pings			= 0;

	m_eEncoderState					= eEncoderStateUnvalid;
	m_bEstablishMessageChannel		= FALSE;
	m_bFrameSync					= TRUE;
	m_bRun							= TRUE;
	m_dwCamScanMask					= 0x0000;
	m_dwCamMask						= 0xffff; 			 
	m_dwValidDataPacketMask			= 0xffff; 			 
	m_wVariationRegister			= 0;

	m_sAppIniName					= sAppIniName;
	m_pFrameBuffer					= NULL;

	m_memTransferBuffer.dwLen		= 0;
	m_memTransferBuffer.pLinAddr	= NULL;
	m_memTransferBuffer.pPhysAddr	= NULL;
	
	m_memFrameBuffer.dwLen			= 0;
	m_memFrameBuffer.pLinAddr		= NULL;
	m_memFrameBuffer.pPhysAddr		= NULL;
	
	m_pReadDataThread				= NULL;
	m_pWaitForExternalEventThread	= NULL;
	m_hNewDataSemaphore				= NULL;

	m_bSilence						= TRUE;
	
	m_dwBF535RecoveryTimeout		= 10*1000;	
	m_dwBF533RecoveryTimeout		= 4*1000;
	m_dwStreamRecoveryTimeout		= 4*1000;

	m_hWatchDogTriggerEvent			= NULL;
	m_hShutDownEvent				= NULL;
	m_hPiezoEvent					= NULL;

	DeleteEntry(_T("Recovery"), _T("BF533RecoveryTimeout"), m_sAppIniName);
	DeleteEntry(_T("Recovery"), _T("BF535RecoveryTimeout"), m_sAppIniName);
	DeleteEntry(_T("Recovery"), _T("StreamRecoveryTimeout"), m_sAppIniName);
	DeleteEntry(_T("Recovery"), _T("WatchdogTimeout"), m_sAppIniName);

	// Recovery timeouts
	m_nBF533BootAttempts	  = ReadConfigurationInt(_T("Recovery"), _T("BF533BootAttempts"), 3, m_sAppIniName);
	m_dwBF533RecoveryTimeout  = ReadConfigurationInt(_T("Recovery"), _T("BF533RecoveryTimeout"), m_dwBF533RecoveryTimeout, m_sAppIniName);
	m_dwBF535RecoveryTimeout  = ReadConfigurationInt(_T("Recovery"), _T("BF535RecoveryTimeout"), m_dwBF535RecoveryTimeout, m_sAppIniName);
	m_dwStreamRecoveryTimeout = ReadConfigurationInt(_T("Recovery"), _T("StreamRecoveryTimeout"), m_dwStreamRecoveryTimeout, m_sAppIniName);
	m_bIgnoreDIP			  = ReadConfigurationInt(_T(""), _T("IgnoreDIPs"),  0, _T("TashaUnit"));

	ML_TRACE(_T("BF533RecoveryTimeout = %d seconds\n"), m_dwBF533RecoveryTimeout/1000);
	ML_TRACE(_T("BF535RecoveryTimeout = %d seconds\n"), m_dwBF535RecoveryTimeout/1000);
	ML_TRACE(_T("StreamRecoveryTimeout= %d seconds\n"), m_dwStreamRecoveryTimeout/1000);

	// Tracemeldungen (An=TRUE / Aus=FALSE)
	m_bTraceStartEncoder			= FALSE;
	m_bTraceStopEncoder				= FALSE;
	m_bTracePauseEncoder			= FALSE;
	m_bTraceResumeEncoder			= FALSE;
	m_bTraceSetBrightness			= FALSE;
	m_bTraceSetContrast				= FALSE;
	m_bTraceSetSaturation			= FALSE;
	m_bTraceSetRelais				= FALSE;
	m_bTraceSetAlarmEdge			= FALSE;
	m_bTraceGetBrightness			= FALSE;
	m_bTraceGetContrast				= FALSE;
	m_bTraceGetSaturation			= FALSE;
	m_bTraceGetRelais				= FALSE;
	m_bTraceGetAlarmEdge			= FALSE;
	m_bTraceAlarmIndication			= FALSE;
	m_bTraceAnalogOutSwitch			= FALSE;
	m_bTraceSetMDTreshold			= FALSE;
	m_bTraceSetMaskTreshold			= FALSE;
	m_bTraceSetMaskIncrement		= FALSE;
	m_bTraceSetMaskDelay			= FALSE;
	m_bTraceChangePermanentMask		= FALSE;
	m_bTraceSetPermanentMask		= FALSE;
	m_bTraceGetMDTreshold			= FALSE;
	m_bTraceGetMaskTreshold			= FALSE;
	m_bTraceGetMaskIncrement		= FALSE;
	m_bTraceGetMaskDelay			= FALSE;
	m_bTraceClearPermanentMask		= FALSE;
	m_bTraceInvertPermanentMask		= FALSE;
	m_bTraceSetAnalogOut			= FALSE;
	m_bTraceGetAnalogOut			= FALSE;
	m_bTraceSetTerminationState		= FALSE;
	m_bTraceGetTerminationState		= FALSE;
	m_bTraceSetVideoEnableState		= FALSE;
	m_bTraceGetVideoEnableState		= FALSE;
	m_bTraceSetCrosspointEnableState= FALSE;
	m_bTraceGetCrosspointEnableState= FALSE;
	m_bTraceEnableWatchdog			= FALSE;
	m_bTraceTriggerWatchdog			= FALSE;
 	m_bTraceSetPowerLED				= FALSE;
 	m_bTraceSetResetLED				= FALSE;
	m_bTraceGetVariationRegister	= FALSE;
	m_bTraceSetEncoderParam			= FALSE;
	m_bTraceGetEncoderParam			= FALSE;
	m_bTraceSPORTCheckSummError		= FALSE;
	m_bTraceSPICheckSummError		= FALSE;
	m_bTraceSPICommandTimeout		= FALSE;
	m_bTraceSPICommandPending		= FALSE;
	m_bTraceMissingFields			= FALSE;
	m_bTraceReadFromEEProm			= FALSE;
	m_bTraceWriteToEEProm			= FALSE;
	m_bTraceBF533DebugText			= FALSE;
	m_bTraceGetDIPState				= FALSE;
	m_bTraceGetAlarmState			= FALSE;
	m_bTraceRequestSingleFrame		= FALSE;
	m_bTraceSetNR					= FALSE;
	m_bTraceGetNR					= FALSE;
	
	m_lpTestImage					= NULL;
	m_dwTestImageLen				= 0;

	// Testbild aus der Resource laden
	HINSTANCE hInst= AfxFindResourceHandle(MAKEINTRESOURCE(IDR_BIN_TEST_IMAGE), _T("BINRES"));
	HRSRC hRc = FindResource(hInst, MAKEINTRESOURCE(IDR_BIN_TEST_IMAGE), _T("BINRES"));
	if (hRc)
	{
		HGLOBAL hJpegResource	= LoadResource(hInst, hRc);
		if (hJpegResource) 
		{
			m_dwTestImageLen	= SizeofResource(hInst, hRc);
			m_lpTestImage		= (BYTE*)LockResource(hJpegResource);
		}
		else
			ML_TRACE_ERROR(_T("Testbild konnte nicht geladen werden\n"));
	}
	else
		ML_TRACE_ERROR(_T("Testbild konnte nicht geladen werden\n"));

	// Angaben über zu protokollierene Funktionen aus der Registry
	ReadDebugConfiguration();

	// Defaultmäßig sind Netz und ISDN und Backup sperrren
	WriteConfigurationInt(_T("EEProm"), _T("ISDN"), 0, m_sAppIniName);
	WriteConfigurationInt(_T("EEProm"), _T("NET"), 0, m_sAppIniName);
	WriteConfigurationInt(_T("EEProm"), _T("BackUp"), 0, m_sAppIniName);
	WriteConfigurationInt(_T("EEProm"), _T("BackUpToDVD"), 0, m_sAppIniName);
	WriteConfigurationString(_T("EEProm"), _T("SN"), _T(""), m_sAppIniName);
	m_bSilence = ReadConfigurationInt(_T("Debug"), _T("Silence"), m_bSilence, m_sAppIniName);

	// Buffer enthält die Bilddaten aller 8 Channels
	m_pFrameBuffer = (BYTE*) new BYTE[CHANNEL_SIZE*CHANNEL_COUNT/2+1024];

	// Jeder Kanal erhält zur Framesynchronisation einen Circularbuffer
	for (int nChannel = 0; nChannel < CHANNEL_COUNT; nChannel++)
	{
		m_pCirBuff[nChannel] = new CCircularBuffer(nChannel, MAGIC_MARKER_START, MAGIC_MARKER_END, 4*1024*1024);
		m_dwLastValidDataPacketTC[nChannel] = WK_GetTickCount();
		ZeroMemory(&m_PermanentMask[nChannel], sizeof(MASK_STRUCT));
		ZeroMemory(&m_AdaptiveMask[nChannel], sizeof(MASK_STRUCT));
		m_eVideoStreamState[nChannel] = VideoStreamValid;
		m_nPFrameCounter[nChannel] = 0;
	}

	// Diese Semaphore teilt der 'ReadDataThread' mit, daß Bilddaten abgeholt werden können
	m_hNewDataSemaphore = CreateSemaphore(NULL, 0, 100, NULL);

	if (!m_hNewDataSemaphore)
		ML_TRACE_ERROR(_T("Create NewDataSemaphore failed\n"));

	// Dieser Thread holt die Daten aus den Circularbuffern und reicht sie weiter.
	m_pReadDataThread = AfxBeginThread(ReadDataThread, this);
	if (m_pReadDataThread)
	{
		m_pReadDataThread->m_bAutoDelete = FALSE;	
		m_pReadDataThread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);	
	}


	// Nur die erste Tasha soll auf die externen Events lauschen
	if (m_nBoardID == 1)
	{
		// Dieses Event veranlaßt das Beenden der einzelnen Threads
		m_hShutDownEvent = CreateEvent(0, TRUE, FALSE, NULL);
		if (m_hShutDownEvent==NULL) 
			ML_TRACE_ERROR(_T("Create WatchDogTriggerEvent failed\n"));

		// Mit Hilfe diese Event läßt sich der Watchdog von Außen triggern
		m_hWatchDogTriggerEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,EV_TASHA_TRIGGER_WATCHDOG);
		if (m_hWatchDogTriggerEvent == NULL)
			m_hWatchDogTriggerEvent = CreateEvent(NULL, FALSE, FALSE, EV_TASHA_TRIGGER_WATCHDOG);
		else
			ML_TRACE(_T("Watchdog Trigger Event already open\n"));

		// Mit Hilfe diese Event läßt sich der Störmelder von Außen triggern
		m_hPiezoEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,EV_TASHA_PIEZO);
		if (m_hPiezoEvent==NULL) 
			m_hPiezoEvent = CreateEvent(NULL, FALSE, FALSE, EV_TASHA_PIEZO);
		else
			ML_TRACE(_T("PiezoOnOffEvent already open\n"));

		if (m_hPiezoEvent && m_hWatchDogTriggerEvent)
		{
			// Dieser Thread wartet auf externe Events.
			m_pWaitForExternalEventThread = AfxBeginThread(WaitForExternalEventThread, this);
			if (m_pWaitForExternalEventThread)
			{
				m_pWaitForExternalEventThread->m_bAutoDelete = FALSE;	
				m_pWaitForExternalEventThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);	
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
CCodec::~CCodec()
{	
	TRACE(_T("CCodec::~CCodec()\n"));

	m_bRun = FALSE;


	// Warte bis 'ReadDataThread' beendet ist.
	if (m_pReadDataThread)
		WaitForSingleObject(m_pReadDataThread->m_hThread, 2000);
	WK_DELETE(m_pReadDataThread); //Autodelete = FALSE;

	// Framebuffer freigeben
	WK_DELETE(m_pFrameBuffer);

	// Circularbuffer freigeben
	for (int nChannel = 0; nChannel < CHANNEL_COUNT; nChannel++)
	{
		m_pCirBuff[nChannel]->Flush();
		WK_DELETE(m_pCirBuff[nChannel]);
	}

	// ShutDown-Event setzen
	if (m_hShutDownEvent)
		SetEvent(m_hShutDownEvent);

	// Warte bis 'WaitForExternalEventThread' beendet ist.
	if (m_pWaitForExternalEventThread)
		WaitForSingleObject(m_pWaitForExternalEventThread->m_hThread, 2000);
	WK_DELETE(m_pWaitForExternalEventThread); //Autodelete = FALSE;

	if (m_hWatchDogTriggerEvent)
	{
		CloseHandle(m_hWatchDogTriggerEvent);
		m_hWatchDogTriggerEvent = NULL;
	}

	if (m_hPiezoEvent)
	{
		CloseHandle(m_hPiezoEvent);
		m_hPiezoEvent = NULL;
	}

	if (m_hShutDownEvent)
	{
		CloseHandle(m_hShutDownEvent);
		m_hShutDownEvent = NULL;
	}

}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestOpenDevice()
{
	BOOL bResult = FALSE;
	ML_TRACE(_T("CCodec::DoRequestOpenDevice (BoardID=%d)\n"), m_nBoardID);

	m_nPendingBF535Pings = 0;
	if (AfxBeginThread((AFX_THREADPROC)OpenDeviceThread, this, THREAD_PRIORITY_NORMAL) != NULL)
		bResult = TRUE;

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmOpenDevice(int nBoardID, int nErrors)
{
	// Should be overridden
	ML_TRACE(_T("CCodec::OnConfirmOpenDevice (BoardID=%d, Errors=%d)\n"), nBoardID, nErrors);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
UINT CCodec::OpenDeviceThread(LPVOID pData)
{
	CCodec* pThis = (CCodec*)pData;

	XTIB::SetThreadName(_T("OpenDeviceThread"));

	enum mdsErrorCode Status;
	int nDllVersion		= 0;
	int	nDriverVersion	= 0; 
	int nFirmwareVersion= 0;
	int	nErrors			= 0; 
	int nDeviceInstance = -1;
	CString			sImagePath(pThis->GetHomeDir()+TASHA_FIRMWARE);

	// Suche die richtige Tashakarte, anhand der DIP-Schalter
	nDeviceInstance = pThis->SearchTashaBoard(pThis->m_nBoardID);

	ML_TRACE(_T("CCodec::OpenDeviceThread sImagePath1=%s (DeviceInstance=%d)\n"), sImagePath, nDeviceInstance);

	// Den Eintrag zu nächst löschen. Nach erfolgreichem Test der tatsächlichen DIP-Schalter Stellung
	// wird dieser Eintrag aktualisiert.
	DeleteEntry(_T(""), _T("DeviceInstance"), pThis->m_sAppIniName);
	
	if (nDeviceInstance != -1)
	{
		// MDS-Treiber öffnen
		Status = mdsBoardOpen(MDS_BOARD_HAWK35, &nDeviceInstance, 0, 0, &pThis->m_hDSP);

		if (Status == MDS_ERROR_NO_ERROR)
		{
			// Get DLL version
			Status = mdsBoardGetLibraryVersion(&nDllVersion);
	  
			// Check return value
			if (Status == MDS_ERROR_NO_ERROR)
			{
				// Get driver version
				Status = mdsBoardGetDriverVersion(pThis->m_hDSP, &nDriverVersion);

				// Check return value
				if (Status == MDS_ERROR_NO_ERROR)
				{
					// Get firmware version
					//Status = mdsBoardGetFirmwareVersion(m_hDSP,	&nFirmwareVersion, MDS_DSP_INDEX_A);	

					// Check return value
					if (Status == MDS_ERROR_NO_ERROR)
					{ 
						//Informational Message
						CString sText;
						sText.Format(_T("DLL Version: major %d minor %d"), (nDllVersion & 0x0000FF00) >> 8,
																		(nDllVersion & 0x03FF0000) >> 16);
						ML_TRACE(_T("%s\n"), sText);

						sText.Format(_T("Driver Version: major %d minor %d build %d"), (nDriverVersion & 0x0000FF00) >> 8,
																					(nDriverVersion & 0x000000FF),
																					(nDriverVersion & 0x03FF0000) >> 16);
						ML_TRACE(_T("%s\n"), sText);

 						sText.Format(_T("Firmware Version: major %d minor %d wee %d"), (nFirmwareVersion & 0x00ff0000) >> 16,
																					(nFirmwareVersion & 0x0000FF00) >> 8,
																					(nFirmwareVersion & 0x000000ff));
						CWK_String sTemp(sImagePath);
						char* pAnsiString = (char*)(LPCSTR)sTemp;
						Status = mdsBoardDownLoad(pThis->m_hDSP, pAnsiString, MDS_DSP_INDEX_A);
						ML_TRACE(_T("CCodec::SearchTashaBoard AnsiString=%s\n"), pAnsiString);

						// Check return value
						if (Status == MDS_ERROR_NO_ERROR)
						{ 
							ML_TRACE(_T("Downloading Firmware <%s> to Tasha complete\n"), sImagePath);
							if (pThis->OpenMessageChannel())
							{				
								pThis->m_evTmmInitComplete.ResetEvent(); 
								if (WaitForSingleObject(pThis->m_evTmmInitComplete, 5000) == WAIT_TIMEOUT)
								{
									pThis->OnIndicateWarning(TASHA_WARNING_INIT_COMPLETE_TIMEOUT);
									ML_TRACE_WARNING(_T("HOST: TmmInitComplete timeout\n"));
									nErrors++;
								}
								else
								{
									// Alterafile laden
									if (pThis->TransferEPLDdata(pThis->GetHomeDir()+EPLD_FILENAME))
									{
										// Wenn die DIP-Schalter stellung nicht mit der BoardID übereinstimmt,
										// muß der Eintrag in der Registry gelöscht werden.
										int nDIPState = 0;
										pThis->DoRequestGetDIPState(nDIPState);
										// DIP-State zählt von '0' nRequestedBoardID hingegen von '1' aus
										if (pThis->m_bIgnoreDIP || (nDIPState+1 == pThis->m_nBoardID))
										{
											Beep(440, 100);

											// mdsDeviceInstance <-> TashaBoardnummer
											WriteConfigurationInt(_T(""), _T("DeviceInstance"), nDeviceInstance, pThis->m_sAppIniName);	
											
											// Dongle auslesen
											//pThis->ReadDongleInformation();

											// BF533 Bootloader an den BF535 schicken.
											pThis->TransferBootloader(pThis->GetHomeDir()+BOOT_LOADER_533);								

											CString sFileName;

											// Dieses Array bestimmt die Boot Reihenfolge.
											int nBootOrder[CHANNEL_COUNT] = {0,2,4,6,1,3,5,7};
											
											// Die DSPs 0, 2, 4, 6, 1, 3, 5 und 7 booten
											for (int nI = 0; nI < CHANNEL_COUNT; nI++)
											{						
												if (!pThis->TransferBF533BootData(nBootOrder[nI]), pThis->m_nBF533BootAttempts)
												{
													if (ReadConfigurationInt(_T("Recovery"), _T("HaltOnBootError"), 0, pThis->m_sAppIniName))
														nErrors++;
												}
											}

											if (pThis->DoRequestSetPCIFrameBufferAddress((DWORD)pThis->m_memFrameBuffer.pPhysAddr))
											{
												WORD wVariation = 0;
												if (pThis->DoRequestGetVariationRegister(wVariation))
													ML_TRACE(_T("VariationRegister=0x%x\n"), wVariation);

												// Watchdog aktivieren
												if (!pThis->DoRequestEnableWatchdog(pThis->m_dwBF533RecoveryTimeout))
													nErrors++;

												// Terminierungseinstellung laden und setzen.
												if (!pThis->DoRequestSetTerminationState(ReadConfigurationInt(_T("Slaves"), _T("Termination"), 0xff, pThis->m_sAppIniName)))
													nErrors++;

												// Enable/Disable VideoOP
												if (!pThis->DoRequestSetVideoEnableState(ReadConfigurationInt(_T("Slaves"), _T("VideoEnable"), 1, pThis->m_sAppIniName)))
													nErrors++;

												// Videoout Matrix laden und setzen
												if (!pThis->DoRequestSetAnalogOut(ReadConfigurationInt(_T("Crosspoint"), _T("Matrix"), 0x01020304, pThis->m_sAppIniName)))
													nErrors++;
											}
											else
												nErrors++;
										}
										else
										{
											pThis->OnIndicateWarning(TASHA_WARNING_DIP_SWITCHES_CHANGED);
											ML_TRACE_WARNING(_T("DIP-Switches seems to be changed from 0x%02x to 0x%02x\n)"), pThis->m_nBoardID-1, nDIPState);
											nErrors++;
										}
									}
									else
									{
										ML_TRACE_ERROR(_T("TransferEPLDdata failed\n"));
										nErrors++;
									}
								}
							}
							else
							{
								pThis->OnIndicateError(TASHA_ERROR_MESSAGE_CHANNEL_OPEN_FAILED);
								ML_TRACE_ERROR(_T("HOST: Can't open Message channel\n"));
								nErrors++;
							}
						}
						else
						{
							//Give more specific error information
							switch(Status)
							{		
								case MDS_ERROR_FILE_ACCESS :
									pThis->OnIndicateError(TASHA_ERROR_FILE_ACCESS);
									ML_TRACE_ERROR(_T("Failed to open %s for download.\n"), sImagePath);
									mdsBoardClose(pThis->m_hDSP);
									pThis->m_hDSP = NULL;
									break;
								default :
									pThis->OnIndicateError(TASHA_ERROR_BF535_FIRMWARE_DOWNLOAD_FAILED);
									ML_TRACE_ERROR(_T("Failed to download %s to Tasha.\n"), sImagePath);
									mdsBoardClose(pThis->m_hDSP);
									pThis->m_hDSP = NULL ;
								break;
							}
							nErrors++;
						}				
					}
					else
					{
						pThis->OnIndicateError(TASHA_ERROR_GETTING_FIRMWARE_VERSION_FAILED);
						ML_TRACE_ERROR(_T("Failed getting firmware version.\n"));
						mdsBoardClose(pThis->m_hDSP);
						pThis->m_hDSP = NULL;
						nErrors++;
					}							
				}
				else
				{	
					pThis->OnIndicateError(TASHA_ERROR_GETTING_DRIVER_VERSION_FAILED);
					ML_TRACE_ERROR(_T("Failed getting driver version.\n"));
					mdsBoardClose(pThis->m_hDSP);
					pThis->m_hDSP = NULL;
					nErrors++;
				}
			}
			else
			{ 
				pThis->OnIndicateError(TASHA_ERROR_GETTING_DLL_VERSION_FAILED);
				ML_TRACE_ERROR(_T("CCodec::CCodec Failed getting DLL version (%d)\n"), Status);
				mdsBoardClose(pThis->m_hDSP);
				pThis->m_hDSP = NULL;
				nErrors++;
			}
		}
		else
		{
			switch(Status)
			{
				case MDS_ERROR_BOARD_IN_USE :
					pThis->OnIndicateError(TASHA_ERROR_BOARD_ALREADY_IN_USE);
					ML_TRACE_ERROR(_T("Failed to open Tasha board. Board is in use by another application.\n"));
					break;     
				case MDS_ERROR_BOARD_NOT_PRESENT :
					pThis->OnIndicateError(TASHA_ERROR_BOARD_NOT_PRESENT);
					ML_TRACE_ERROR(_T("Failed to open Tasha board. Board is not present.\n"));
					break;      
				default :
					pThis->OnIndicateError(TASHA_ERROR_OPEN_TASHA_BOARD_FAILED);
					ML_TRACE_ERROR(_T("Failed to open Tasha board.\n"));
				return 1;
			}
			nErrors++;
		}
	}
	else
		nErrors++;

	pThis->OnConfirmOpenDevice(pThis->m_nBoardID, nErrors);

	return 0;
}

//////////////////////////////////////////////////////////////////////
int CCodec::SearchTashaBoard(int nRequestedBoardID)
{
	enum mdsErrorCode Status;
	BOOL			bFoundBoard = FALSE;
	int				nDeviceInstance	= -1;
	CString			sImagePath(GetHomeDir() + TASHA_FIRMWARE);

	ML_TRACE(_T("CCodec::SearchTashaBoard sImagePath1=%s (RequestBoardID=%d)\n"), sImagePath, nRequestedBoardID);

	// Adresse des 8MByte großen DMA-Buffers holen.
	if (!DAGetFramebuffer(nRequestedBoardID, m_memFrameBuffer))
	{
		AfxMessageBox(_T("Konnte Framebufferadresse nicht ermitteln\n"), MB_ICONERROR);
		return FALSE;
	}

	// 1MByte großen Transferbuffer (Altera, EEProm, BF533-Bootimage) anlegen
	if (!DAGetTransferbuffer(nRequestedBoardID, m_memTransferBuffer))
	{
		AfxMessageBox(_T("Konnte Transferbuffer nicht anlegen\n"), MB_ICONERROR);
		return FALSE;
	}

	// Suche die richtige Tashakarte, anhand der DIP-Schalter, oder aber ignoriere sie.
	if (m_bIgnoreDIP)
	{
		ML_TRACE(_T("Ignoring DIPs using BoardID for DeviceID\n"));
		return  nRequestedBoardID;
	}

	// Wenn ein Eintrag für die Treiberinstance in der Registry existiert, dann nehme diesen und breche die
	// weitere Suche ab.
	nDeviceInstance = ReadConfigurationInt(_T(""), _T("DeviceInstance"), -1, m_sAppIniName);
	if (nDeviceInstance != -1)
	{
		ML_TRACE(_T("Found 'DeviceInstance' entry '%d' in Registry...using this entry\n"), nDeviceInstance);
		return nDeviceInstance;
	}
	else
		ML_TRACE(_T("Found NO 'DeviceInstance' entry in registry. Start searching Tashaboard '%d'...\n"), nRequestedBoardID);

	// Suche aller möglichen Tashaboards nach der richtigen DIP-Stellung ab. Dazu muß die
	// Firmware in das Board übertragen und der Altera programmiert werden. 
	for (int nI = 1; ((nI <= MAX_TASHA_BOARDS) && !bFoundBoard ); nI++)
	{
		// MDS-Treiber öffnen
		nDeviceInstance = nI;
		Status = mdsBoardOpen(MDS_BOARD_HAWK35, &nDeviceInstance, 0, 0, &m_hDSP);
		if (Status == MDS_ERROR_NO_ERROR)
		{
#if (1)
			// Download program to DSP
			CWK_String sTemp(sImagePath);
			char* pAnsiString = (char*)(LPCSTR)sTemp;
			Status = mdsBoardDownLoad(m_hDSP, pAnsiString, MDS_DSP_INDEX_A);
			ML_TRACE(_T("CCodec::SearchTashaBoard AnsiString=%s\n"), pAnsiString);
#else
			// Download program to DSP
			CStringA sTemp(sImagePath);
			Status = mdsBoardDownLoad(m_hDSP, (char*)sTemp.GetBuffer(MAX_PATH), MDS_DSP_INDEX_A);
			sTemp.ReleaseBuffer(-1);
#endif			
			// Check return value
			if (Status == MDS_ERROR_NO_ERROR)
			{ 
				if (OpenMessageChannel())
				{				
					m_evTmmInitComplete.ResetEvent(); 
					if (WaitForSingleObject(m_evTmmInitComplete, 5000) == WAIT_OBJECT_0)
					{
						// Alterafile laden
						if (TransferEPLDdata(GetHomeDir()+EPLD_FILENAME))
						{
							int nDIPState = 0;
							if (DoRequestGetDIPState(nDIPState))
							{
								// DIP-State zählt von '0' nRequestedBoardID hingegen von '1' aus
								if (nDIPState+1 == nRequestedBoardID)
								{
									ML_TRACE(_T("...Found Tashaboard '%d' at DeviceInstance '%d'\n"), nRequestedBoardID, nDeviceInstance);
									bFoundBoard = TRUE;
								}
							}
							else
							{
								ML_TRACE_WARNING(_T("cannot get DIP state Tashaboard '%d'...\n"), 
									nRequestedBoardID);
							}
						}
						else
						{
							ML_TRACE_WARNING(_T("cannot transfer EPLD data Tashaboard '%d'...\n"), 
								nRequestedBoardID);
						}
					}
					else
					{
						ML_TRACE_WARNING(_T("wait time out searching Tashaboard '%d'...\n"), 
							nRequestedBoardID);
					}
				}
				else
				{
					ML_TRACE_WARNING(_T("cannot open message channel searching Tashaboard '%d'...\n"), 
						nRequestedBoardID);
				}
			}
			else
			{
				ML_TRACE_ERROR(_T("Failed to open %s for download (Errorcode=%d).\n"), sImagePath, Status);
			}

			Close();
		}
		else
		{
			ML_TRACE(_T("mdsBoardOpen failed %d. searching Tashaboard '%d' at DeviceInstance=%d...\n"), 
				(int)Status,nRequestedBoardID, nDeviceInstance);
		}
	}

	Close();
	
	Sleep(1000);

	if (bFoundBoard)
		return nDeviceInstance;
	else
		return -1;
}

//////////////////////////////////////////////////////////////////////
void CCodec::Close()
{
	if (m_hDSP)
	{
		if (m_bEstablishMessageChannel)
		{
			// Termination DSP-Programm and wait until it's terminate 
			if (DoRequestTermination())
				ML_TRACE(_T("HOST: Target terminated\n"));
	
			CloseMessageChannel();
		}
		mdsBoardClose(m_hDSP);
		m_hDSP = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OpenMessageChannel()
{
	BOOL bResult = FALSE;
	if (m_hDSP)
	{
		enum mdsErrorCode Status;

		// install callback function
		Status = mdsBoardSetInterruptCallback(m_hDSP, MDS_DSP_INDEX_A, MessageReceiveCallback, this, 0,0);

		// Check return value
		if (Status == MDS_ERROR_NO_ERROR)
		{
			ML_TRACE(_T("Open MessageChannel success\n"));
			bResult = TRUE;
			m_bEstablishMessageChannel = TRUE;
		}
		else
		{
			OnIndicateError(TASHA_ERROR_SETUP_CALLBACK_FAILED);
			ML_TRACE_ERROR(_T("Unable to set interrupt callback for Tasha.\n"));
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::CloseMessageChannel()
{
	BOOL bResult = FALSE;
	if (m_hDSP && m_bEstablishMessageChannel)
	{
		enum mdsErrorCode Status;

		// Callbackfunktion ausklinken
		Status = mdsBoardSetInterruptCallback(m_hDSP, MDS_DSP_INDEX_A, NULL, NULL, 0, 0);

		// Check return value
		if (Status == MDS_ERROR_NO_ERROR)
		{
			bResult = TRUE;
			m_bEstablishMessageChannel = FALSE;
		}
		else
		{
			OnIndicateError(TASHA_ERROR_CLEAR_CALLBACK_FAILED);
			ML_TRACE_ERROR(_T("Unable to clear interrupt callback for Tasha.\n"));
		}
	}
	
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestBF535Ping()
{
	BOOL bResult = FALSE;

	// Wenn der BF535 länger als X Sekunden nicht reagiert, wird dies der
	// TashaUnit mitgeteilt, so daß diese Maßnahmen ergreifen kann.
	// (DoRequestBF535Ping wird im Sekundentakt aufgerufen.)
	if (m_nPendingBF535Pings++ > m_dwBF535RecoveryTimeout/1000)
	{
		m_nPendingBF535Pings = 0;
		bResult = OnIndicationNoBF535Response();
	}
	else
		bResult = SendMessage(TMM_REQUEST_BF535_PING);

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmBF535Ping()
{
	m_nPendingBF535Pings = 0;
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetAnalogOut(DWORD dwCrosspointMask)
{
	if (m_bTraceSetAnalogOut)
		ML_TRACE(_T("HOST: DoRequestSetAnalogOut (dwCrosspointMask=0x%x)\n"), dwCrosspointMask);

	return SendMessage(TMM_REQUEST_SET_ANALOG_OUT, dwCrosspointMask);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetAnalogOut(DWORD dwCrosspointMask)
{
	if (m_bTraceSetAnalogOut)
		ML_TRACE(_T("HOST: OnConfirmSetAnalogOut (dwCrosspointMask=0x%x)\n"), dwCrosspointMask);

	WriteConfigurationInt(_T("Crosspoint"), _T("Matrix"), dwCrosspointMask, m_sAppIniName);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetAnalogOut()
{
	if (m_bTraceGetAnalogOut)
		ML_TRACE(_T("HOST: DoRequestGetAnalogOut\n"));

	return SendMessage(TMM_REQUEST_GET_ANALOG_OUT);
}	

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetAnalogOut(DWORD dwCrosspointMask)
{
	if (m_bTraceGetAnalogOut)
		ML_TRACE(_T("HOST: OnConfirmGetAnalogOut (dwCrosspointMask=0x%x)\n"), dwCrosspointMask);
	
	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetTerminationState(BYTE byTermMask)
{
	if (m_bTraceSetTerminationState)
		ML_TRACE(_T("HOST: DoRequestSetTerminationState (byTermMask=0x%x)\n"), (WORD)byTermMask);

	return SendMessage(TMM_REQUEST_SET_TERMINATION_STATE, byTermMask);
}	

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetTerminationState(BYTE byTermMask)
{
	if (m_bTraceSetTerminationState)
		ML_TRACE(_T("HOST: OnConfirmSetTerminationState (byTermMask=0x%x)\n"), (WORD)byTermMask);

	WriteConfigurationInt(_T("Slaves"), _T("Termination"), byTermMask, m_sAppIniName);

	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetTerminationState()
{
	if (m_bTraceGetTerminationState)
		ML_TRACE(_T("HOST: DoRequestGetTerminationState\n"));

	return SendMessage(TMM_REQUEST_GET_TERMINATION_STATE);
}	

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetTerminationState(BYTE byTermMask)
{
	if (m_bTraceGetTerminationState)
		ML_TRACE(_T("HOST: OnConfirmGetTerminationState (byTermMask=0x%x)\n"), (WORD)byTermMask);

	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetVideoEnableState(BOOL bState)
{
	if (m_bTraceSetVideoEnableState)
		ML_TRACE(_T("HOST: DoRequestSetVideoEnableState\n"));

	return SendMessage(TMM_REQUEST_SET_VIDEO_ENABLE_STATE, bState);
}	

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetVideoEnableState(BOOL bState)
{
	if (m_bTraceSetVideoEnableState)
		ML_TRACE(_T("HOST: OnConfirmSetVideoEnableState (bState=%d)\n"), bState);

	WriteConfigurationInt(_T("Slaves"), _T("VideoEnable"), bState, m_sAppIniName);

	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetVideoEnableState()
{
	if (m_bTraceGetVideoEnableState)
		ML_TRACE(_T("HOST: DoRequestGetVideoEnableState\n"));

	return SendMessage(TMM_REQUEST_GET_VIDEO_ENABLE_STATE);
}	

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetVideoEnableState(BOOL bState)
{
	if (m_bTraceGetVideoEnableState)
		ML_TRACE(_T("HOST: OnConfirmGetVideoEnableState (bState=%d)\n"), bState);

	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetCrosspointEnableState(BOOL bState)
{
	if (m_bTraceSetCrosspointEnableState)
		ML_TRACE(_T("HOST: DoRequestSetCrosspointEnableState\n"));

	return SendMessage(TMM_REQUEST_SET_CROSSPOINT_ENABLE_STATE, bState);
}	

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetCrosspointEnableState(BOOL bState)
{
	if (m_bTraceSetCrosspointEnableState)
		ML_TRACE(_T("HOST: OnConfirmSetCrosspointEnableState (bState=%d)\n"), bState);

	WriteConfigurationInt(_T("Crosspoint"), _T("Enable"), bState, m_sAppIniName);

	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetCrosspointEnableState()
{
	if (m_bTraceGetCrosspointEnableState)
		ML_TRACE(_T("HOST: DoRequestGetCrosspointEnableState\n"));

	return SendMessage(TMM_REQUEST_GET_CROSSPOINT_ENABLE_STATE);
}	

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetCrosspointEnableState(BOOL bState)
{
	if (m_bTraceGetCrosspointEnableState)
		ML_TRACE(_T("HOST: OnConfirmGetCrosspointEnableState (bState=%d)\n"), bState);

	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetChannel(WORD wSource, WORD wNewSource)
{
	ML_TRACE(_T("HOST: DoRequestSetChannel (Source=%d, NewChannel=%d)\n"), wSource, wNewSource);
	return SendMessage(TMM_REQUEST_SET_CHANNEL, wSource, wNewSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetChannel(WORD wSource, WORD wNewSource)
{
	ML_TRACE(_T("HOST: OnConfirmSetChannel (Source=%d, NewChannel=%d)\n"), wSource, wNewSource);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestNewSingleFrame(DWORD dwUserData, WORD wSource)
{
	BOOL bResult = FALSE;

	if (m_bTraceRequestSingleFrame)
		ML_TRACE(_T("HOST: DoRequestSingleFrame (UserData=0x%x, Source=%d)\n"), dwUserData, wSource);

	// Wenn kein Videosignal vorhanden ist, muß ein Testbild auf dem 2. Stream
	// geschickt werden. Die Tashafirmware macht dieses nämlich nicht.
	if (TSTBIT(m_dwCamMask, wSource))
		bResult = SendMessage(TMM_REQUEST_NEW_FRAME, dwUserData, wSource);
	else
		bResult = OnSendTestImage(dwUserData, wSource, 1);

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmNewSingleFrame(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceRequestSingleFrame)
		ML_TRACE(_T("HOST: OnConfirmSingleFrame (UserData=0x%x, Source=%d)\n"), dwUserData, wSource);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Baut ein DataPacket mit einem Testbild zusammen und verschickt es
// mit Hilfe von 'OnIndicationNewCodecData'
BOOL CCodec::OnSendTestImage(DWORD dwUserData, WORD wSource, WORD wStreamID)
{
	CSystemTime	TimeStamp;
	TimeStamp.GetLocalTime();
	DATA_PACKET* pDataPacket = (DATA_PACKET*)new BYTE[sizeof(DATA_PACKET)+ m_dwTestImageLen + 1024];

	pDataPacket->dwStartMarker			= MAGIC_MARKER_START;
	pDataPacket->wSource				= wSource;
	pDataPacket->dwStreamID				= 1;
	pDataPacket->TimeStamp.wYear		= TimeStamp.wYear;
	pDataPacket->TimeStamp.wMonth		= TimeStamp.wMonth;
	pDataPacket->TimeStamp.wDay			= TimeStamp.wDay;
	pDataPacket->TimeStamp.wHour		= TimeStamp.wHour;
	pDataPacket->TimeStamp.wMinute		= TimeStamp.wMinute;
	pDataPacket->TimeStamp.wSecond		= TimeStamp.wSecond;
	pDataPacket->TimeStamp.wMSecond		= TimeStamp.wMilliseconds;
	pDataPacket->TimeStamp.bValid		= TRUE;

	pDataPacket->bValid					= TRUE;
	pDataPacket->wSizeH					= 352;
	pDataPacket->wSizeV					= 288;
	pDataPacket->wBytesPerPixel			= 2;
	pDataPacket->wSize					= sizeof(DATA_PACKET);
	pDataPacket->eType					= eTypeMpeg4;
	pDataPacket->eSubType				= eSubIFrame;
	pDataPacket->bVidPresent			= TRUE;
	pDataPacket->Diff.bValid			= FALSE;
	pDataPacket->PermanentMask.bValid	= FALSE;
	pDataPacket->AdaptiveMask.bValid	= FALSE;
	pDataPacket->Debug.bValid			= FALSE;
	pDataPacket->dwEndMarker			= MAGIC_MARKER_END;
	pDataPacket->bVidPresent			= FALSE;
	pDataPacket->dwFieldID				= 0;
	pDataPacket->dwProzessID			= dwUserData;

	pDataPacket->dwImageDataLen			= m_dwTestImageLen;

	CopyMemory(&pDataPacket->pImageData, m_lpTestImage, m_dwTestImageLen);

	// Testbild verschicken.
	OnIndicationNewCodecData(pDataPacket);

	WK_DELETE(pDataPacket);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam)
{
	DWORD dwVal = 0;

	if (m_bTraceSetEncoderParam)
	{
		ML_TRACE(_T("HOST: DoRequestSetEncoderParam (UserData=0x%x, Source=%d, StreamID=%d, Res.=%d, Bitrate=%d, Fps=%d, GOPInterv.=%d, CompType=%d)\n"),
														dwUserData, wSource, EncParam.nStreamID,
														EncParam.eRes, EncParam.nBitrate,
														EncParam.nFps, EncParam.nIFrameIntervall, EncParam.eType);
	}
	//									20	  18				12          8  			   3	    0
	// F  F  F  F  F  F  F  F  F  F  F  F  E  E  D  D  D  D  D  D  C  C  C  C  B  B  B  B  B  A  A  A
	// A = Resolution (eImageResHigh, eImageResMid, eImageResLow)
	// B = Frames per second (1...25)
	// C = Compressiontype (eTypeMpeg4, eTypeYUV422...)
	// D = I-Frame Intervall (1...100)
	// E = StreamID (0,1)
	// F = Bitrate (64...4000KBits)
	dwVal = (EncParam.eRes  & 0x07) << 0 | 
			(EncParam.nFps  & 0x1f) << 3 |
			(EncParam.eType & 0x0f) << 8 |
			(EncParam.nIFrameIntervall & 0x3F) << 12 |
			(EncParam.nStreamID & 0x03) << 18 |
			(EncParam.nBitrate & 0xfff)	<< 20;

	return SendMessage(TMM_REQUEST_SET_ENCODER_PARAMETER, dwUserData, wSource, dwVal);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam)
{
	if (m_bTraceSetEncoderParam)
	{
		ML_TRACE(_T("HOST: OnConfirmSetEncoderParam (UserData=0x%x, Source=%d, StreamID=%d, Res.=%d, Bitrate=%d, Fps=%d, GOPInterv.=%d, CompType=%d)\n"),
													dwUserData, wSource, EncParam.nStreamID,
													EncParam.eRes, EncParam.nBitrate,
													EncParam.nFps, EncParam.nIFrameIntervall, EncParam.eType);
	}

	CString sSection;

	// Alle (gültigen) Encoder Parameter in der Registry sichern.
	sSection.Format(_T("Slaves\\Source%02u\\EncoderParams\\StreamID%d"), wSource, EncParam.nStreamID);
	if (EncParam.eRes != eImageResUnvalid)
		WriteConfigurationInt(sSection, _T("Resolution"), EncParam.eRes, m_sAppIniName);
	if ((EncParam.nBitrate > 0) && (EncParam.nBitrate <= 4000))
			WriteConfigurationInt(sSection, _T("Bitrate"), EncParam.nBitrate, m_sAppIniName);
	if ((EncParam.nFps > 0) && (EncParam.nFps <= 25))
		WriteConfigurationInt(sSection, _T("Framerate"), EncParam.nFps, m_sAppIniName);
	if ((EncParam.nIFrameIntervall > 0) && (EncParam.nIFrameIntervall <= 64))
		WriteConfigurationInt(sSection, _T("I-FrameIntervall"), EncParam.nIFrameIntervall, m_sAppIniName);
	if (EncParam.eType != eTypeUnvalid)
		WriteConfigurationInt(sSection, _T("ImageType"), EncParam.eType, m_sAppIniName);
	WriteConfigurationInt(sSection, _T("UserData"), dwUserData, m_sAppIniName);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetBrightness)
		ML_TRACE(_T("HOST: DoRequestSetBrightness (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);
	
	return SendMessage(TMM_REQUEST_SET_BRIGHTNESS, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetBrightness)
		ML_TRACE(_T("HOST: OnConfirmSetBrightness (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);
	
	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u"),wSource);
	WriteConfigurationInt(sBuffer, _T("Brightness"), nValue, m_sAppIniName);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetContrast)
		ML_TRACE(_T("HOST: DoRequestSetContrast (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);
	return SendMessage(TMM_REQUEST_SET_CONTRAST, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetContrast)
		ML_TRACE(_T("HOST: OnConfirmSetContrast (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u"),wSource);
	WriteConfigurationInt(sBuffer, _T("Contrast"), nValue, m_sAppIniName);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetSaturation)
		ML_TRACE(_T("HOST: DoRequestSetSaturation (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	return SendMessage(TMM_REQUEST_SET_SATURATION, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetSaturation)
		ML_TRACE(_T("HOST: OnConfirmSetSaturation (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u"),wSource);
	WriteConfigurationInt(sBuffer, _T("Saturation"), nValue, m_sAppIniName);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetEncoderParam(DWORD dwUserData, WORD wSource, WORD wStreamID)
{
	if (m_bTraceGetEncoderParam)
		ML_TRACE(_T("HOST: DoRequestGetEncoderParam (UserData=0x%x Source=%d StreamID=%d)\n"), dwUserData, wSource, wStreamID);
	
	return SendMessage(TMM_REQUEST_GET_ENCODER_PARAMETER, dwUserData, wSource, wStreamID);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam)
{
	if (m_bTraceGetEncoderParam)
	{
		ML_TRACE(_T("HOST: OnConfirmGetEncoderParam (UserData=0x%x, Source=%d, StreamID=%d, Res.=%d, Bitrate=%d, Fps=%d, GOPInterv.=%d, CompType=%d)\n"),
										   dwUserData, wSource, EncParam.nStreamID, EncParam.eRes, EncParam.nBitrate, EncParam.nFps, EncParam.nIFrameIntervall, EncParam.eType);
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetBrightness(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetBrightness)
		ML_TRACE(_T("HOST: DoRequestGetBrightness (UserData=0x%x Source=%d)\n"), dwUserData, wSource);
	return SendMessage(TMM_REQUEST_GET_BRIGHTNESS, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceGetBrightness)
		ML_TRACE(_T("HOST: OnConfirmGetBrightness (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetContrast(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetContrast)
		ML_TRACE(_T("HOST: DoRequestGetContrast (UserData=0x%x Source=%d)\n"), dwUserData, wSource);

	return SendMessage(TMM_REQUEST_GET_CONTRAST, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceGetContrast)
		ML_TRACE(_T("HOST: OnConfirmGetContrast (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetSaturation(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetSaturation)
		ML_TRACE(_T("HOST: DoRequestGetSaturation (UserData=0x%x Source=%d)\n"), dwUserData, wSource);
	return SendMessage(TMM_REQUEST_GET_SATURATION, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceGetSaturation)
		ML_TRACE(_T("HOST: OnConfirmGetSaturation (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetMask(WORD wSource, const CIPCActivityMask& mask)
{
	BOOL bResult = FALSE;

	MASK_STRUCT PermMask;
	PermMask.nDimH = MD_RESOLUTION_H/8;
	PermMask.nDimV = MD_RESOLUTION_V/8;
	PermMask.bValid= TRUE;
	PermMask.bHasChanged = TRUE;

	// CIPCActivityMask in die interne MASK_STRUCT überführen...
	if (mask.GetType() == AM_PERMANENT)
	{
		// Bringt die makenwerte von 0....255 auf einen Bereich von 0....1000;
		double fScale = (double)AM_UPPER_BOUNDARY/255.0;

		if ((mask.GetSize().cx != MD_RESOLUTION_H/8) ||	(mask.GetSize().cy != MD_RESOLUTION_V/8))
		{
			ML_TRACE(_T("SetMask: Size of the Masks are different...scaling permanent and adaptive mask\n"));

			float fX = 0.0;
			float fY = 0.0;
			float fdX = (float)mask.GetSize().cx / (float)(MD_RESOLUTION_H/8);
			float fdY = (float)mask.GetSize().cy / (float)(MD_RESOLUTION_V/8);
 
			for (int nY = 0; nY < MD_RESOLUTION_V/8; nY++, fX = 0.0)
			{
				for (int nX = 0; nX < MD_RESOLUTION_H/8; nX++)
				{
					// TODO: Die Maske wird später noch im BF533 invertiert. So daß
					// 0 transparent und 255 deckend ist
					PermMask.byMask[nY][nX] = min(max(255-mask.GetAt(nX, nY)/fScale, 0), 255);
					fX += fdX;
				}  
				fY += fdY;
			}
		}
		else
		{
			for (int nY = 0; nY < MD_RESOLUTION_V/8; nY++)
			{
				for (int nX = 0; nX < MD_RESOLUTION_H/8; nX++)
				{
					// TODO: Die Maske wird später noch im BF533 invertiert. So daß
					// 0 transparent und 255 deckend ist
					PermMask.byMask[nY][nX] = min(max(255-mask.GetAt(nX, nY)/fScale, 0), 255);
				}
			}
		}
		// Und gleich sichern
		if (SavePermanentMaskToRegistry(&PermMask, wSource))
			bResult = DoRequestSetPermanentMask(0, wSource, PermMask, FALSE);
	}
	else
	{
		OnIndicateWarning(TASHA_WARNING_ONLY_PERMANENT_MASK_CAN_BE_SET);
		ML_TRACE_WARNING(_T("CCodec::SetMask Only permanent mask can be set\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::GetMask(WORD wSource, CIPCActivityMask& mask)
{
	BOOL bResult = FALSE;

	if (mask.GetType() == AM_PERMANENT)
	{
		MASK_STRUCT PermMask;
		if (ReadPermanentMaskFromRegistry(&PermMask, wSource))
		{
			// Convert from 'MASK_STRUCT' to 'CIPCActivityMask'
			mask.Create(AM_PERMANENT, MD_RESOLUTION_H/8, MD_RESOLUTION_V/8);

			// Bringt die makenwerte von 0....1000 auf einen Bereich von 0....255;
			double fScale = (double)AM_UPPER_BOUNDARY/255.0;

			for (int nY = 0; nY < MD_RESOLUTION_V/8; nY++)
			{
				for (int nX = 0; nX < MD_RESOLUTION_H/8; nX++)
				{
					// TODO: Die Maske wird später noch im BF533 invertiert. So daß
					// 0 transparent und 255 deckend ist
					mask.SetAt(nX, nY, min(max(1000-(PermMask.byMask[nY][nX]*fScale), 0), 1000));
				}
			}
			bResult = TRUE;
		}
	}
	else if (mask.GetType() == AM_ADAPTIVE)
	{
		MASK_STRUCT AdaptiveMask;

		if (GetCurrentAdaptiveMask(&AdaptiveMask, wSource))
		{
			// Convert from 'MASK_STRUCT' to 'CIPCActivityMask'
			mask.Create(AM_ADAPTIVE, MD_RESOLUTION_H/8, MD_RESOLUTION_V/8);

			// Bringt die makenwerte von 0....1000 auf einen Bereich von 0....255;
			double fScale = (double)AM_UPPER_BOUNDARY/255.0;

			for (int nY = 0; nY < MD_RESOLUTION_V/8; nY++)
			{
				for (int nX = 0; nX < MD_RESOLUTION_H/8; nX++)
				{
					// TODO: Die Maske wird später noch im BF533 invertiert. So daß
					// 0 transparent und 255 deckend ist
					mask.SetAt(nX, nY, min(max(1000-(AdaptiveMask.byMask[nY][nX]*fScale), 0), 1000));
				}
			}
			bResult = TRUE;
		}
	}
	else
	{
		OnIndicateError(TASHA_ERROR_UNKNOWN_MASK_TYPE);
		ML_TRACE_ERROR(_T("CCodec::GetMask Unknown Mask type\n"));
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::SaveCurrentPermanentMask(MASK_STRUCT* pPermanentMask, WORD wSource)
{
	BOOL bResult = FALSE;
	if (wSource < MAX_CAMERAS)
	{
		// Maske lokal und in der Registry speichern
		memcpy(&m_PermanentMask[wSource], pPermanentMask, sizeof(MASK_STRUCT));
		SavePermanentMaskToRegistry(pPermanentMask, wSource);
		bResult = TRUE;
	}
	
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::SaveCurrentAdaptiveMask(MASK_STRUCT* pAdaptiveMask, WORD wSource)
{
	BOOL bResult = FALSE;
	if (wSource < MAX_CAMERAS)
	{
		memcpy(&m_AdaptiveMask[wSource], pAdaptiveMask, sizeof(MASK_STRUCT));
		bResult = TRUE;
	}
	
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::GetCurrentPermanentMask(MASK_STRUCT* pPermanentMask, WORD wSource)
{
	BOOL bResult = FALSE;
	if (wSource < MAX_CAMERAS)
	{
		memcpy(pPermanentMask, &m_PermanentMask[wSource], sizeof(MASK_STRUCT));
		bResult = TRUE;
	}
	
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::GetCurrentAdaptiveMask(MASK_STRUCT* pAdaptiveMask, WORD wSource)
{
	BOOL bResult = FALSE;
	if (wSource < MAX_CAMERAS)
	{
		memcpy(pAdaptiveMask, &m_AdaptiveMask[wSource], sizeof(MASK_STRUCT));
		bResult = TRUE;
	}
	
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::SavePermanentMaskToRegistry(MASK_STRUCT* pPermanentMask, WORD wSource)
{
	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u\\MD"),wSource);

	return WriteConfigurationBinary(sBuffer, _T("PermanentMask"), (BYTE*)pPermanentMask, sizeof(MASK_STRUCT), m_sAppIniName);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::ReadPermanentMaskFromRegistry(MASK_STRUCT* pPermanentMask, WORD wSource)
{
	BOOL bResult = FALSE;

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u\\MD"),wSource);
	MASK_STRUCT* pPerMask	 = NULL;
	UINT		 nLen		 = 0;
	
	if (ReadConfigurationBinary(sBuffer, _T("PermanentMask"), (BYTE**)&pPerMask, &nLen, m_sAppIniName))
	{
		memcpy(pPermanentMask, pPerMask, nLen);
		WK_DELETE(pPerMask);
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetMDMaskSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel)
{
	BOOL bResult = FALSE;
	if (wSource >= MAX_CAMERAS)
	{
		OnIndicateError(TASHA_ERROR_UNVALID_PARAMETER);
		ML_TRACE_ERROR(_T("CCodec::SetMDMaskSensitivity\tWrong Camera (%d)\n"), wSource+1);
		return FALSE;
	}

	// Einstellung sofort in der Registry sichern
	_TCHAR szBuffer[255];
	wsprintf(szBuffer, _T("Slaves\\Source%02u\\MD"), wSource);
	WriteConfigurationString(szBuffer, _T("MaskSensitivity"), sLevel, m_sAppIniName); 

	// TODO: Die Level müssen noch entsprechend der Empfindlichkeit eingestellt werden
	if (sLevel == CSD_HIGH)
	{
		DoRequestSetMaskTreshold(0, wSource, 2);
		DoRequestSetMaskIncrement(0, wSource, 20);
		DoRequestSetMaskDelay(0, wSource, 0);
		bResult = TRUE;
	}
	else if (sLevel == CSD_NORMAL)
	{
		DoRequestSetMaskTreshold(0, wSource, 6);
		DoRequestSetMaskIncrement(0, wSource, 20);
		DoRequestSetMaskDelay(0, wSource, 0);
		bResult = TRUE;
	}
	else if (sLevel == CSD_LOW)
	{
		DoRequestSetMaskTreshold(0, wSource, 10);
		DoRequestSetMaskIncrement(0, wSource, 20);
		DoRequestSetMaskDelay(0, wSource, 0);
		bResult = TRUE;
	}
	else if (sLevel == CSD_OFF)
	{
		DoRequestSetMaskDelay(0, wSource, 0);
		DoRequestSetMaskIncrement(0, wSource, 0);
		bResult = TRUE;
	}
	else
	{
		OnIndicateWarning(TASHA_WARNING_UNKNOWN_MASK_SENSITIVITY);
		ML_TRACE_WARNING(_T("Unknown MDMaskSensitivity\n"));
	}

	if (bResult)
		OnConfirmSetMDMaskSensitivity(dwUserData, wSource, sLevel);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetMDMaskSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel)
{
	// Dummy funktion, should be overridden

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel)
{
	BOOL bResult = FALSE;

	if (wSource >= MAX_CAMERAS)
	{
		OnIndicateError(TASHA_ERROR_UNVALID_PARAMETER);
		ML_TRACE_ERROR(_T("CCodec::SetMDAlarmSensitivity\tWrong Camera (%d)\n"), wSource+1);
		return FALSE;
	}

	// Einstellung sofort in der Registry sichern
	_TCHAR szBuffer[255];
	wsprintf(szBuffer, _T("Slaves\\Source%02u\\MD"), wSource);
	WriteConfigurationString(szBuffer, _T("AlarmSensitivity"), sLevel, m_sAppIniName); 

	if (sLevel == CSD_HIGH)
		bResult = DoRequestSetMDTreshold(0, wSource, 5);
	else if (sLevel == CSD_NORMAL)
		bResult = DoRequestSetMDTreshold(0, wSource, 10);
	else if (sLevel == CSD_LOW)
		bResult = DoRequestSetMDTreshold(0, wSource, 20);
	else if (sLevel == CSD_OFF)
		bResult = DoRequestSetMDTreshold(0, wSource, 100);
	else
	{
		OnIndicateWarning(TASHA_WARNING_UNKNOWN_ALARM_SENSITIVITY);
		ML_TRACE_WARNING(_T("Unknown MDAlarmSensitivity\n"));
	}

	if (bResult)
		OnConfirmSetMDAlarmSensitivity(dwUserData, wSource, sLevel);
		
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel)
{
	// Dummy funktion, should be overridden

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetMDMaskSensitivity(DWORD dwUserData, WORD wSource)
{
	BOOL bResult = FALSE;

	if (wSource >= MAX_CAMERAS)
	{
		OnIndicateError(TASHA_ERROR_UNVALID_PARAMETER);
		ML_TRACE_ERROR(_T("CCodec::GetMDMaskSensitivity\tWrong Camera (%d)\n"), wSource+1);
	}
	else
	{
		_TCHAR szSection[255] = {0};
		_TCHAR szBuffer[255]  = {0};
		wsprintf(szSection, _T("Slaves\\Source%02u\\MD"), wSource);

		ReadConfigurationString(szSection, _T("MaskSensitivity"), CSD_OFF, szBuffer, 16, m_sAppIniName);
		CString sLevel(szBuffer);

		OnConfirmGetMDMaskSensitivity(dwUserData, wSource, sLevel);
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetMDMaskSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel)
{
	// Dummy funktion, should be overridden

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetMDAlarmSensitivity(DWORD dwUserData, WORD wSource)
{
	BOOL bResult = FALSE;

	if (wSource >= MAX_CAMERAS)
	{
		OnIndicateError(TASHA_ERROR_UNVALID_PARAMETER);
		ML_TRACE_ERROR(_T("CCodec::GetMDAlarmSensitivity\tWrong Camera (%d)\n"), wSource+1);
	}
	else
	{
		_TCHAR szSection[255] = {0};
		_TCHAR szBuffer[255]	= {0};
		wsprintf(szSection, _T("Slaves\\Source%02u\\MD"), wSource);

		ReadConfigurationString(szSection, _T("AlarmSensitivity"), CSD_NORMAL, szBuffer, 16, m_sAppIniName);
		
		CString sLevel(szBuffer);
		OnConfirmGetMDAlarmSensitivity(dwUserData, wSource, sLevel);
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel)
{
	// Dummy funktion, should be overridden

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMDTreshold)
		ML_TRACE(_T("HOST: DoRequestSetMDTreshold (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	return SendMessage(TMM_REQUEST_SET_THRESHOLD_MD, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMDTreshold)
		ML_TRACE(_T("HOST: OnConfirmSetMDTreshold (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u\\MD"),wSource);
	WriteConfigurationInt(sBuffer, _T("MDTreshold"), nValue, m_sAppIniName);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMaskTreshold)
		ML_TRACE(_T("HOST: DoRequestSetMaskTreshold (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);
	
	return SendMessage(TMM_REQUEST_SET_THRESHOLD_MASK, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMaskTreshold)
		ML_TRACE(_T("HOST: OnConfirmSetMaskTreshold (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u\\MD"),wSource);
	WriteConfigurationInt(sBuffer, _T("MaskTreshold"), nValue, m_sAppIniName);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMaskIncrement)
		ML_TRACE(_T("HOST: DoRequestSetMaskIncrement (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	return SendMessage(TMM_REQUEST_SET_INCREMENT_MASK, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMaskIncrement)
		ML_TRACE(_T("HOST: OnConfirmSetMaskIncrement (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u\\MD"),wSource);
	WriteConfigurationInt(sBuffer, _T("MaskIncrement"), nValue, m_sAppIniName);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMaskDelay)
		ML_TRACE(_T("HOST: DoRequestSetMaskDelay (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	return SendMessage(TMM_REQUEST_SET_DELAY_MASK, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMaskDelay)
		ML_TRACE(_T("HOST: OnConfirmSetMaskDelay (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u\\MD"),wSource);
	WriteConfigurationInt(sBuffer, _T("MaskDelay"), nValue, m_sAppIniName);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestChangePermanentMask(DWORD dwUserData, WORD wSource, int nX, int nY, int nValue)
{
	if (m_bTraceChangePermanentMask)
		ML_TRACE(_T("HOST: DoRequestChangePermanentMask (UserData=0x%x Source=%d nX=%d, nY=%d Value=%d)\n"), dwUserData, wSource, nX, nY, nValue);

	return SendMessage(TMM_REQUEST_CHANGE_PERMANENT_MASK, dwUserData, wSource, MAKELONG(nY, nX), nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmChangePermanentMask(DWORD dwUserData, WORD wSource, int nX, int nY, int nValue)
{
	BOOL bResult = FALSE;

	if (m_bTraceChangePermanentMask)
		ML_TRACE(_T("HOST: OnConfirmChangePermanentMask (UserData=0x%x Source=%d nX=%d, nY=%d Value=%d)\n"), dwUserData, wSource, nX, nY, nValue);

	MASK_STRUCT* pPermanentMask	 = new MASK_STRUCT;

	if (ReadPermanentMaskFromRegistry(pPermanentMask, wSource))
	{
		pPermanentMask->byMask[nY][nX] = nValue;
		bResult = SavePermanentMaskToRegistry(pPermanentMask, wSource);
	}
	WK_DELETE(pPermanentMask);
								    
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetMDTreshold(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetMDTreshold)
		ML_TRACE(_T("HOST: DoRequestGetMDTreshold (UserData=0x%x Source=%d)\n"), dwUserData, wSource);
	
	return SendMessage(TMM_REQUEST_GET_THRESHOLD_MD, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetMDTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceGetMDTreshold)
		ML_TRACE(_T("HOST: OnConfirmGetMDTreshold (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetMaskTreshold(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetMaskTreshold)
		ML_TRACE(_T("HOST: DoRequestGetMaskTreshold (UserData=0x%x Source=%d)\n"), dwUserData, wSource);
	
	return SendMessage(TMM_REQUEST_GET_THRESHOLD_MASK, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceGetMaskTreshold)
		ML_TRACE(_T("HOST: OnConfirmGetMaskTreshold (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetMaskIncrement(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetMaskIncrement)
		ML_TRACE(_T("HOST: DoRequestGetMaskIncrement (UserData=0x%x Source=%d)\n"), dwUserData, wSource);
	return SendMessage(TMM_REQUEST_GET_INCREMENT_MASK, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceGetMaskIncrement)
		ML_TRACE(_T("HOST: OnConfirmGetMaskIncrement (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetMaskDelay(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetMaskDelay)
		ML_TRACE(_T("HOST: DoRequestGetMaskDelay (UserData=0x%x Source=%d)\n"), dwUserData, wSource);
	
	return SendMessage(TMM_REQUEST_GET_DELAY_MASK, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetMaskDelay(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceGetMaskDelay)
		ML_TRACE(_T("HOST: OnConfirmGetMaskDelay (UserData=0x%x Source=%d Value=%d)\n"), dwUserData, wSource, nValue);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestStartCapture() 
{
	// Den Zeitpunkt der letzten gültigen Videodaten auf jetzt stellen.
	for (int nChannel = 0; nChannel < CHANNEL_COUNT; nChannel++)
		m_dwLastValidDataPacketTC[nChannel] = WK_GetTickCount();

	if (m_bTraceStartEncoder)
		ML_TRACE(_T("HOST: DoRequestStartCapture\n"));
	
	return SendMessage(TMM_REQUEST_START_CAPTURE, 0, 0);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmStartCapture() 
{
	if (m_bTraceStartEncoder)
		ML_TRACE(_T("HOST: OnConfirmStartCapture\n"));

	m_eEncoderState = eEncoderStateOn;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestStopCapture() 
{
	if (m_bTraceStopEncoder)
		ML_TRACE(_T("HOST: DoRequestStopCapture\n"));
	
	return SendMessage(TMM_REQUEST_STOP_CAPTURE, 0, 0);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmStopCapture() 
{
	if (m_bTraceStopEncoder)
		ML_TRACE(_T("HOST: OnConfirmStopCapture\n"));

	m_eEncoderState = eEncoderStateOff;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestPauseCapture() 
{
	if (m_bTracePauseEncoder)
		ML_TRACE(_T("HOST: DoRequestPauseCapture\n"));

	return SendMessage(TMM_REQUEST_PAUSE_CAPTURE, 0, 0);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmPauseCapture() 
{
	if (m_bTracePauseEncoder)
		ML_TRACE(_T("HOST: OnConfirmPauseCapture\n"));
	
	m_eEncoderState = eEncoderStatePause;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestResumeCapture() 
{
	if (m_bTraceResumeEncoder)
		ML_TRACE(_T("HOST: DoRequestResumeCapture\n"));
	return SendMessage(TMM_REQUEST_RESUME_CAPTURE, 0, 0);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmResumeCapture()
{
	if (m_bTraceResumeEncoder)
		ML_TRACE(_T("HOST: OnConfirmResumeCapture\n"));
	
	m_eEncoderState = eEncoderStateOn;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetNR(DWORD dwUserData, WORD wSource, BOOL bEnable)
{
	if (m_bTraceSetNR)
		ML_TRACE(_T("HOST: DoRequestSetNR (UserData=0x%x Source=%d bEnable=%d)\n"), dwUserData, wSource, bEnable);
	return SendMessage(TMM_REQUEST_SET_NR, dwUserData, wSource, bEnable);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetNR(DWORD dwUserData, WORD wSource, BOOL bEnable)
{
	if (m_bTraceSetNR)
		ML_TRACE(_T("HOST: OnConfirmSetNR (UserData=0x%x Source=%d bEnable=%d)\n"), dwUserData, wSource, bEnable);

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u"),wSource);
	WriteConfigurationInt(sBuffer, _T("NoiseReduction"), bEnable, m_sAppIniName);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetNR(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetNR)
		ML_TRACE(_T("HOST: DoRequestGetNR (UserData=0x%x Source=%d)\n"), dwUserData, wSource);
	return SendMessage(TMM_REQUEST_GET_NR, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetNR(DWORD dwUserData, WORD wSource, BOOL bEnable)
{
	if (m_bTraceGetNR)
		ML_TRACE(_T("HOST: OnConfirmGetNR (UserData=0x%x Source=%d bEnable=%d)\n"), dwUserData, wSource, bEnable);

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u"),wSource);
	WriteConfigurationInt(sBuffer, _T("NoiseReduction"), bEnable, m_sAppIniName);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::SendMessage(int nMessageID, DWORD dwParam1/*=0*/, DWORD dwParam2/*=0*/, DWORD dwParam3/*=0*/,DWORD dwParam4/*=0*/)
{
	BOOL bResult = FALSE;

	if (m_hDSP)
	{
		m_csSendMessage.Lock();

		enum mdsErrorCode Status;

		MessageStruct Message;
		Message.nMessageID	= nMessageID;
		Message.dwParam1	= dwParam1;
		Message.dwParam2	= dwParam2;
		Message.dwParam3	= dwParam3;
		Message.dwParam4	= dwParam4;
		Message.bAck		= FALSE;

		// Message in Sharedmemory ablegen
		Status = mdsBoardWriteMemory(m_hDSP, MDS_DSP_MEM_DATA, sizeof(Message), (int)PC2DSPMESSAGEBUFFER, &Message, MDS_DSP_INDEX_A);
    
		// check return value
		if (Status == MDS_ERROR_NO_ERROR)
		{
			// send interrupt to DSP
			Status = mdsBoardInterruptDsp(m_hDSP, MDS_DSP_INDEX_A,	0, 0);	

			// Check return value
			if (Status == MDS_ERROR_NO_ERROR)
			{
				// Auf die Empfangsbestätigung warten.
				DWORD dwTC = GetTickCount();
				do
				{
			 		mdsBoardReadMemory(m_hDSP,	MDS_DSP_MEM_DATA, sizeof(Message), (int)PC2DSPMESSAGEBUFFER, &Message, MDS_DSP_INDEX_A);
				}while(!Message.bAck && (GetTickCount() - dwTC)	> 100);

				if (!Message.bAck)
				{
					OnIndicateError(TASHA_ERROR_ACKNOLEDGE_TIMEOUT);
					ML_TRACE_ERROR(_T("Wait for Acknoledge timeout (MessageID=%d)\n"), nMessageID);
				}

				bResult = Message.bAck;
			}
			else
			{
				OnIndicateError(TASHA_ERROR_FAILED_TO_INTERRUPT_DSP);
				ML_TRACE_ERROR(_T("Failed to interrupt DSP on Tasha. (MessageID=%d)\n"), nMessageID);
			}

		}
		else		 
		{
			OnIndicateError(TASHA_ERROR_WRITE_DSP_MEMORY_FAILED);
			ML_TRACE_ERROR(_T("Error writing to DSP data memory.\n"));
		}

		m_csSendMessage.Unlock();
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::ReceiveMessage(int &nMessageID, DWORD &dwParam1, DWORD &dwParam2, DWORD &dwParam3, DWORD &dwParam4)
{
	BOOL bResult = FALSE;
	if (m_hDSP)
	{
		m_csReceiveMessage.Lock();
		
		enum mdsErrorCode Status;

		MessageStruct Message;

		// Lese message
 		Status = mdsBoardReadMemory(m_hDSP,	MDS_DSP_MEM_DATA, sizeof(Message), (int)DSP2PCMESSAGEBUFFER, &Message, MDS_DSP_INDEX_A);

		// check return value
		if (Status == MDS_ERROR_NO_ERROR)
		{
			nMessageID	= Message.nMessageID;
			dwParam1	= Message.dwParam1;
			dwParam2	= Message.dwParam2;
			dwParam3	= Message.dwParam3;
			dwParam4	= Message.dwParam4;
			// Acknoledge zurück
			Message.bAck= TRUE;		
			Status = mdsBoardWriteMemory(m_hDSP, MDS_DSP_MEM_DATA, sizeof(Message), (int)DSP2PCMESSAGEBUFFER, &Message, MDS_DSP_INDEX_A);
	
			// check return value
			if (Status == MDS_ERROR_NO_ERROR)	
				bResult = TRUE;
			else
			{
				OnIndicateError(TASHA_ERROR_SEND_ACKNOLEDGE_FAILED);
				ML_TRACE_ERROR(_T("Error can't send acknoledge.\n"));
			}
		}
		else
		{
			OnIndicateError(TASHA_ERROR_READ_DSP_FAILED_FAILED);
			ML_TRACE_ERROR(_T("Error reading from DSP data memory.\n"));
		}
		m_csReceiveMessage.Unlock();
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void __stdcall CCodec::MessageReceiveCallback(LPVOID pData)
{	
	static BOOL bChangeThreadPriority = TRUE;
	CCodec*	pThis	 = (CCodec*)pData;  // Pointer auf's Codec-Objekt
	
	if (bChangeThreadPriority)
	{
		SetThreadPriority(GetCurrentThread() ,THREAD_PRIORITY_ABOVE_NORMAL);
		bChangeThreadPriority = FALSE;
	}

	if (!pThis)
	{
		ML_TRACE_ERROR(_T("HOST: MessageReceiveCallback pCodec = NULL\n"));
		return;
	}

	MESSAGE* pMsg = new MESSAGE;
	ZeroMemory(pMsg, sizeof(MESSAGE)); 
	pMsg->pCodec = pThis;
	
	if (pThis->ReceiveMessage(pMsg->nMessageID, pMsg->dwParam1, pMsg->dwParam2, pMsg->dwParam3, pMsg->dwParam4))
	{
#ifdef USE_MULTITHREADED_MESSAGE_HANDLING	// Starte neuen Tread zur weiteren Bearbeitung der Message
		if (pMsg->nMessageID !=	TMM_NEW_CODEC_DATA)
			AfxBeginThread(MessageHandleThread, (LPVOID)pMsg, THREAD_PRIORITY_NORMAL);
		else
			MessageHandleThread((LPVOID)pMsg);
#else
		MessageHandleThread((LPVOID)pMsg);
#endif
	}
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Liest die über den SPORT ankommenden Daten und sammelt diese im entsprechenden
// Circularbuffer.
// nSport = Nummer des SPORTs (0, 1)
// dwBufferOffset=Offsetadresse, ab der die Daten abgelegt wurden.
// dwBufferLen=Länge der gültigen Daten
BOOL CCodec::OnReceiveNewCodecDataEx(int nSport, DWORD dwBufferOffset, DWORD dwBufferLen)
{
	BOOL bResult = FALSE;
	BOOL bErr	 = FALSE;

	if (dwBufferLen == CHANNEL_SIZE*CHANNEL_COUNT/2)
	{	
		if (m_memFrameBuffer.pLinAddr != NULL)
		{
			DWORD dwFrameBufferAddr = (DWORD)m_memFrameBuffer.pLinAddr+dwBufferOffset;
			
			// Lese den Framebuffer des zugehörigen Channels aus
			if (DAReadMemoryX(m_pFrameBuffer, dwFrameBufferAddr, dwBufferLen))
			{
				// Die Daten von allen 8 DSPs in die jeweiliegen Circularbuffer hängen.
				for (int nI = 0; nI < CHANNEL_COUNT/2; nI++)
				{
					int nChannel = nSport*CHANNEL_COUNT/2+nI;
					if (!m_pCirBuff[nChannel]->Add(m_pFrameBuffer+nI*CHANNEL_SIZE, CHANNEL_SIZE))
						bErr = TRUE;
				}
				// Wenn beide SPORTs ihre Daten geliefert haben, dem ReadDataThread bescheid geben.
				if (nSport == 1)
					ReleaseSemaphore(m_hNewDataSemaphore, 1, NULL);
				
				bResult = TRUE;
			}
		}
	}
	else
		ML_TRACE(_T("Packet size too large (%d>%d)\n"), dwBufferLen, CHANNEL_SIZE*CHANNEL_COUNT/2);

	return bResult && !bErr;
}

/////////////////////////////////////////////////////////////////////////////
// Diese Funktion wird später durch OnReceiveNewCodecDataEx komplett ersetzt.
BOOL CCodec::OnReceiveNewCodecData(int nSport, DWORD dwBufferOffset, DWORD dwBufferLen)
{
	BOOL bResult = FALSE;
	static DWORD dwFieldID = 0;

	if (dwBufferLen == CHANNEL_SIZE*CHANNEL_COUNT/2)
	{	
		if (m_memFrameBuffer.pLinAddr != NULL)
		{
			DWORD dwFrameBufferAddr = (DWORD)m_memFrameBuffer.pLinAddr+dwBufferOffset;
			
			// Lese den Framebuffer des zugehörigen Channels aus
			if (DAReadMemoryX(m_pFrameBuffer, dwFrameBufferAddr, dwBufferLen))
			{
				DATA_PACKET* pDataPacket = NULL;

				for (int nI = 0; nI < CHANNEL_COUNT/2; nI++)
				{
					int nChannel = nSport*CHANNEL_COUNT/2+nI;
					pDataPacket = (DATA_PACKET*)(m_pFrameBuffer+nI*CHANNEL_SIZE);
					
					pDataPacket->dwStartMarker			= MAGIC_MARKER_START;
					pDataPacket->wSource				= nChannel;
					pDataPacket->dwStreamID				= 0;
					pDataPacket->bValid					= TRUE;
					pDataPacket->wSizeH					= 352;
					pDataPacket->wSizeV					= 288;
					pDataPacket->wBytesPerPixel			= 2;
					pDataPacket->wSize					= sizeof(DATA_PACKET);
					pDataPacket->eType					= eTypeYUV422;
					pDataPacket->bVidPresent			= TRUE;
					pDataPacket->Diff.bValid			= FALSE;
					pDataPacket->PermanentMask.bValid	= FALSE;
					pDataPacket->AdaptiveMask.bValid	= FALSE;
					pDataPacket->Debug.bValid			= FALSE;
					pDataPacket->dwEndMarker			= MAGIC_MARKER_END;
					pDataPacket->TimeStamp.bValid		= FALSE;
					pDataPacket->dwImageDataLen			= pDataPacket->wSizeH*pDataPacket->wSizeV*pDataPacket->wBytesPerPixel;
					pDataPacket->dwFieldID				= dwFieldID++;
					if (pDataPacket)
					{
						// Liegen Daten vor?		    
						if (pDataPacket->bValid && pDataPacket->dwStartMarker == MAGIC_MARKER_START)
						{  	
							// Ist die Imagesize korrekt?
							if (pDataPacket->dwImageDataLen == (DWORD)pDataPacket->wSizeH*pDataPacket->wSizeV*pDataPacket->wBytesPerPixel)
							{
								pDataPacket->wSource = nChannel; // Fake the Source
								// Ist das Struktallignment im Blackfin das selbe wie hier?
								if (pDataPacket->wSize == sizeof(DATA_PACKET))
								{
									// Ja, dann Packet weiterreichen...
									if (pDataPacket->eType == eTypeYUV422)
										bResult = OnIndicationNewCodecData(pDataPacket);
								}
								else
								{
									OnIndicateError(TASHA_ERROR_DATA_PACKET_SIZE_WRONG);
									ML_TRACE_ERROR(_T("Size of DATA_PACKET wrong (Channel=%d) (%d != %d)\n"), nChannel, pDataPacket->wSize, sizeof(DATA_PACKET)); 
								}
							}
							else
							{
								OnIndicateError(TASHA_ERROR_IMAGE_SIZE_WRONG);
								ML_TRACE_ERROR(_T("Imagesize wrong (Channel=%d HSize=%d VSize=%d BPP=%d Size=%d Bytes\n"), nChannel, pDataPacket->wSizeH, pDataPacket->wSizeV, pDataPacket->wBytesPerPixel, pDataPacket->dwImageDataLen);
							}
						}
						else
						{
							OnIndicateWarning(TASHA_WARNING_UNVALID_DATA_PACKET);
							ML_TRACE_WARNING(_T("Unvalid data packet (Channel=%d)\n"), nChannel);
						}
					}
				}
			}
		}
	}
	else
		ML_TRACE(_T("Packet size too large (%d>%d)\n"), dwBufferLen, CHANNEL_SIZE*CHANNEL_COUNT/2);

	return bResult;
}
 
/////////////////////////////////////////////////////////////////////////////
UINT CCodec::MessageHandleThread(LPVOID pData)
{
	MESSAGE* pMsg = (MESSAGE*)pData;
	if (!pMsg)
		return 0;

	XTIB::SetThreadName(_T("MessageHandleThread"));

	int		nMessageID	= pMsg->nMessageID;
	DWORD	dwParam1	= pMsg->dwParam1;
	DWORD	dwParam2	= pMsg->dwParam2;
	DWORD	dwParam3	= pMsg->dwParam3;
	DWORD	dwParam4	= pMsg->dwParam4;
	CCodec*	pThis		= (CCodec*)pMsg->pCodec;  // Pointer auf's Codec-Objekt
	
	switch (nMessageID)
	{
		case TMM_CONFIRM_BF535_PING:
			pThis->OnConfirmBF535Ping();
			break;
		case TMM_CONFIRM_NEW_FRAME:
			pThis->OnConfirmNewSingleFrame(dwParam1, (WORD)dwParam2);
			break;
		case TMM_CONFIRM_SET_ENCODER_PARAMETER:
			{
				EncoderParam EncParam;
				//									20	  18				12          8  			   3	    0
				// F  F  F  F  F  F  F  F  F  F  F  F  E  E  D  D  D  D  D  D  C  C  C  C  B  B  B  B  B  A  A  A
				// A = Resolution (eImageResHigh, eImageResMid, eImageResLow)
				// B = Frames per second (1...25)
				// C = Compressiontype (eTypeMpeg4, eTypeYUV422...)
				// D = I-Frame Intervall (1...100)
				// E = StreamID (0,1)
				// F = Bitrate (64...4000KBits)
				EncParam.eRes				= (ImageRes)((dwParam3 >> 0)  & 0x07);
				EncParam.nFps				= (int)(	 (dwParam3 >> 3)  & 0x1f);
				EncParam.eType				= (DataType)((dwParam3 >> 8)  & 0x0f);
				EncParam.nIFrameIntervall	= (int)(	 (dwParam3 >> 12) & 0x3f);
				EncParam.nStreamID			= (int)(	 (dwParam3 >> 18) & 0x03);
				EncParam.nBitrate			= (int)(	 (dwParam3 >> 20) & 0xfff);

				pThis->OnConfirmSetEncoderParam(dwParam1, (WORD)dwParam2, EncParam);
			}
			break;
		case TMM_CONFIRM_SET_BRIGHTNESS:
			pThis->OnConfirmSetBrightness(dwParam1, (WORD)dwParam2, (int)dwParam3);
			break;
		case TMM_CONFIRM_SET_CONTRAST:
			pThis->OnConfirmSetContrast(dwParam1, (WORD)dwParam2, (int)dwParam3);
			break;
		case TMM_CONFIRM_SET_SATURATION:
			pThis->OnConfirmSetSaturation(dwParam1, (WORD)dwParam2, (int)dwParam3);
			break;
		case TMM_CONFIRM_GET_ENCODER_PARAMETER:
			{
				EncoderParam EncParam;
				//									20	  18				12          8  			   3	    0
				// F  F  F  F  F  F  F  F  F  F  F  F  E  E  D  D  D  D  D  D  C  C  C  C  B  B  B  B  B  A  A  A
				// A = Resolution (eImageResHigh, eImageResMid, eImageResLow)
				// B = Frames per second (1...25)
				// C = Compressiontype (eTypeMpeg4, eTypeYUV422...)
				// D = I-Frame Intervall (1...100)
				// E = StreamID (0,1)
				// F = Bitrate (64...4000KBits)
				EncParam.eRes				= (ImageRes)((dwParam3 >> 0)  & 0x07);
				EncParam.nFps				= (int)(	 (dwParam3 >> 3)  & 0x1f);
				EncParam.eType				= (DataType)((dwParam3 >> 8)  & 0x0f);
				EncParam.nIFrameIntervall	= (int)(	 (dwParam3 >> 12) & 0x3f);
				EncParam.nStreamID			= (int)(     (dwParam3 >> 18) & 0x03);
				EncParam.nBitrate			= (int)(	 (dwParam3 >> 20) & 0xfff);

				pThis->OnConfirmGetEncoderParam(dwParam1, (WORD)dwParam2, EncParam);
			}
			break;
		case TMM_CONFIRM_GET_BRIGHTNESS:
			pThis->OnConfirmGetBrightness(dwParam1, (WORD)dwParam2, (int)dwParam3);
			break;
		case TMM_CONFIRM_GET_CONTRAST:
			pThis->OnConfirmGetContrast(dwParam1, (WORD)dwParam2, (int)dwParam3);
			break;
		case TMM_CONFIRM_GET_SATURATION:
			pThis->OnConfirmGetSaturation(dwParam1, (WORD)dwParam2, (int)dwParam3);
			break;
		case TMM_CONFIRM_SET_THRESHOLD_MD:
			pThis->OnConfirmSetMDTreshold(dwParam1, (WORD)dwParam2, (int)dwParam3);
			break;
		case TMM_CONFIRM_SET_THRESHOLD_MASK:
			pThis->OnConfirmSetMaskTreshold(dwParam1, (WORD)dwParam2, (int)dwParam3);
			break;
		case TMM_CONFIRM_SET_INCREMENT_MASK:
			pThis->OnConfirmSetMaskIncrement(dwParam1, (WORD)dwParam2, (int)dwParam3);
			break;
		case TMM_CONFIRM_SET_DELAY_MASK:
			pThis->OnConfirmSetMaskDelay(dwParam1, (WORD)dwParam2, (int)dwParam3);
			break;
		case TMM_CONFIRM_GET_THRESHOLD_MD:
			pThis->OnConfirmGetMDTreshold(dwParam1, (WORD)dwParam2, (int)dwParam3);
			break;
		case TMM_CONFIRM_GET_THRESHOLD_MASK:
			pThis->OnConfirmGetMaskTreshold(dwParam1, (WORD)dwParam2, (int)dwParam3);
			break;
		case TMM_CONFIRM_GET_INCREMENT_MASK:
			pThis->OnConfirmGetMaskIncrement(dwParam1, (WORD)dwParam2, (int)dwParam3);
			break;
		case TMM_CONFIRM_GET_DELAY_MASK:
			pThis->OnConfirmGetMaskDelay(dwParam1, (WORD)dwParam2, (int)dwParam3);
			break;
		case TMM_CONFIRM_SET_PERMANENT_MASK:
			pThis->OnConfirmSetPermanentMask(dwParam1, (WORD)dwParam2, dwParam3, dwParam4);
			break;
		case TMM_CONFIRM_CHANGE_PERMANENT_MASK:
			pThis->OnConfirmChangePermanentMask(dwParam1, (WORD)dwParam2, (int)HIWORD(dwParam3), (int)LOWORD(dwParam3), (int)dwParam4);
			break;			
		case TMM_CONFIRM_INVERT_PERMANENT_MASK:
			pThis->OnConfirmInvertPermanentMask(dwParam1, (WORD)dwParam2);
			break;
		case TMM_CONFIRM_CLEAR_PERMANENT_MASK:
			pThis->OnConfirmClearPermanentMask(dwParam1, (WORD)dwParam2);
			break;			
		case TMM_CONFIRM_START_CAPTURE:
			pThis->OnConfirmStartCapture();
			break;
		case TMM_CONFIRM_STOP_CAPTURE:
			pThis->OnConfirmStopCapture();
			break;
		case TMM_CONFIRM_RESUME_CAPTURE:
			pThis->OnConfirmResumeCapture();
			break;
		case TMM_CONFIRM_PAUSE_CAPTURE:
			pThis->OnConfirmPauseCapture();
			break;
		case TMM_NEW_CODEC_DATA:
			if (pThis->m_bFrameSync)
				pThis->OnReceiveNewCodecDataEx(dwParam1, dwParam2, dwParam3);
			else
				pThis->OnReceiveNewCodecData(dwParam1, dwParam2, dwParam3);
			break;
		case TMM_CONFIRM_SCAN_FOR_CAMERAS:
			pThis->OnConfirmScanForCameras(dwParam1, dwParam2);
			break;
		case TMM_CONFIRM_SET_RELAIS_STATE:
			pThis->OnConfirmSetRelayState((WORD)dwParam1, (BOOL)dwParam2);
			break;
		case TMM_CONFIRM_GET_RELAIS_STATE:
			pThis->OnConfirmGetRelayState(dwParam1);
			break;
		case TMM_CONFIRM_GET_ALARM_STATE:
			pThis->OnConfirmGetAlarmState(dwParam1);
			break;
		case TMM_CONFIRM_SET_ALARM_EDGE:
			pThis->OnConfirmSetAlarmEdge(dwParam1);
			break;
		case TMM_CONFIRM_GET_ALARM_EDGE:
			pThis->OnConfirmGetAlarmEdge(dwParam1);
			break;
		case TMM_CONFIRM_SET_FRAMEBUFF_ADDR:
			pThis->OnConfirmSetPCIFrameBufferAddress(dwParam1);
			break;
		case TMM_CONFIRM_EPLD_DATA_TRANSFER:
			pThis->OnConfirmEPLDdataTransfer(dwParam1, dwParam2);
			break;
		case TMM_CONFIRM_BOOT_BF533:
			pThis->OnConfirmBoot533((int)dwParam1, dwParam2, dwParam3);
			break;
		case TMM_CONFIRM_TRANSFER_BOOT_LOADER:
			pThis->OnConfirmTransferBootloader(dwParam1, dwParam2);
			break;
		case TMM_CONFIRM_WRITE_EEPROM:
			pThis->OnConfirmWriteToEEProm(dwParam1, dwParam2, dwParam3);
			break;
		case TMM_CONFIRM_READ_EEPROM:
			pThis->OnConfirmReadFromEEProm(dwParam1, dwParam2, dwParam3);
			break;
		case TMM_CONFIRM_SET_CHANNEL:
			pThis->OnConfirmSetChannel((WORD)dwParam1, (WORD)dwParam2);
			break;
		case TMM_CONFIRM_INIT_COMPLETE:
			pThis->OnConfirmInitComplete();
			break;
		case TMM_CONFIRM_TERMINATION:
			pThis->OnConfirmTermination();
			break;
		case TMM_NOTIFY_ALARM_STATE_CHANGED:
			pThis->OnIndicationAlarmStateChanged(dwParam1);
			break;
		case TMM_NOTIFY_ADAPTER_SELECT_STATE_CHANGED:
			pThis->OnIndicationAdapterSelectStateChanged(dwParam1);
			break;
	 	case TMM_NOTIFY_PC_KEYS_STATE_CHANGED:
			pThis->OnIndicationPCKeysStateChanged(dwParam1);
			break;				
		case TMM_NOTIFY_INFORMATION:
			pThis->OnIndicationInformation((int)dwParam1, dwParam2, dwParam3, dwParam4);
			break;
		case TMM_NOTIFY_DEBUG_INFORMATION:
			pThis->OnIndicationDebugInformation(dwParam1, dwParam2);
			break;
		case TMM_NOTIFY_SLAVE_INIT_READY:
			pThis->OnNotifySlaveInitReady((WORD)dwParam1);
			break;			
		case TMM_CONFIRM_SET_ANALOG_OUT:
			pThis->OnConfirmSetAnalogOut(dwParam1);
			break;
		case TMM_CONFIRM_GET_ANALOG_OUT:
			pThis->OnConfirmGetAnalogOut(dwParam1);
			break;
		case TMM_CONFIRM_SET_TERMINATION_STATE:
			pThis->OnConfirmSetTerminationState((BYTE)dwParam1);
			break;
		case TMM_CONFIRM_GET_TERMINATION_STATE:
			pThis->OnConfirmGetTerminationState((BYTE)dwParam1);
			break;
		case TMM_CONFIRM_SET_VIDEO_ENABLE_STATE:
			pThis->OnConfirmSetVideoEnableState((BOOL)dwParam1);
			break;
		case TMM_CONFIRM_GET_VIDEO_ENABLE_STATE:
			pThis->OnConfirmGetVideoEnableState((BOOL)dwParam1);
			break;			
		case TMM_CONFIRM_SET_CROSSPOINT_ENABLE_STATE:
			pThis->OnConfirmSetCrosspointEnableState((BOOL)dwParam1);
			break;
		case TMM_CONFIRM_GET_CROSSPOINT_ENABLE_STATE:
			pThis->OnConfirmGetCrosspointEnableState((BOOL)dwParam1);
			break;
		case TMM_CONFIRM_ENABLE_WATCHDOG:
			pThis->OnConfirmEnableWatchdog((int)dwParam1);
			break;
		case TMM_CONFIRM_TRIGGER_WATCHDOG:
			pThis->OnConfirmTriggerWatchdog((int)dwParam1);
			break;
		case TMM_CONFIRM_SET_POWER_LED:
			pThis->OnConfirmSetPowerLED((BOOL)dwParam1);
			break;
		case TMM_CONFIRM_SET_RESET_LED:
			pThis->OnConfirmSetResetLED((BOOL)dwParam1);
			break;
		case TMM_CONFIRM_GET_VARIATION_REGISTER:
			pThis->OnConfirmGetVariationRegister((WORD)dwParam1);
			break;		
		case TMM_CONFIRM_GET_DIPSTATE:
			pThis->OnConfirmGetDIPState((WORD)dwParam1);
			break;		
		case TMM_CONFIRM_SET_NR:
			pThis->OnConfirmSetNR(dwParam1, (WORD)dwParam2, (BOOL)dwParam3);
			break;
		case TMM_CONFIRM_GET_NR:
			pThis->OnConfirmGetNR(dwParam1, (WORD)dwParam2, (BOOL)dwParam3);
			break;
		default:
			{
				pThis->OnIndicateWarning(TASHA_WARNING_UNKNOWN_MESSAGE);
				ML_TRACE_WARNING(_T("HOST: Unknown Message (MessageID=%d)\n"), nMessageID);
			}
	}
	
	WK_DELETE(pMsg);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
UINT CCodec::WaitForExternalEventThread(LPVOID pData)
{	 
	CCodec*	pCodec	 = (CCodec*)pData;  // Pointer auf's Codec-Objekt

	XTIB::SetThreadName(_T("ExternEventThread"));

	static HANDLE hEvent[3];
	
	hEvent[0] = pCodec->m_hWatchDogTriggerEvent;
	hEvent[1] = pCodec->m_hPiezoEvent;
	hEvent[2] = pCodec->m_hShutDownEvent;
	CWK_Profile prof;

	// Poll bis Mainprozeß terminiert werden soll.
	while (pCodec->m_bRun) 
	{
		// Warte auf externe Events
		switch (WaitForMultipleObjects(3, hEvent, FALSE, 1000))
		{
			case WAIT_TIMEOUT:
			case WAIT_FAILED:
				break;			
			case WAIT_OBJECT_0:
				pCodec->DoRequestTriggerWatchdog(prof.GetInt(_T("TashaUnit\\Device1\\General"), _T("WatchDogTimeout"), 60));
				break;
			case WAIT_OBJECT_0+1:
				pCodec->TriggerPiezo();
				break;
			case WAIT_OBJECT_0+2:		
				return 0;	
			default:
				break;
		}
	}

	ML_TRACE(_T("Exit ExternEventThread\n"));
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CCodec::TriggerPiezo()
{
	int nState = ReadConfigurationInt(_T("General"), _T("PiezoState"), -1, m_sAppIniName);

	switch (nState)
	{
		case 0:
			DoRequestSetRelayState(0, FALSE);
			break;
		case 1:
			DoRequestSetRelayState(0, TRUE);
			break;
		default:
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
UINT CCodec::ReadDataThread(LPVOID pData)
{
	static CSystemTime LastTimeStamp[MAX_STREAMS][MAX_CAMERAS];
	static int	nFrame[MAX_STREAMS][MAX_CAMERAS] = {0};
	static DWORD dwLastFieldID = 0;
	static CSystemTime LastTime; 
	// Gibt die Zuordnung zwischen Chennelnummer und DSP Nummer.
	static int nSourceMap[CHANNEL_COUNT] = {0,2,4,6,1,3,5,7};

	CCodec*	pThis	 = (CCodec*)pData;  // Pointer auf's Codec-Objekt
	
	XTIB::SetThreadName(_T("ReadDataThread"));

	// Thread solange ausführen, bis m_bRun = FALSE
	while (pThis && pThis->m_bRun)
	{
		// Soll der Frameheader gesucht werden?
		if (pThis->m_bFrameSync)
		{
			// Warte bis neue Daten eintreffen.
			if (WaitForSingleObject(pThis->m_hNewDataSemaphore, 500) == WAIT_OBJECT_0)
			{
				DATA_PACKET* pDataPacket = NULL;
				
				// Alle Channels nach neuen Frames durcsuchen.
				for (int nChannel = 0; nChannel < CHANNEL_COUNT; nChannel++)
				{
					// bVideoDataPresent enthält am ende den Status, ob gültige Bilddaten vorhanden waren.
					BOOL bValidDataPacket = FALSE;

					// Hole nächstes Frame aus dem Circularbuffer
					while ((pDataPacket = (DATA_PACKET*)pThis->m_pCirBuff[nChannel]->GetNextFrame())!= NULL)
					{
						// Ist das Struktalignment im Blackfin das selbe wie hier?
						if (pDataPacket->wSize == sizeof(DATA_PACKET))
						{	
							// Ist die Prüfsumme korrekt?
							DWORD dwCheckSum = pThis->GetCheckSum(pDataPacket);
							if (pDataPacket->dwCheckSum == dwCheckSum)
							{  	
								// Kameranummer gültig?
								if (pDataPacket->wSource < MAX_CAMERAS)
								{
									// StreamID gültig?
									if (pDataPacket->dwStreamID < MAX_STREAMS)
									{
										// Es liegen gültige Daten vom DSP vor.
										bValidDataPacket = TRUE;
						
										// Liegen Daten vor? (Der 2. Stream setzt dieses Flag auf FALSE, wenn kein 
										// Bild im packet vorhanden ist
										if (pDataPacket->bValid)
										{
											// Timestamp setzen, wenn nicht schon erfolgt.
											if (!pDataPacket->TimeStamp.bValid)
											{
												CSystemTime	TimeStamp;
												CSystemTime	TimeStamp2;
												TimeStamp.GetLocalTime();
												TimeStamp2 = TimeStamp;

												// Sind mehrere Bilder in der gleichen Millisekunde? Dann korrigiere den
												// Timestamp
												if (TimeStamp <= LastTimeStamp[pDataPacket->dwStreamID][pDataPacket->wSource])
													TimeStamp.IncrementTime(0,0,0,0,(++nFrame[pDataPacket->dwStreamID][pDataPacket->wSource])*40);
												else
													nFrame[pDataPacket->dwStreamID][pDataPacket->wSource] = 0; 

												LastTimeStamp[pDataPacket->dwStreamID][pDataPacket->wSource] = TimeStamp2;
												LastTimeStamp[pDataPacket->dwStreamID][pDataPacket->wSource].IncrementTime(0,0,0,0,10);

												pDataPacket->TimeStamp.wYear	= TimeStamp.wYear;
												pDataPacket->TimeStamp.wMonth	= TimeStamp.wMonth;
												pDataPacket->TimeStamp.wDay		= TimeStamp.wDay;
	  											pDataPacket->TimeStamp.wHour	= TimeStamp.wHour;
												pDataPacket->TimeStamp.wMinute	= TimeStamp.wMinute;
												pDataPacket->TimeStamp.wSecond	= TimeStamp.wSecond;
												pDataPacket->TimeStamp.wMSecond	= TimeStamp.wMilliseconds;
												pDataPacket->TimeStamp.bValid	= TRUE;

											}
											
											// Hat sich die permanente Maske verändert?...Dann die Registry aktualisieren.
											// Und die aktuelle Maske zur späteren Abfrage sichern.
											if (pDataPacket->PermanentMask.bHasChanged && pDataPacket->PermanentMask.bValid)
												pThis->SaveCurrentPermanentMask(&pDataPacket->PermanentMask, pDataPacket->wSource);

											// Ist eine gültige adaptive Maske vorhanden, dann diese zur späteren Abfrage
											// ebenfalls sichern
											if (pDataPacket->AdaptiveMask.bValid)
												pThis->SaveCurrentAdaptiveMask(&pDataPacket->AdaptiveMask, pDataPacket->wSource);
											
											// Hat einer der BF533 einen Debugstring geschickt?
											if (pDataPacket->Debug.bValid && pThis->m_bTraceBF533DebugText)
											{
												// Umwandlung in Unicode
												CString sMsg(pDataPacket->Debug.sText);
												ML_TRACE(_T("BF533 Message:%s"), sMsg);
											}
											
											if (pDataPacket->dwStreamID == 0)
											{
												// Realen Kameraausfall sofort melden!
												if (pDataPacket->bVidPresent)
												{
													if (!TSTBIT(pThis->m_dwCamMask, pDataPacket->wSource))
													{
														// Stellt eine Maske zusammen, die darüber Auskunft gibt, von welcher Kamera Daten gekommen sind
														pThis->m_dwCamMask = SETBIT(pThis->m_dwCamMask, pDataPacket->wSource);
														if (TSTBIT(pThis->m_dwCamScanMask, pDataPacket->wSource))
															pThis->OnIndicationCameraStatusChanged(pDataPacket->wSource, TRUE);														
														if (!pThis->m_bSilence)
															Beep(1000, 100);
													}
												}
												else
												{
													// Wenn kein Kamerasignal vorhanden ist, soll das Testbild verwendet werden.
													pDataPacket->eSubType = eSubIFrame;
													pDataPacket->dwImageDataLen = pThis->m_dwTestImageLen;
													CopyMemory(&pDataPacket->pImageData, pThis->m_lpTestImage, pThis->m_dwTestImageLen);

													if (TSTBIT(pThis->m_dwCamMask, pDataPacket->wSource))
													{
														// Stellt eine Maske zusammen, die darüber Auskunft gibt, von welcher Kamera Daten gekommen sind
														pThis->m_dwCamMask = CLRBIT(pThis->m_dwCamMask, pDataPacket->wSource);
										 				if (TSTBIT(pThis->m_dwCamScanMask, pDataPacket->wSource))
			 												pThis->OnIndicationCameraStatusChanged(pDataPacket->wSource, FALSE);			
														if (!pThis->m_bSilence)
															Beep(500, 100);
													}									
												}
												pThis->m_dwCamScanMask = SETBIT(pThis->m_dwCamScanMask, pDataPacket->wSource);	
											}
											//	ML_TRACE(_T("Cycle counter=%dys\n"), pDataPacket->dwProzessTime);
											// Packet weiterreichen...
											if (pDataPacket->dwImageDataLen > 0)
												pThis->OnIndicationNewCodecData(pDataPacket);
										}
										else
										{
											pThis->OnIndicateWarning(TASHA_WARNING_UNVALID_DATA_PACKET);
											ML_TRACE_WARNING(_T("Unvalid data packet (Channel=%d, StreamID=%d)\n"), nChannel, pDataPacket->dwStreamID);															
										}
									}
									else
									{
										pThis->OnIndicateWarning(TASHA_WARNING_UNVALID_STREAM_ID_IN_DATA_PACKET);
										ML_TRACE_WARNING(_T("CCodec::ReadDataThread Unvalid StreamID (%d)\n"), pDataPacket->dwStreamID);
									}
								}
								else
								{
									pThis->OnIndicateWarning(TASHA_WARNING_UNVALID_SOURCE_ID_IN_DATA_PACKET);
									ML_TRACE_WARNING(_T("CCodec::ReadDataThread Unvalid Source (Source=%d, StreamID=%d)\n"), pDataPacket->wSource, pDataPacket->dwStreamID);
								}
							}
							else
							{
								// Nach einem Prüfsummenfehler auf Stream1 sollte ein neues I-Frame angefordert werden.
								if (pDataPacket->dwStreamID == 1)
									pThis->ForceNewIFrame(pDataPacket->wSource, pDataPacket->dwStreamID);

								if (pThis->m_bTraceSPORTCheckSummError)
								{
									pThis->OnIndicateWarning(TASHA_WARNING_DATA_PACKET_CHECKSUM_FAILED);
									ML_TRACE_WARNING(_T("Unvalid data packet checksum (Channel=%d, StreamID=%d) ((H)%d != (S)%d)\n"), nChannel, pDataPacket->dwStreamID, dwCheckSum, pDataPacket->dwCheckSum);									
								}
								if (!pThis->m_bSilence)
									Beep(100+pDataPacket->wSource*100, 100);
							}
						}
						else
						{
							pThis->OnIndicateError(TASHA_ERROR_DATA_PACKET_SIZE_WRONG);
							ML_TRACE_ERROR(_T("Size of DATA_PACKET wrong (Channel=%d, StreamID=%d) (%d != %d)\n"), nChannel, pDataPacket->dwStreamID, pDataPacket->wSize, sizeof(DATA_PACKET)); 
						}
					}

					// Liegen gültige Bilddaten vor?
					if (bValidDataPacket)
					{
						pThis->m_dwLastValidDataPacketTC[nSourceMap[nChannel]] = WK_GetTickCount();
						// War der Status NICHT Valid? dann melde diese Statusänderung
						if (pThis->m_eVideoStreamState[nSourceMap[nChannel]] != VideoStreamValid)
						{
							// Neuen Streamstatus sichern
							pThis->m_eVideoStreamState[nSourceMap[nChannel]] = VideoStreamValid;

							// Statusänderung mitteilen
							pThis->OnIndicationVideoStreamResumed(nSourceMap[nChannel]);
							if (!pThis->m_bSilence)
								Beep(1000, 100);
						}
					}
					else					
					{
						// Liegt der Zeitpunkt der letzten gültigen Daten länger als 10 Sekunden zurück?
						if (WK_GetTimeSpan(pThis->m_dwLastValidDataPacketTC[nSourceMap[nChannel]]) > pThis->m_dwStreamRecoveryTimeout)
						{
							ML_TRACE(_T("Timespan: %d\n"), WK_GetTimeSpan(pThis->m_dwLastValidDataPacketTC[nSourceMap[nChannel]]));

							pThis->m_dwLastValidDataPacketTC[nSourceMap[nChannel]] = WK_GetTickCount();
							
							// War der letzte Status 'Valid' oder 'Unknown'
							if ((pThis->m_eVideoStreamState[nSourceMap[nChannel]] == VideoStreamValid) ||
								(pThis->m_eVideoStreamState[nSourceMap[nChannel]] == VideoStreamUnknown))
							{
								// Neuen Streamstatus sichern
								pThis->m_eVideoStreamState[nSourceMap[nChannel]] = VideoStreamAborted;
								
								// Statusänderung mitteilen
								pThis->OnIndicationVideoStreamAborted(nSourceMap[nChannel]);
								if (!pThis->m_bSilence)
									Beep(500, 100);
							}
						}
					}
				}
			}
		}
		else
			Sleep(10);	// Verhindert eine zu große Prozessorauslastung im 'Nicht Famesync Modus'
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnIndicationNewCodecData(DATA_PACKET* pDataPacket)
{
	static DWORD dwLastFieldID[MAX_CAMERAS][MAX_STREAMS] = {0};
	static DWORD dwLastFieldType[MAX_CAMERAS][MAX_STREAMS] = {eSubPFrame};
	static BOOL	 bWaitToNextIFrame[MAX_CAMERAS][MAX_STREAMS] = {TRUE};
	BOOL	bResult = FALSE;

	// Wenn ein Field fehlt, dann warte bei diesem Stream auf das nächste I-Frame.
	// Liefert TRUE, wenn Frame ok.
	if (pDataPacket)
	{
		WORD wSource	= pDataPacket->wSource;
		WORD wStreamID	= (WORD)pDataPacket->dwStreamID;

		if ((wSource >=VIDEO_SOURCE0 && wSource <= VIDEO_SOURCE7) && (wStreamID < MAX_STREAMS))
		{
			// Fehlt ein Field?
			if (pDataPacket->dwFieldID > 0 && (pDataPacket->dwFieldID != dwLastFieldID[wSource][wStreamID]+1))
			{
				OnIndicateError(TASHA_ERROR_UNEXPECTED_FIELD_ID);
				if (m_bTraceMissingFields)
				{	 
					ML_TRACE_ERROR(_T("FieldID wrong, frames dropped (Source=%d, StreamID=%d, (H)FieldID=%d , (S)FieldID=%d (H)FrameType=%d (S)FrameType=%d (ChkSum=%d)\n"),
											wSource, wStreamID, dwLastFieldID[wSource][wStreamID],  pDataPacket->dwFieldID,
											dwLastFieldType[wSource][wStreamID], pDataPacket->eSubType, pDataPacket->dwCheckSum);
				}
				
				// Wenn ein Field fehlt, und das aktuelle ist kein I-Frame, dann muß für Stream0 auf das nächste I-Frame
				// gewartet werden, und bei Stream1 ein I-Frame angefordert werden.
				if (pDataPacket->eSubType != eSubIFrame)
				{
					if (wStreamID == 1)
						ForceNewIFrame(wSource, wStreamID);

					// Es ist ein Prüfsummenfehler erkannt worden. ->Resync aufs nächste I-Frame
					bWaitToNextIFrame[pDataPacket->wSource][wStreamID] = TRUE;
				}
			}
			dwLastFieldID[wSource][wStreamID] = pDataPacket->dwFieldID;
			dwLastFieldType[wSource][wStreamID] = pDataPacket->eSubType;

			// Erzwinge für Stream1 das vorgegebene I-Frame Intervall
			if (wStreamID == 1)
			{
				 if (pDataPacket->eSubType != eSubIFrame)
				 {
					CString sSection;
					sSection.Format(_T("Slaves\\Source%02u\\EncoderParams\\StreamID%d"), wSource, wStreamID);

					m_nPFrameCounter[wSource]++;
					if (m_nPFrameCounter[wSource] >= (int)ReadConfigurationInt(sSection, _T("I-FrameIntervall"), 6, m_sAppIniName))
					{
						m_nPFrameCounter[wSource] = 0;
						ForceNewIFrame(wSource, wStreamID);
						bWaitToNextIFrame[pDataPacket->wSource][wStreamID] = TRUE;
					}
				 }
				 else
					m_nPFrameCounter[wSource] = 0;
			}

			// Warte auf das nächste I-Frame
			if ((bWaitToNextIFrame[pDataPacket->wSource][wStreamID] == FALSE) || (pDataPacket->eSubType == eSubIFrame))
			{
				bWaitToNextIFrame[pDataPacket->wSource][wStreamID] = FALSE;
				bResult = TRUE;
			}
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
DWORD CCodec::GetCheckSum(const DATA_PACKET* pPacket)
{
	DWORD dwCheckSum = 0;
	BYTE* pImageData = (BYTE*)&pPacket->pImageData;

	for (DWORD dwI = 0; dwI < pPacket->dwImageDataLen; dwI++)
		dwCheckSum += pImageData[dwI];

	return dwCheckSum;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestScanForCameras(DWORD dwUserData)
{
	BOOL bResult = FALSE;
	ML_TRACE(_T("HOST: DoRequestScanForCameras (UserData=0x%x)\n"), dwUserData);

	// Schickt die in 'ReadDataThread' gesammelten Informationen zurück. Sofern
	// von allen 8 Kameras Daten vorliegen.
	if (m_dwCamScanMask == 0xff)
		bResult = OnConfirmScanForCameras(dwUserData, m_dwCamMask);
	else
		ML_TRACE(_T("DoRequestScanForCameras: Not all camerastati available (0x%x)\n"), m_dwCamScanMask);


	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask)
{
	ML_TRACE(_T("HOST: OnConfirmScanForCameras (UserData=0x%x, CameraMask=0x%x)\n"), dwUserData, dwCameraMask);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetRelayState(WORD wRelayID, BOOL bOpenClose)
{
	if (m_bTraceSetRelais)
		ML_TRACE(_T("HOST: DoRequestSetRelayState (Relay=%d State=%d)\n"), wRelayID, bOpenClose);

	return SendMessage(TMM_REQUEST_SET_RELAIS_STATE, wRelayID, bOpenClose);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose)
{
	if (m_bTraceSetRelais)
		ML_TRACE(_T("HOST: OnConfirmSetRelayState (Relay=%d State=%d)\n"), wRelayID, bOpenClose);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetRelayState()
{
	if (m_bTraceGetRelais)
		ML_TRACE(_T("HOST: DoRequestGetRelayState\n"));
	return SendMessage(TMM_REQUEST_GET_RELAIS_STATE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetRelayState(DWORD dwRelayStateMask)
{
	if (m_bTraceGetRelais)
		ML_TRACE(_T("HOST: OnConfirmGetRelayState (RelayStateMask=0x%x)\n"), dwRelayStateMask);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetAlarmState()
{
	if (m_bTraceGetAlarmState)
		ML_TRACE(_T("HOST: DoRequestGetAlarmState\n"));
	return SendMessage(TMM_REQUEST_GET_ALARM_STATE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetAlarmState(DWORD dwAlarmStateMask)
{
	if (m_bTraceGetAlarmState)
		ML_TRACE(_T("HOST: OnConfirmGetAlarmState (AlarmState=0x%x)\n"), dwAlarmStateMask);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetAlarmEdge(DWORD dwAlarmEdge)
{
	ML_TRACE(_T("HOST: DoRequestSetAlarmEdge (AlarmEdge=0x%x)\n"), dwAlarmEdge);
	return SendMessage(TMM_REQUEST_SET_ALARM_EDGE, dwAlarmEdge);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetAlarmEdge(DWORD dwAlarmEdge)
{
	ML_TRACE(_T("HOST: OnConfirmSetAlarmEdge (AlarmEdge=0x%x)\n"), dwAlarmEdge);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetAlarmEdge()
{
	ML_TRACE(_T("HOST: DoRequestGetAlarmEdge\n"));
	return SendMessage(TMM_REQUEST_GET_ALARM_EDGE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetPCIFrameBufferAddress(DWORD dwPCIAddr)
{
	BOOL bResult = FALSE;

	ML_TRACE(_T("HOST: DoRequestSetPCIFrameBufferAddress (Addr=0x%08x)\n"), dwPCIAddr);

	m_evSetPCIFrameBufferAddress.ResetEvent();
	if (SendMessage(TMM_REQUEST_SET_FRAMEBUFF_ADDR, dwPCIAddr))
	{
		if (WaitForSingleObject(m_evSetPCIFrameBufferAddress, 2000) == WAIT_TIMEOUT)
		{
			OnIndicateWarning(TASHA_WARNING_SET_PCI_PCI_BUFFADDR_TIMEOUT);
			ML_TRACE_WARNING(_T("DoRequestSetPCIFrameBufferAddress: timeout\n"));
		}
		else
			bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetPCIFrameBufferAddress(DWORD dwPCIAddr)
{
	ML_TRACE(_T("HOST: OnConfirmSetPCIFrameBufferAddress (Addr=0x%08x)\n"), dwPCIAddr);
	m_evSetPCIFrameBufferAddress.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetAlarmEdge(DWORD dwAlarmEdge)
{
	ML_TRACE(_T("HOST: OnConfirmGetAlarmEdge (AlarmEdge=0x%x)\n"),dwAlarmEdge);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmInitComplete()
{
	ML_TRACE(_T("HOST: OnConfirmInitComplete\n"));
//	Beep(440, 100);
	m_evTmmInitComplete.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestTermination()
{
	ML_TRACE(_T("HOST: DoRequestTermination\n"));

	BOOL bResult = FALSE;

	m_evTmmTermination.ResetEvent();
	if (SendMessage(TMM_REQUEST_TERMINATION))
	{
		if (WaitForSingleObject(m_evTmmTermination, 1000) == WAIT_TIMEOUT)
		{
			OnIndicateWarning(TASHA_WARNING_TERMINATION_TIMEOUT);
			ML_TRACE_WARNING(_T("HOST: DoRequestTermination timeout\n"));
		}
		else
			bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::TransferEPLDdata(const CString &sFilename)
{
	BOOL bResult = FALSE;

	CFile file;
	if (file.Open(sFilename, CFile::modeRead | CFile::shareDenyNone))
	{
		int nLen = file.GetLength();
		BYTE* pBuffer = new BYTE[nLen];
		if (file.Read(pBuffer, nLen) == (DWORD)nLen)
			bResult = DoRequestEPLDdataTransfer(pBuffer, nLen);
		WK_DELETE(pBuffer);
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestEPLDdataTransfer(const BYTE* pData, DWORD dwLen)
{
	BOOL bResult = FALSE;

	if (dwLen < m_memTransferBuffer.dwLen)
	{
		// EPLD-Daten zum BF535 schicken.
		if (DAWriteMemoryX((void*)pData, (DWORD)m_memTransferBuffer.pLinAddr, dwLen))
		{
			ML_TRACE(_T("HOST: DoRequestEPLDdataTransfer (Addr=0x%x, dwLen=%d)\n"), m_memTransferBuffer.pPhysAddr, dwLen);

			// BF535 über das Eintreffen der neuen EPLD-Daten informieren.
			m_evEPLDdataTransfer.ResetEvent();
			if (SendMessage(TMM_REQUEST_EPLD_DATA_TRANSFER, (DWORD)m_memTransferBuffer.pPhysAddr, dwLen))
			{
				if (WaitForSingleObject(m_evEPLDdataTransfer, 2000) == WAIT_TIMEOUT)
				{
					OnIndicateWarning(TASHA_WARNING_TRANSFER_EPLDDATA_TIMEOUT);
					ML_TRACE_WARNING(_T("DoRequestEPLDdataTransfer: timeout\n"));
				}
				else
					bResult = TRUE;
			}
		}
		else
		{
			OnIndicateError(TASHA_ERROR_TRANSFER_EPLDDATA_FAILED);
			ML_TRACE_ERROR(_T("DoRequestEPLDdataTransfer: download data failes\n"));
		}
	}
	else
	{
		OnIndicateError(TASHA_ERROR_EPLDDATA_TOO_LARGE);
		ML_TRACE_ERROR(_T("DoRequestEPLDdataTransfer: Data to large (%d > %d)\n"), dwLen, m_memTransferBuffer.dwLen);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmEPLDdataTransfer(DWORD dwAddr, DWORD dwLen)
{
	ML_TRACE(_T("HOST: OnConfirmEPLDdataTransfer Addr=0x%x, dwLen=%d\n"), dwAddr, dwLen);

	m_evEPLDdataTransfer.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::TransferBF533BootData(int nBFNr, int nMaxBootAttempts/*=3*/)
{
	BOOL bResult = FALSE;
	BYTE* pBuffer = NULL;

	CString sFilename;
//	sFilename.Format(_T(GetHomeDir()+BOOT_IMAGE_533), nBFNr);
	sFilename.Format(GetHomeDir()+BOOT_IMAGE_533, nBFNr);
	
	CFile file;

	if (file.Open(sFilename, CFile::modeRead | CFile::shareDenyNone))
	{
		int nLen = file.GetLength();
		pBuffer = new BYTE[nLen];
		
		if (file.Read(pBuffer, nLen) == (DWORD)nLen)
		{
			// Maximal 3 Versuche pro DSP
			for (int nTrials = 1; nTrials <= nMaxBootAttempts && !bResult; nTrials++)
			{
				m_csBF533Boot.Lock();
				if (DoRequestBoot533(nBFNr, pBuffer, nLen))
					bResult = TRUE;		
				m_csBF533Boot.Unlock();
			}
		}		
		else
		{
			OnIndicateError(TASHA_ERROR_BF533_IMAGE_FILE_READ_FAILED);
			ML_TRACE_ERROR(_T("file.Read failed\n"));
		}

		WK_DELETE(pBuffer);
	}
	else
	{
		OnIndicateError(TASHA_ERROR_BF533_IMAGE_FILE_OPEN_FAILED);
		ML_TRACE_ERROR(_T("file.Open failed <%s>\n"), sFilename);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestBoot533(int nBFNr, const BYTE* pData, DWORD dwLen)
{
	BOOL bResult = FALSE;

	if (dwLen < m_memTransferBuffer.dwLen)
	{
		// Bootdatei zum BF535 schicken.
		if (DAWriteMemoryX((void*)pData, (DWORD)m_memTransferBuffer.pLinAddr, dwLen))
		{
			ML_TRACE(_T("HOST: DoRequestBoot533 (BF533 Nr.%d, Addr=0x%x dwLen=%d)\n"), nBFNr, (DWORD)m_memTransferBuffer.pPhysAddr, dwLen);

			// BF535 über das Eintreffen der neuen BF533-Bootdaten informieren.
			m_evBootBF533.ResetEvent();
			
			// ValidData-Timeout zurücksetzen
			m_dwLastValidDataPacketTC[nBFNr] = WK_GetTickCount();
			if (m_eVideoStreamState[nBFNr] == VideoStreamAborted)
				m_eVideoStreamState[nBFNr] = VideoStreamUnknown;

			if (SendMessage(TMM_REQUEST_BOOT_BF533, nBFNr, (DWORD)m_memTransferBuffer.pPhysAddr, dwLen))
			{
				// Alle bisherigen Daten verwerfen.
				m_pCirBuff[nBFNr]->Flush();

				if (WaitForSingleObject(m_evBootBF533, 1000) == WAIT_TIMEOUT)
				{
					OnIndicateWarning(TASHA_WARNING_REQUEST_BOOT_BF533_TIMEOUT);
					ML_TRACE_WARNING(_T("DoRequestBoot533: timeout (BF533 Nr.%d)\n"), nBFNr);
				}
				else
					bResult = TRUE;
			}
		}
		else
		{
			ML_TRACE_ERROR(_T("DoRequestBoot533: failed\n"));
		}
	}
	else
	{
		OnIndicateError(TASHA_ERROR_BF533_IMAGE_FILE_TOO_LARGE);
		ML_TRACE_ERROR(_T("DoRequestBoot533: Data to large (%d > %d)\n"), dwLen, m_memTransferBuffer.dwLen);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmBoot533(int nBFNr, DWORD dwAddr, DWORD dwLen)
{
	ML_TRACE(_T("HOST: OnConfirmBoot533 (BF533 Nr.%d, Addr=0x%x, dwLen=%d)\n") , nBFNr, dwAddr, dwLen);

	m_evBootBF533.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::TransferBootloader(const CString &sFilename)
{
	BOOL bResult = FALSE;
	BYTE* pBuffer = NULL;
	
	CFile file;

	if (file.Open(sFilename, CFile::modeRead | CFile::shareDenyNone))
	{
		int nLen = file.GetLength();
		pBuffer = new BYTE[nLen];
		
		if (file.Read(pBuffer, nLen) == (DWORD)nLen)
			bResult = DoRequestTransferBootloader(pBuffer, nLen);

		WK_DELETE(pBuffer);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestTransferBootloader(const BYTE* pData, DWORD dwLen)
{
	BOOL bResult = FALSE;

	if (dwLen < m_memTransferBuffer.dwLen)
	{
		// Bootdatei zum BF535 schicken.
		if (DAWriteMemoryX((void*)pData, (DWORD)m_memTransferBuffer.pLinAddr, dwLen))
		{
			ML_TRACE(_T("HOST: DoRequestTransferBootloader (Addr=0x%x dwLen=%d)\n"), (DWORD)m_memTransferBuffer.pPhysAddr, dwLen);

			// BF535 über das Eintreffen der neuen BF533-Bootdaten informieren.
			m_evBootLoader.ResetEvent();
			if (SendMessage(TMM_REQUEST_TRANSFER_BOOT_LOADER, (DWORD)m_memTransferBuffer.pPhysAddr, dwLen))
			{
				if (WaitForSingleObject(m_evBootLoader, 2000) == WAIT_TIMEOUT)
				{
					OnIndicateWarning(TASHA_WARNING_BOOT_LOADER_TRANSFER_TIMEOUT);
					ML_TRACE_WARNING(_T("DoRequestTransferBootloader: timeout\n"));
				}
				else
					bResult = TRUE;
			}
		}
		else
		{
			OnIndicateError(TASHA_ERROR_BOOT_LOADER_TRANSFER_FAILED);
			ML_TRACE_ERROR(_T("DoRequestTransferBootloader: failed\n"));
		}
	}
	else
	{
		OnIndicateError(TASHA_ERROR_BOOT_LOADER_FILE_TOO_LARGE);
		ML_TRACE_ERROR(_T("DoRequestTransferBootloader: Data to large (%d > %d)\n"), dwLen, m_memTransferBuffer.dwLen);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmTransferBootloader(DWORD dwAddr, DWORD dwLen)
{
	ML_TRACE(_T("HOST: OnConfirmTransferBootloader (Addr=0x%x, dwLen=%d)\n") , dwAddr, dwLen);

	m_evBootLoader.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnNotifySlaveInitReady(WORD wSource)
{
	ML_TRACE(_T("HOST: >>> OnNotifySlaveInitReady (BF533 Nr.%d)\n\n") , wSource);

	if (!m_bSilence)
		Beep(400+400*wSource, 100);

	// Timeoutcounter zurück setzen.
	m_dwLastValidDataPacketTC[wSource] = WK_GetTickCount();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::WriteEEPromData(const CString &sFilename)
{
	BOOL bResult = FALSE;
	BOOL bError  = FALSE;

  	BYTE* pBuffer1 = NULL;
  	BYTE* pBuffer2 = NULL;

	CFile file;

	if (file.Open(sFilename, CFile::modeRead | CFile::shareDenyNone))
	{
		int nLen = file.GetLength();
		pBuffer1 = new BYTE[nLen];
		if (file.Read(pBuffer1, nLen) == (DWORD)nLen)
		{
			if (DoRequestWriteToEEProm(pBuffer1, nLen, 0))
			{
				bResult = TRUE;
				pBuffer2 = new BYTE[nLen];
				if (DoRequestReadFromEEProm(pBuffer2, nLen, 0))
				{
					for (int nI = 0; nI < nLen; nI++)
					{
//						TRACE(_T("Addr=%d\t%d\t%d\n"), nI, pBuffer1[nI], pBuffer2[nI]);
						if (pBuffer1[nI] != pBuffer2[nI])
							bError = TRUE;
					}
				}
				WK_DELETE(pBuffer2);
			}
		}
		
		WK_DELETE(pBuffer1);
	}

	if ((bResult) && bError)
	{
		OnIndicateError(TASHA_ERROR_EEPROM_READBACK_FAILED);
		ML_TRACE_ERROR(_T("EEPROM Readback test failed\n"));
		AfxMessageBox(IDS_EEPROM_READBACK_FAILED, MB_OK);
	}

	return bResult && !bError;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestWriteToEEProm(const BYTE* pData, DWORD dwLen, DWORD dwStartAddr)
{
	BOOL bResult = FALSE;

	if (dwLen < m_memTransferBuffer.dwLen)
	{
		// EPLD-Daten zum BF535 schicken.
		if (DAWriteMemoryX((void*)pData, (DWORD)m_memTransferBuffer.pLinAddr, dwLen))
		{
			ML_TRACE(_T("HOST: DoRequestWriteToEEProm (Addr=0x%x, dwLen=%d, dwStartAddr=0x%x)\n"), m_memTransferBuffer.pPhysAddr, dwLen, dwStartAddr);

			// BF535 über das Eintreffen der neuen EEProm-Daten informieren.
			m_evEEPromDataTransfer.ResetEvent();
			if (SendMessage(TMM_REQUEST_WRITE_EEPROM, (DWORD)m_memTransferBuffer.pPhysAddr, dwLen, dwStartAddr))
			{
				if (WaitForSingleObject(m_evEEPromDataTransfer, 5000) == WAIT_TIMEOUT)
//				if (WaitForSingleObject(m_evEEPromDataTransfer, 10*dwLen) == WAIT_TIMEOUT)
				{
					OnIndicateWarning(TASHA_WARNING_WRITE_TO_EEPROM_TIMEOUT);
					ML_TRACE_WARNING(_T("DoRequestWriteToEEProm: timeout\n"));
				}
				else
					bResult = TRUE;
			}
		}
		else
		{
			ML_TRACE_ERROR(_T("DoRequestWriteToEEProm: download data failes\n"));
		}
	}
	else
	{
		OnIndicateError(TASHA_ERROR_WRITE_TO_EEPROM_DATALEN_TOO_LARGE);
		ML_TRACE_ERROR(_T("DoRequestWriteToEEProm: Data to large (%d > %d)\n"), dwLen, m_memTransferBuffer.dwLen);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmWriteToEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr)
{
	ML_TRACE(_T("HOST: OnConfirmWriteToEEProm Addr=0x%x, dwLen=%d dwStartAddr=0x%x\n"), dwAddr, dwLen, dwStartAddr);

	m_evEEPromDataTransfer.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::ReadEEPromData(const CString &sFilename)
{
	BOOL bResult = FALSE;

	CFile file;
	if (file.Open(sFilename, CFile::modeWrite|CFile::modeCreate))
	{
		int nLen = 8*1024; // 8KByte EEProm
		BYTE* pData = new BYTE[nLen];

		if (DoRequestReadFromEEProm(pData, nLen, 0))
			file.Write(pData, nLen);

		WK_DELETE(pData);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestReadFromEEProm(BYTE* pData, DWORD dwLen, DWORD dwStartAddr)
{
	BOOL bResult = FALSE;

	if (dwLen < m_memTransferBuffer.dwLen)
	{
		if (m_bTraceReadFromEEProm)
			ML_TRACE(_T("HOST: DoRequestReadFromEEProm (Addr=0x%x, dwLen=%d, dwStartAddr=0x%x)\n"), m_memTransferBuffer.pPhysAddr, dwLen, dwStartAddr);

		// BF535 über das Eintreffen der neuen EEProm-Daten informieren.
		m_evEEPromDataTransfer.ResetEvent();
		if (SendMessage(TMM_REQUEST_READ_EEPROM, (DWORD)m_memTransferBuffer.pPhysAddr, dwLen, dwStartAddr))
		{
//			if (WaitForSingleObject(m_evEEPromDataTransfer, 10*dwLen) == WAIT_TIMEOUT)
			if (WaitForSingleObject(m_evEEPromDataTransfer, 5000) == WAIT_TIMEOUT)
			{
				OnIndicateWarning(TASHA_WARNING_READ_FROM_EEPROM_TIMEOUT);
				ML_TRACE_WARNING(_T("DoRequestReadFromEEProm: timeout\n"));
			}
			else
				bResult = DAReadMemoryX((void*)pData, (DWORD)m_memTransferBuffer.pLinAddr, dwLen);
		}
	}
	else
	{
		OnIndicateError(TASHA_ERROR_READ_FROM_EEPROM_DATALEN_TOO_LARGE);
		ML_TRACE_ERROR(_T("DoRequestReadFromEEProm: Data to large (%d > %d)\n"), dwLen, m_memTransferBuffer.dwLen);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetPermanentMask(DWORD dwUserData, WORD wSource, const MASK_STRUCT& mask, BOOL bWait)
{
	BOOL bResult = FALSE;
	enum mdsErrorCode Status;

	// Permanente Maske zum BF535 schicken.
	DWORD dwBufferOffset = PERMMASKS+wSource*sizeof(MASK_STRUCT);
	Status = mdsBoardWriteMemory(m_hDSP, MDS_DSP_MEM_DATA, sizeof(MASK_STRUCT), dwBufferOffset, (void*)&mask, MDS_DSP_INDEX_A);

	// check return value
	if (Status == MDS_ERROR_NO_ERROR)
	{
		if (m_bTraceSetPermanentMask)
			ML_TRACE(_T("HOST: DoRequestSetPermanentMask (dwUserData=0x%x wSource=%d Addr=0x%x, dwLen=%d)\n"), dwUserData, wSource, dwBufferOffset, sizeof(MASK_STRUCT));

		// BF535 über das Eintreffen der neuen EEProm-Daten informieren.
		m_evSetPermanentMask.ResetEvent();
		if (SendMessage(TMM_REQUEST_SET_PERMANENT_MASK, dwUserData, wSource, dwBufferOffset, sizeof(MASK_STRUCT)))
		{
			if (bWait)
			{
				if (WaitForSingleObject(m_evSetPermanentMask, 2000) == WAIT_TIMEOUT)
				{
					OnIndicateWarning(TASHA_WARNING_SET_PERMANENT_MASK_TIMEOUT);
					ML_TRACE_WARNING(_T("DoRequestSetPermanentMask: timeout (wSource=%d)\n"), wSource);
				}
				else
					bResult = TRUE;
			}
			else
				bResult = TRUE;
		}
	}
	else
	{
		OnIndicateError(TASHA_ERROR_SET_PERMANENT_MASK_FAILED);
		ML_TRACE_ERROR(_T("DoRequestSetPermanentMask: download permanent mask failed (wSource=%d)\n"), wSource);
	}

  	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetPermanentMask(DWORD dwUserData, WORD wSource, DWORD dwAddr, DWORD dwLen)
{
	if (m_bTraceSetPermanentMask)
		ML_TRACE(_T("HOST: OnConfirmSetPermanentMask dwUserData=0x%x, wSource=%d Addr=0x%x, dwLen=%d\n"), dwUserData, wSource, dwAddr, dwLen);

	m_evSetPermanentMask.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestClearPermanentMask(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceClearPermanentMask)
		ML_TRACE(_T("HOST: DoRequestClearPermanentMask (UserData=0x%x Source=%d)\n"), dwUserData, wSource);
	
	return SendMessage(TMM_REQUEST_CLEAR_PERMANENT_MASK, dwUserData, wSource);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmClearPermanentMask(DWORD dwUserData, WORD wSource)
{
	BOOL bResult = FALSE;

	if (m_bTraceClearPermanentMask)
		ML_TRACE(_T("HOST: OnConfirmClearPermanentMask (UserData=0x%x Source=%d\n"), dwUserData, wSource);
	
	// Die Maske in der Registrierung aktualisieren.
	MASK_STRUCT* pPermanentMask = new MASK_STRUCT;
	if (pPermanentMask)
	{
		pPermanentMask->bValid		= TRUE;
		pPermanentMask->nDimH		= MD_RESOLUTION_H/8;
		pPermanentMask->nDimV		= MD_RESOLUTION_V/8;

		for (int nY=0; nY < pPermanentMask->nDimV; nY++)
		{
			for (int nX=0; nX < pPermanentMask->nDimH; nX++)
				pPermanentMask->byMask[nY][nX] = 0xff;
		}
		bResult = SavePermanentMaskToRegistry(pPermanentMask, wSource);
	}
	WK_DELETE(pPermanentMask);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestInvertPermanentMask(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceInvertPermanentMask)
		ML_TRACE(_T("HOST: DoRequestInvertPermanentMask (UserData=0x%x Source=%d)\n"), dwUserData, wSource);
	
	return SendMessage(TMM_REQUEST_INVERT_PERMANENT_MASK, dwUserData, wSource);
}	 

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmInvertPermanentMask(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceInvertPermanentMask)
		ML_TRACE(_T("HOST: OnConfirmInvertPermanentMask (UserData=0x%x Source=%d)\n"), dwUserData, wSource);
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestEnableWatchdog(int nBF533ResponseTimeOut)
{
	if (m_bTraceEnableWatchdog)
		ML_TRACE(_T("HOST: DoRequestEnableWatchdog (BF533ResponseTimeOut=%d)\n"), nBF533ResponseTimeOut);

	return SendMessage(TMM_REQUEST_ENABLE_WATCHDOG, nBF533ResponseTimeOut);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmEnableWatchdog(int nBF533ResponseTimeOut)
{
	if (m_bTraceEnableWatchdog)
		ML_TRACE(_T("HOST: OnConfirmEnableWatchdog (BF533ResponseTimeOut=%d)\n"), nBF533ResponseTimeOut);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestTriggerWatchdog(int nWatchdogTimeOut)
{
	if (m_bTraceTriggerWatchdog)
		ML_TRACE(_T("HOST: DoRequestTriggerWatchdog (WatchdogTimeOut=%d Sekunden)\n"), nWatchdogTimeOut);

	return SendMessage(TMM_REQUEST_TRIGGER_WATCHDOG, nWatchdogTimeOut);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmTriggerWatchdog(int nWatchdogTimeOut)
{
	if (m_bTraceTriggerWatchdog)
		ML_TRACE(_T("HOST: OnConfirmTriggerWatchdog (WatchdogTimeOut=%d Sekunden)\n"), nWatchdogTimeOut);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetPowerLED(BOOL bState)
{
	if (m_bTraceSetPowerLED)
		ML_TRACE(_T("HOST: DoRequestSetPowerLED(%s)\n"), bState ?_T("TRUE"):_T("FALSE"));

	return SendMessage(TMM_REQUEST_SET_POWER_LED, bState);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetPowerLED(BOOL bState)
{
	if (m_bTraceSetPowerLED)
		ML_TRACE(_T("HOST: OnConfirmSetPowerLED(%s)\n"), bState ?_T("TRUE"):_T("FALSE"));

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetResetLED(BOOL bState)
{
	if (m_bTraceSetResetLED)
		ML_TRACE(_T("HOST: DoRequestSetResetLED(%s)\n"), bState ?_T("TRUE"):_T("FALSE"));

	return SendMessage(TMM_REQUEST_SET_RESET_LED, bState);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetResetLED(BOOL bState)
{
	if (m_bTraceSetResetLED)
		ML_TRACE(_T("HOST: OnConfirmSetResetLED(%s)\n"), bState ?_T("TRUE"):_T("FALSE"));

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetVariationRegister(WORD& wValue)
{
	BOOL bResult = FALSE;

	if (m_bTraceGetVariationRegister)
		ML_TRACE(_T("HOST: DoRequestGetVariationRegister\n"));

	m_evGetVariationRegister.ResetEvent();
	if (SendMessage(TMM_REQUEST_GET_VARIATION_REGISTER))
	{
		if (WaitForSingleObject(m_evGetVariationRegister, 1000) == WAIT_TIMEOUT)
		{
			OnIndicateWarning(TASHA_WARNING_GET_VARIATION_REG_TIMEOUT);
			ML_TRACE_WARNING(_T("DoRequestGetVariationRegister: timeout\n"));
		}
		else
		{
			wValue = m_wVariationRegister;
			bResult = TRUE;
		}
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetVariationRegister(WORD wValue)
{
	if (m_bTraceGetVariationRegister)
		ML_TRACE(_T("HOST: OnConfirmGetVariationRegister\n"));

	m_wVariationRegister = wValue;
	m_evGetVariationRegister.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetDIPState(int& nValue)
{
	BOOL bResult = FALSE;

	if (m_bTraceGetDIPState)
		ML_TRACE(_T("HOST: DoRequestGetDIPState\n"));

	m_evGetDIPState.ResetEvent();
	if (SendMessage(TMM_REQUEST_GET_DIPSTATE))
	{
		if (WaitForSingleObject(m_evGetDIPState, 1000) == WAIT_TIMEOUT)
		{
			OnIndicateWarning(TASHA_WARNING_GET_DIPSTATE_TIMEOUT);
			ML_TRACE_WARNING(_T("DoRequestGetDIPState: timeout\n"));
		}
		else
		{
			nValue = m_nDIPState;
			bResult = TRUE;
		}
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetDIPState(int nValue)
{
	if (m_bTraceGetDIPState)
		ML_TRACE(_T("HOST: OnConfirmGetDIPState (0x%x)\n"), nValue);

	m_nDIPState = nValue;
	m_evGetDIPState.SetEvent();

	return TRUE;
}

// Es werden nur die folgenden Recordtypen unterstützt.
#define DATA_RECORD			0x00
#define END_RECORD			0x01
#define EXT_LIN_ADDR_RECORD	0x04

//////////////////////////////////////////////////////////////////////
BOOL CCodec::LoadIntelHexfile(const CString &sFilename, BYTE*& pData, DWORD& dwLen)
{
	DWORD	dwAddr			= 0;
	DWORD 	dwNum			= 0;
	DWORD	dwExtLinAddr	= 0;
	DWORD	dwMaxAddr		= 0;
	WORD	wRecordType		= 0;
	BOOL	bResult			= FALSE;
	BOOL	bErr			= FALSE;

	CStdioFile file;
	if (file.Open(sFilename, CFile::modeRead | CFile::shareDenyNone))
	{
		// Suche maximale Adresse
		CString sLine;
		dwMaxAddr = 0;
		while (file.ReadString(sLine) && !bErr)
		{	
			dwNum		= HexToInt(sLine.Mid(1,2));
			dwAddr		= HexToInt(sLine.Mid(3,2))<<8 | HexToInt(sLine.Mid(5,2));
			wRecordType = HexToInt(sLine.Mid(7,2));
			if ((wRecordType != DATA_RECORD) && (wRecordType != END_RECORD) && (wRecordType != EXT_LIN_ADDR_RECORD))
			{
				OnIndicateWarning(TASHA_WARNING_UNSUPPORTED_RECORD_TYPE);
				ML_TRACE(_T("%s // RecordType=%d AddrOffset=%d dwAddr=%d dwNum=%d\n"), sLine, wRecordType, dwExtLinAddr, dwAddr, dwNum, wRecordType);
				ML_TRACE_WARNING(_T("Unsupported RecordType (0x%x) in IntelHEX-file...loading aborded\n"), wRecordType);
				bErr = TRUE;
			}
			if (wRecordType == EXT_LIN_ADDR_RECORD)
				dwExtLinAddr = HexToInt(sLine.Mid(9,2))<<24 | HexToInt(sLine.Mid(11,2))<<16;
			if (wRecordType == DATA_RECORD || wRecordType == EXT_LIN_ADDR_RECORD)
				dwMaxAddr = max(dwMaxAddr, dwExtLinAddr+dwAddr+dwNum);
		}

//		ML_TRACE(_T("Maximal Address=%d\n"), dwMaxAddr);
		if (!bErr)
		{
			BYTE* pOutput = new BYTE[dwMaxAddr+128];
			
			// Image erzeugen
			file.SeekToBegin();
			dwExtLinAddr = 0;
			while (file.ReadString(sLine))
			{	
				dwNum		= HexToInt(sLine.Mid(1,2));
				dwAddr		= HexToInt(sLine.Mid(3,2))<<8 | HexToInt(sLine.Mid(5,2));
				wRecordType = HexToInt(sLine.Mid(7,2));
				
				if (wRecordType == EXT_LIN_ADDR_RECORD)
					dwExtLinAddr = HexToInt(sLine.Mid(9,2))<<24 | HexToInt(sLine.Mid(11,2))<<16;
				
//				ML_TRACE(_T("%s // RecordType=%d AddrOffset=%d dwAddr=%d dwNum=%d\n"), sLine, wRecordType, dwExtLinAddr, dwAddr, dwNum, wRecordType);
				if (wRecordType == DATA_RECORD || wRecordType == EXT_LIN_ADDR_RECORD)
				{				
					for (DWORD dwI = 0; dwI < dwNum; dwI++)
						pOutput[dwExtLinAddr+dwAddr+dwI] = HexToInt(sLine.Mid(9+2*dwI,2));;
				}
			}
			// Pointer auf den Buffer, sowie die maximale Adresse zurück liefern
			pData	= pOutput;
			dwLen	= dwMaxAddr;
			bResult	= TRUE;
		}
	}

	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmReadFromEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr)
{
	if (m_bTraceReadFromEEProm)
		ML_TRACE(_T("HOST: OnConfirmReadFromEEProm Addr=0x%x, dwLen=%d dwStartAddr=0x%x\n"), dwAddr, dwLen, dwStartAddr);

	m_evEEPromDataTransfer.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmTermination()
{
	ML_TRACE(_T("HOST: OnConfirmTermination\n"));

	m_evTmmTermination.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnIndicationVideoStreamAborted(int nChannel)
{
	ML_TRACE(_T("HOST: OnIndicationVideoStreamAborted (Channel=%d)\n"), nChannel);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnIndicationVideoStreamResumed(int nChannel)
{
	ML_TRACE(_T("HOST: OnIndicationVideoStreamResumed (Channel=%d)\n"), nChannel);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnIndicationCameraStatusChanged(int nChannel, BOOL bState)
{
	ML_TRACE(_T("HOST: OnIndicationCameraStatusChanged (Channel=%d, State=%d)\n"), nChannel, bState);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnIndicationAlarmStateChanged(DWORD dwAlarmMask)
{
	ML_TRACE(_T("HOST: OnIndicationAlarmStateChanged (AlarmMask=0x%x)\n"), dwAlarmMask);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnIndicationAdapterSelectStateChanged(DWORD dwAdapterSelectMask)
{
	ML_TRACE(_T("HOST: OnIndicationAdapterSelectStateChanged (AdapterSelectMask=0x%x)\n"), dwAdapterSelectMask);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnIndicationPCKeysStateChanged(DWORD dwPCKeysStateChanged)
{
	ML_TRACE(_T("HOST: OnIndicationPCKeysStateChanged (PCKeysStateChanged=0x%x)\n"), dwPCKeysStateChanged);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnIndicationInformation(int nMessage, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{
	switch (LOWORD(nMessage))
	{
		case INFO_SPORT_TIMING:
			{
				// dwParam1=SPORT0/SPORT1, dwParam2=cycles, dwParam3=SPORTx_COUNT_RX
				LARGE_INTEGER i64Time1;
				i64Time1.HighPart = dwParam1;
				i64Time1.LowPart  = dwParam2;

				LARGE_INTEGER i64Time2;
				i64Time2.HighPart = HIWORD(nMessage);
				i64Time2.LowPart  = dwParam3;

				ML_TRACE(_T(">>>>>>>>>>HOST: Time1: %I64d ys Time2: %I64d ys\n"), (__int64)i64Time1.QuadPart, (__int64)i64Time2.QuadPart);
			}
			break;
		case INFO_UNKNOWN_MESSAGE_ID:
			OnIndicateWarning(TASHA_WARNING_UNKNOWN_MESSAGE_ID);
			ML_TRACE_WARNING(_T("BF535: Unknown MessageID (%d)\n"), (int)dwParam2);
			break;
		case INFO_TOO_MANY_DESCRIPTORS:
			OnIndicateWarning(TASHA_WARNING_TOO_MANY_DESCRIPTORS);
			ML_TRACE_WARNING(_T("BF535: Too many descriptors (%d)\n"), (int)dwParam2);
			break;
		case INFO_EPLD_PROGRAMMING_FAILED:
			OnIndicateError(TASHA_ERROR_TRANSFER_EPLDDATA_FAILED);
			ML_TRACE_ERROR(_T("BF535: EPLD Programming failed\n"));
			break;
		case INFO_WRITE_TO_EEPROM_FAILED:
			OnIndicateError(TASHA_ERROR_TRANSFER_EPLDDATA_FAILED);
			ML_TRACE_ERROR(_T("BF535: Write to EEProm failed\n"));
			break;
		case INFO_READ_FROM_EEPROM_FAILED:
			OnIndicateError(TASHA_ERROR_READ_FROM_EEPROM_FAILED);
			ML_TRACE_ERROR(_T("BF535: Read from EEProm failed\n"));
			break;
		case INFO_TOO_MANY_CHANNELS:
			OnIndicateWarning(TASHA_WARNING_TOO_MANY_CHANNELS);
			ML_TRACE_WARNING(_T("BF535: Too many channels\n"));
			break;	
		case INFO_BOOT533_FAILED:
			OnIndicateError(TASHA_ERROR_BOOT_BF533_FAILED);
			ML_TRACE_ERROR(_T("BF535: Boot BF533 Nr. %d failed\n"), dwParam1);
			break;	
		case INFO_SEND_SPI_COMMAND_TIMEOUT:
			OnIndicateError(TASHA_ERROR_SEND_SPI_COMMAND_FAILED);
			if (m_bTraceSPICommandTimeout)
				ML_TRACE_WARNING(_T("BF535: Send SPI Command %lu to Slave %lu timeout\n"), dwParam1, dwParam2);
			break;
		case INFO_SEND_SPI_COMMAND_PENDING:
			OnIndicateError(TASHA_ERROR_SEND_SPI_COMMAND_PENDING);
			if (m_bTraceSPICommandPending)
				ML_TRACE_WARNING(_T("BF535: Send SPI Command %lu to Slave %lu pending (Pending counter=%lu\n"), dwParam1, dwParam2, dwParam3);
			break;
		case INFO_SLAVE_PF_REQUEST:
			ML_TRACE(_T("BF533: INFO_SLAVE_PF_REQUEST (Slave=%d)\n"), dwParam1);
			break;
		case INFO_PPI_PREAMBEL_ERROR:
			ML_TRACE(_T("BF533: INFO_PPI_PREAMBEL_ERROR (Slave=%d)\n"), dwParam1);
			break;
		case INFO_PPI_PREAMBEL_ERROR_NO_CORR:
			ML_TRACE(_T("BF533: INFO_PPI_PREAMBEL_ERROR_NO_CORR (Slave=%d)\n"), dwParam1);
			break;	
		case INFO_PPI_ERROR:
			OnIndicateError(TASHA_ERROR_BF533_PPI_ERROR);
			ML_TRACE(_T("BF533: INFO_PPI_ERROR (Slave=%d)\n"), dwParam1);
			break;	
		case INFO_SPI_RECEIVE_CHECKSUM_FAILED:
			OnIndicateWarning(TASHA_WARNING_SPI_RECEIVE_CHECKSUM_FAILED);
			if (m_bTraceSPICheckSummError)
				ML_TRACE(_T("BF533: INFO_SPI_RECEIVE_CHECKSUM_FAILED (Slave=%d (H)Checksum=%d != (S)Checksum=%d)\n"), dwParam1, dwParam2, dwParam3);
			break;	
		case INFO_SPI_SEND_CHECKSUM_FAILED:
			OnIndicateWarning(TASHA_WARNING_SPI_SEND_CHECKSUM_FAILED);
			if (m_bTraceSPICheckSummError)
				ML_TRACE(_T("BF533: INFO_SPI_SEND_CHECKSUM_FAILED (Checksum=%d != Checksum=%d)\n"), dwParam1, dwParam2);
			break;	
		case INFO_NO_BF533_RESPONSE:
			OnIndicationNoBF533Response((int)dwParam1);
			break;
		default:
			OnIndicateWarning(TASHA_WARNING_UNKNOWN_INDICATION);
			ML_TRACE_WARNING(_T("BF533: Unknown Message (MessageId=%d)\n"), nMessage);
			break;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnIndicationDebugInformation(DWORD dwAddr, DWORD dwLen)
{
	enum mdsErrorCode Status;
	char szMessage[MAX_DEBUG_STRING_LEN] = {0};
	// Lese Debugmessage
 	Status = mdsBoardReadMemory(m_hDSP,	MDS_DSP_MEM_DATA, dwLen, dwAddr, &szMessage, MDS_DSP_INDEX_A);
	CString sText(szMessage);
	ML_TRACE(_T("%s"), sText);
	return (Status == MDS_ERROR_NO_ERROR);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnIndicationNoBF533Response(int nChannel)
{
	// Timeout zurücksetzen.
	m_dwLastValidDataPacketTC[nChannel] = WK_GetTickCount();

	ML_TRACE_ERROR(_T("BF533: INFO_NO_BF533_RESPONSE!! (Slave=%d)\n"), nChannel);
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnIndicationNoBF535Response()
{
	ML_TRACE_ERROR(_T("OnIndicationNoBF535Response!!\n"));
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::EnableFrameSync(BOOL bFrameSync)
{
	m_bFrameSync = bFrameSync;
}

//////////////////////////////////////////////////////////////////////
BYTE CCodec::HexToInt(const CString &sS)
{
	BYTE byL = 0;
	BYTE byH = 0;

	byH = (BYTE)sS.GetAt(0)-48;
	if (byH > 9)
		byH -= 7;

	byL = (BYTE)sS.GetAt(1)-48;
	if (byL > 9)
		byL -= 7;

	return 16*byH+byL;
}

	/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::LoadConfig(WORD wSource)
{
	BOOL bResult = TRUE;

	_TCHAR szBuffer[SIZE];
	ZeroMemory(szBuffer, sizeof(szBuffer));
	
	// Jede Kamera hat ihre eigenen Einstellungen
	wsprintf(szBuffer, _T("Slaves\\Source%02u"), wSource);
	bResult &= DoRequestSetBrightness(0, wSource, ReadConfigurationInt(szBuffer, _T("Brightness"), 50, m_sAppIniName));
	bResult &= DoRequestSetContrast(0, wSource, ReadConfigurationInt(szBuffer, _T("Contrast"), 50, m_sAppIniName));
	bResult &= DoRequestSetSaturation(0, wSource, ReadConfigurationInt(szBuffer, _T("Saturation"), 50, m_sAppIniName));
	bResult &= DoRequestSetNR(0, wSource, (BOOL)ReadConfigurationInt(szBuffer, _T("NoiseReduction"), 0, m_sAppIniName));

	// Maske aus der Registry laden und an den DSP schicken.
	MASK_STRUCT* pPermanentMask	 = new MASK_STRUCT;
	if (!ReadPermanentMaskFromRegistry(pPermanentMask, wSource))
	{
		pPermanentMask->bValid		= TRUE;
		pPermanentMask->nDimH		= MD_RESOLUTION_H/8;
		pPermanentMask->nDimV		= MD_RESOLUTION_V/8;

		for (int nY=0; nY < pPermanentMask->nDimV; nY++)
		{
			for (int nX=0; nX < pPermanentMask->nDimH; nX++)
				pPermanentMask->byMask[nY][nX] = 0xff;
		}
		SavePermanentMaskToRegistry(pPermanentMask, wSource);
	}
	bResult &= DoRequestSetPermanentMask(0, wSource, *pPermanentMask, FALSE);
	bResult &= TRUE;
	WK_DELETE(pPermanentMask);

	wsprintf(szBuffer, _T("Slaves\\Source%02u\\MD"), wSource);

	//MD-Parameter laden
	bResult &= DoRequestSetMDTreshold(0, wSource,		ReadConfigurationInt(szBuffer, _T("MDTreshold"), 7, m_sAppIniName));
	bResult &= DoRequestSetMaskTreshold(0, wSource,		ReadConfigurationInt(szBuffer, _T("MaskTreshold"), 7, m_sAppIniName));
	bResult &= DoRequestSetMaskIncrement(0, wSource,	ReadConfigurationInt(szBuffer, _T("MaskIncrement"), 6, m_sAppIniName));
	bResult &= DoRequestSetMaskDelay(0, wSource,		ReadConfigurationInt(szBuffer, _T("MaskDelay"), 0, m_sAppIniName));


	CString sSection;
	DWORD   dwUserData = 0;

	// Den Stream0 mit zu letzt verwendeten Daten initialisieren.
	EncoderParam EncParm;

	EncParm.nStreamID			= 1;
	sSection.Format(_T("Slaves\\Source%02u\\EncoderParams\\StreamID%d"), wSource, EncParm.nStreamID);
	EncParm.eRes				= (ImageRes)ReadConfigurationInt(sSection, _T("Resolution"), eImageResMid, m_sAppIniName);
	EncParm.nBitrate			= ReadConfigurationInt(sSection, _T("Bitrate"), 64, m_sAppIniName);
	EncParm.nFps				= ReadConfigurationInt(sSection, _T("Framerate"), 12, m_sAppIniName);
	EncParm.nIFrameIntervall	= ReadConfigurationInt(sSection, _T("I-FrameIntervall"), 6, m_sAppIniName);
	EncParm.eType				= (DataType)ReadConfigurationInt(sSection,  _T("ImageType"), eTypeMpeg4, m_sAppIniName);
	dwUserData					= ReadConfigurationInt(sSection, _T("UserData"), 1, m_sAppIniName);
	bResult &= DoRequestSetEncoderParam(dwUserData, wSource, EncParm);

	EncParm.nStreamID			= 0;
	sSection.Format(_T("Slaves\\Source%02u\\EncoderParams\\StreamID%d"), wSource, EncParm.nStreamID);
	EncParm.eRes				= (ImageRes)ReadConfigurationInt(sSection, _T("Resolution"), eImageResHigh, m_sAppIniName);
	EncParm.nBitrate			= ReadConfigurationInt(sSection, _T("Bitrate"), 4000, m_sAppIniName);
	EncParm.nFps				= ReadConfigurationInt(sSection, _T("Framerate"), 25, m_sAppIniName);
	EncParm.nIFrameIntervall	= ReadConfigurationInt(sSection, _T("I-FrameIntervall"), 5, m_sAppIniName);
	EncParm.eType				= (DataType)ReadConfigurationInt(sSection,  _T("ImageType"), eTypeMpeg4, m_sAppIniName);
	dwUserData					= ReadConfigurationInt(sSection, _T("UserData"), 0, m_sAppIniName);
	bResult &= DoRequestSetEncoderParam(dwUserData, wSource, EncParm);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::ReadDebugConfiguration()
{
	// Informationen der zu protokollierenden Funktionen einlesen
	m_bTraceStartEncoder		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("StartEncoder"),		m_bTraceStartEncoder,		m_sAppIniName);
	m_bTraceStopEncoder			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("StopEncoder"),			m_bTraceStopEncoder,		m_sAppIniName);
	m_bTracePauseEncoder		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("PauseEncoder"),		m_bTracePauseEncoder,		m_sAppIniName);
	m_bTraceResumeEncoder		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("ResumeEncoder"),		m_bTraceResumeEncoder,		m_sAppIniName);	
	m_bTraceSetBrightness		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetBrightness"), 		m_bTraceSetBrightness,		m_sAppIniName);
	m_bTraceSetContrast			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetContrast"), 		m_bTraceSetContrast,		m_sAppIniName);
	m_bTraceSetSaturation		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetSaturation"), 		m_bTraceSetSaturation,		m_sAppIniName);
	m_bTraceSetRelais			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetRelais"), 			m_bTraceSetRelais,			m_sAppIniName);
	m_bTraceSetAlarmEdge		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetAlarmEdge"),		m_bTraceSetAlarmEdge,		m_sAppIniName);
	m_bTraceGetBrightness		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetBrightness"), 		m_bTraceSetBrightness,		m_sAppIniName);
	m_bTraceGetContrast			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetContrast"), 		m_bTraceSetContrast,		m_sAppIniName);
	m_bTraceGetSaturation		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetSaturation"), 		m_bTraceSetSaturation,		m_sAppIniName);
	m_bTraceGetRelais			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetRelais"), 			m_bTraceSetRelais,			m_sAppIniName);
	m_bTraceGetAlarmEdge		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetAlarmEdge"),		m_bTraceSetAlarmEdge,		m_sAppIniName);
	m_bTraceAlarmIndication		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("AlarmIndication"),		m_bTraceAlarmIndication,	m_sAppIniName);
	m_bTraceAnalogOutSwitch		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("AnalogOutSwitch"),		m_bTraceAnalogOutSwitch,	m_sAppIniName);
	m_bTraceSetMDTreshold		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetMDTreshold"),		m_bTraceSetMDTreshold,		m_sAppIniName);
	m_bTraceSetMaskTreshold		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetMaskTreshold"),		m_bTraceSetMaskTreshold,	m_sAppIniName);
	m_bTraceSetMaskIncrement	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetMaskIncrement"),	m_bTraceSetMaskIncrement,	m_sAppIniName);
	m_bTraceSetMaskDelay		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetMaskDelay"),		m_bTraceSetMaskDelay,		m_sAppIniName);
	m_bTraceSetPermanentMask	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetPermanentMask"),	m_bTraceSetPermanentMask,	m_sAppIniName);
	m_bTraceChangePermanentMask	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("ChangePermanentMask"),	m_bTraceChangePermanentMask,m_sAppIniName);
	m_bTraceGetMDTreshold		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetMDTreshold"),		m_bTraceGetMDTreshold,		m_sAppIniName);
	m_bTraceGetMaskTreshold		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetMaskTreshold"),		m_bTraceGetMaskTreshold,	m_sAppIniName);
	m_bTraceGetMaskIncrement	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetMaskIncrement"),	m_bTraceGetMaskIncrement,	m_sAppIniName);
	m_bTraceGetMaskDelay		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetMaskDelay"),		m_bTraceGetMaskDelay,		m_sAppIniName);
	m_bTraceClearPermanentMask	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("ClearPermanentMask"),	m_bTraceClearPermanentMask,	m_sAppIniName);
	m_bTraceInvertPermanentMask	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("InvertPermanentMask"),	m_bTraceInvertPermanentMask,m_sAppIniName);
	m_bTraceSetAnalogOut		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetAnalogOut"),		m_bTraceSetAnalogOut,		m_sAppIniName);
	m_bTraceGetAnalogOut		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetAnalogOut"),		m_bTraceGetAnalogOut,		m_sAppIniName);
	m_bTraceSetTerminationState	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetTerminationState"),	m_bTraceSetTerminationState,m_sAppIniName);
	m_bTraceGetTerminationState	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetTerminationState"),	m_bTraceGetTerminationState,m_sAppIniName);
	m_bTraceSetVideoEnableState	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetVideoEnableState"),	m_bTraceSetVideoEnableState,m_sAppIniName);
	m_bTraceGetVideoEnableState	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetVideoEnableState"),	m_bTraceGetVideoEnableState,m_sAppIniName);
	m_bTraceSetCrosspointEnableState	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetEnableCrosspoint"),	m_bTraceSetCrosspointEnableState,	m_sAppIniName);
	m_bTraceGetCrosspointEnableState	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetEnableCrosspoint"),	m_bTraceGetCrosspointEnableState,	m_sAppIniName);
 	m_bTraceEnableWatchdog		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("EnableWatchdog"),		m_bTraceEnableWatchdog,		m_sAppIniName);
	m_bTraceTriggerWatchdog		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("TriggerWatchdog"),		m_bTraceTriggerWatchdog,	m_sAppIniName);
	m_bTraceSetPowerLED			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetPowerLED"),			m_bTraceSetPowerLED,		m_sAppIniName);
	m_bTraceSetResetLED			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetResetLED"),			m_bTraceSetResetLED,		m_sAppIniName);
	m_bTraceGetVariationRegister= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetVariationRegister"),m_bTraceGetVariationRegister,m_sAppIniName);
	m_bTraceSetEncoderParam		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetEncoderParam"),		m_bTraceSetEncoderParam,	m_sAppIniName);
	m_bTraceGetEncoderParam		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetEncoderParam"),		m_bTraceSetEncoderParam,	m_sAppIniName);
	m_bTraceSPORTCheckSummError	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SPORTCheckSummError"),	m_bTraceSPORTCheckSummError,m_sAppIniName);
	m_bTraceSPICheckSummError	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SPICheckSummError"),	m_bTraceSPICheckSummError,	m_sAppIniName);
	m_bTraceSPICommandTimeout	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SPICommandTimeout"),	m_bTraceSPICommandTimeout,	m_sAppIniName);
	m_bTraceSPICommandPending	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SPICommandPending"),	m_bTraceSPICommandPending,	m_sAppIniName);
	m_bTraceMissingFields		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("MissingFields"),		m_bTraceMissingFields,		m_sAppIniName);
	m_bTraceReadFromEEProm		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("ReadFromEEProm"),		m_bTraceReadFromEEProm,		m_sAppIniName);
	m_bTraceWriteToEEProm		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("WriteToEEProm"),		m_bTraceWriteToEEProm,		m_sAppIniName);
	m_bTraceBF533DebugText		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("BF533DebugText"),		m_bTraceBF533DebugText,		m_sAppIniName);	
	m_bTraceGetDIPState			= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetDIPState"),			m_bTraceGetDIPState,		m_sAppIniName);	
	m_bTraceGetAlarmState		= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetAlarmState"),		m_bTraceGetAlarmState,		m_sAppIniName);	
	m_bTraceRequestSingleFrame	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("RequestSingleFrame"),	m_bTraceRequestSingleFrame,	m_sAppIniName);	
	m_bTraceSetNR				= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("SetNoiseReduction"),	m_bTraceSetNR,				m_sAppIniName);	
	m_bTraceGetNR				= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("GetNoiseReduction"),	m_bTraceGetNR,				m_sAppIniName);	
	
	// Und im Logfile protokollieren
	ML_TRACE(_T("TraceStartEncoder=%s\n"),				(m_bTraceStartEncoder				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceStopEncoder=%s\n"),				(m_bTraceStopEncoder				? _T("On") : _T("Off")));
	ML_TRACE(_T("TracePauseEncoder=%s\n"),				(m_bTracePauseEncoder				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceResumeEncoder=%s\n"),				(m_bTraceResumeEncoder				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetBrighness=%s\n"),				(m_bTraceSetBrightness				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetContrast=%s\n"),				(m_bTraceSetContrast				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetSaturation=%s\n"),				(m_bTraceSetSaturation				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetRelais=%s\n"),					(m_bTraceSetRelais					? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetAlarmEdge=%s\n"),				(m_bTraceSetAlarmEdge				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetBrighness=%s\n"),				(m_bTraceGetBrightness				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetContrast=%s\n"),				(m_bTraceGetContrast				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetSaturation=%s\n"),				(m_bTraceGetSaturation				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetRelais=%s\n"),					(m_bTraceGetRelais					? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetAlarmEdge=%s\n"),				(m_bTraceGetAlarmEdge				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceAlarmIndication=%s\n"),			(m_bTraceAlarmIndication			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceAnalogOutSwitch=%s\n"),			(m_bTraceAnalogOutSwitch			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetMDTreshold=%s\n"),				(m_bTraceSetMDTreshold				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetMaskTreshold=%s\n"),			(m_bTraceSetMaskTreshold			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetMaskIncrement=%s\n"),			(m_bTraceSetMaskIncrement			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetMaskDelay=%s\n"),				(m_bTraceSetMaskDelay				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetPermanentMask=%s\n"),			(m_bTraceSetPermanentMask			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceChangePermanentMask=%s\n"),		(m_bTraceChangePermanentMask		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetMDTreshold=%s\n"),				(m_bTraceGetMDTreshold				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetMaskTreshold=%s\n"),			(m_bTraceGetMaskTreshold			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetMaskIncrement=%s\n"),			(m_bTraceGetMaskIncrement			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetMaskDelay=%s\n"),				(m_bTraceGetMaskDelay				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceClearPermanentMask=%s\n"),		(m_bTraceClearPermanentMask			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceInvertPermanentMask=%s\n"),		(m_bTraceInvertPermanentMask		? _T("On") : _T("Off")));
	ML_TRACE(_T("SetAnalogOut=%s\n"),					(m_bTraceSetAnalogOut				? _T("On") : _T("Off")));
	ML_TRACE(_T("GetAnalogOut=%s\n"),					(m_bTraceGetAnalogOut				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetTerminationState=%s\n"),		(m_bTraceSetTerminationState		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetTerminationState=%s\n"),		(m_bTraceGetTerminationState		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetVideoEnableState=%s\n"),		(m_bTraceSetVideoEnableState		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetVideoEnableState=%s\n"),		(m_bTraceGetVideoEnableState		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetCrosspointEnableState=%s\n"),	(m_bTraceSetCrosspointEnableState	? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetCrosspointEnableState=%s\n"),	(m_bTraceGetCrosspointEnableState	? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceEnableWatchdog=%s\n"),			(m_bTraceEnableWatchdog				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceTriggerWatchdog=%s\n"),			(m_bTraceTriggerWatchdog			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetPowerLED=%s\n"),				(m_bTraceSetPowerLED				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetResetLED=%s\n"),				(m_bTraceSetResetLED				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetVariationRegister=%s\n"),		(m_bTraceGetVariationRegister		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetEncoderParam=%s\n"),			(m_bTraceSetEncoderParam			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetEncoderParam=%s\n"),			(m_bTraceGetEncoderParam			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSPORTCheckSummError=%s\n"),		(m_bTraceSPORTCheckSummError		? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSPICheckSummError=%s\n"),			(m_bTraceSPICheckSummError			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSPICommandTimeout=%s\n"),			(m_bTraceSPICommandTimeout			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSPICommandPending=%s\n"),			(m_bTraceSPICommandPending			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceMissingFields=%s\n"),				(m_bTraceMissingFields				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceReadFromEEProm=%s\n"),			(m_bTraceReadFromEEProm				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceWriteToEEProm=%s\n"),				(m_bTraceWriteToEEProm				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceBF533DebugText=%s\n"),			(m_bTraceBF533DebugText				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceDIBState=%s\n"),					(m_bTraceGetDIPState				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetAlarmState=%s\n"),				(m_bTraceGetAlarmState				? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceRequestSingleFrame=%s\n"),		(m_bTraceRequestSingleFrame			? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceSetNoiseReductione=%s\n"),		(m_bTraceSetNR						? _T("On") : _T("Off")));
	ML_TRACE(_T("TraceGetNoiseReductione=%s\n"),		(m_bTraceGetNR						? _T("On") : _T("Off")));
}

/////////////////////////////////////////////////////////////////////////////
// Prüft, ob ein BootImage für das EEProm vorhanden ist, und überträgt dieses in das EEProm
BOOL CCodec::CheckForEEPromUpdate(const CString& sImageName)
{
	BOOL bResult = FALSE;

	if (DoesFileExist(sImageName))
	{
		if (WriteEEPromData(sImageName))
		{
			ML_TRACE(_T("Writing new bootimage into EEProm\n"));
			try
			{
				CFile::Remove(sImageName);
				bResult = TRUE;
			}
			catch(...)
			{
				ML_TRACE_ERROR(_T("CCodec::CheckForEEPromUpdate exeption(%s)\n"), sImageName);
			}
		}
		else
			ML_TRACE_ERROR(_T("CCodec::CheckForEEPromUpdate WriteEEPromData failed (%s)\n"), sImageName);
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::ReadDongleInformation()
{
	// Defaultmäßig sind Netz und ISDN und Backup sperrren
	WriteConfigurationInt(_T("EEProm"), _T("ISDN"), 0, m_sAppIniName);
	WriteConfigurationInt(_T("EEProm"), _T("NET"), 0, m_sAppIniName);
	WriteConfigurationInt(_T("EEProm"), _T("BackUp"), 0, m_sAppIniName);
	WriteConfigurationInt(_T("EEProm"), _T("BackUpToDVD"), 0, m_sAppIniName);
	WriteConfigurationString(_T("EEProm"), _T("SN"), _T(""), m_sAppIniName);

	// Objekt zum Zugriff auf das EEProm
	CEEProm EEProm(this);

	WORD  wDongle	= 0;
	DWORD dwHeader	= 0;
	CString sNewSN;
	CString sCurSN;


	// Prüft, ob ein BootImage für das EEProm vorhanden ist, und überträgt dieses in das EEProm
//	CheckForEEPromUpdate(_T(GetHomeDir()+BOOT_IMAGE_535));
	CheckForEEPromUpdate(GetHomeDir()+BOOT_IMAGE_535);

	if (EEProm.ReadMagicHeader(dwHeader))
	{
		// Eine evtl. vorhandene neue Seriennummer übertragen.
		if (ReadConfigurationInt(_T("EEProm"), _T("ChangeSN"), 0, m_sAppIniName))
		{
			if (EEProm.ReadSN(sCurSN))
			{
				BOOL bResult = FALSE;
				
				// Wenn ein Template für die SN vorhanden ist, dann nehme diese
				TCHAR  szBuffer[20] ={0};
				ReadConfigurationString(_T("EEProm"), _T("SNTemplate"), _T("12345678901234567\0"), szBuffer, 18, m_sAppIniName);
				CString sSNTemplate(szBuffer);
				sSNTemplate.ReleaseBuffer();
				CSerialNumberDlg dlg(NULL, sCurSN.Trim(), sSNTemplate.Trim());
				if (dlg.DoModal() == IDOK)
				{
					sNewSN = dlg.GetNewSN();
					EEProm.ClearEEProm();				
					EEProm.WriteSN(sNewSN);
					EEProm.WriteSubsystemID(TASHA_DEVICE_ID, TASHA_VENDOR_ID);
					CString sCheckedSN;
					if (EEProm.ReadSN(sCheckedSN))
					{
						if (sCheckedSN.Trim() == sNewSN)
							bResult = TRUE;											
					}
					if (bResult)
					{
						EEProm.ReadMagicHeader(dwHeader); // Header erneut lesen.
						AfxMessageBox(_T("Die Seriennummer wurde erfolgreich geändert!"));
					}
					else
					{
						Beep(500, 500);
						Beep(250, 500);
						AfxMessageBox(_T("FEHLER: Die Seriennummer konnte nicht geändert werden!"));
					}
				}
			}
		}

		if (dwHeader == MAGIC_HEADER)
		{
			// Wenn Dongleinformationen in Registryeintrag _T("Raw") stehen, diese in
			// das EEProm übertragen und den Eintrag löschen.
			int nRaw = ReadConfigurationInt(_T("EEProm"), _T("RawBits"), -1, m_sAppIniName);
			DeleteEntry(_T("EEProm"), _T("RawBits"), m_sAppIniName);
			if (nRaw != -1)
			{
				// Wenn nRawBits=0 ist, sollen die Dongleinformationen im EProm gelöscht werden.
				if (nRaw == 0)
					EEProm.WriteDongleInformation(0);
				else
				{
					// Ansonsten die Dongleinformationen verodern
					if (EEProm.ReadDongleInformation(wDongle))
						EEProm.WriteDongleInformation(wDongle | (WORD)nRaw);
				}
			}

			// Dongleinformationen aus EEProm lesen und in die Registry übertragen.
			if (EEProm.ReadDongleInformation(wDongle))
			{
				if (TSTBIT(wDongle, EE_DONGLE_BIT_ISDN))
				{
					ML_TRACE(_T("Tasha Dongleinformation: ISDN allowed\n"));
					WriteConfigurationInt(_T("EEProm"), _T("ISDN"), 1, m_sAppIniName);
				}
				else
				{
					ML_TRACE(_T("Tasha Dongleinformation: ISDN not allowed\n"));
					WriteConfigurationInt(_T("EEProm"), _T("ISDN"), 0, m_sAppIniName);
				}
				if (TSTBIT(wDongle, EE_DONGLE_BIT_TCP))
				{
					ML_TRACE(_T("Tasha Dongleinformation: TCP/IP allowed\n"));
					WriteConfigurationInt(_T("EEProm"), _T("NET"), 1, m_sAppIniName);
				}
				else
				{
					ML_TRACE(_T("Tasha Dongleinformation: TCP/IP not allowed\n"));
					WriteConfigurationInt(_T("EEProm"), _T("NET"), 0, m_sAppIniName);
				}
				if (TSTBIT(wDongle, EE_DONGLE_BIT_BACKUP))
				{
					ML_TRACE(_T("Tasha Dongleinformation: Backup allowed\n"));
					WriteConfigurationInt(_T("EEProm"), _T("BackUp"), 1, m_sAppIniName);
				}
				else
				{
					ML_TRACE(_T("Tasha Dongleinformation: Backup not allowed\n"));
					WriteConfigurationInt(_T("EEProm"), _T("BackUp"), 0, m_sAppIniName);
				}
				if (TSTBIT(wDongle, EE_DONGLE_BIT_BACKUP_DVD))
				{
					ML_TRACE(_T("Tasha Dongleinformation: Backup to DVD allowed\n"));
					WriteConfigurationInt(_T("EEProm"), _T("BackUpToDVD"), 1, m_sAppIniName);
				}
				else
				{
					ML_TRACE(_T("Tasha Dongleinformation: Backup to DVD not allowed\n"));
					WriteConfigurationInt(_T("EEProm"), _T("BackUpToDVD"), 0, m_sAppIniName);
				}
			}
			// Seriennummer in die Registry übertragen.
			if (EEProm.ReadSN(sCurSN))
			{
				ML_TRACE(_T("Tasha SN=<%s>\n"), sCurSN.Trim());
				WriteConfigurationString(_T("EEProm"), _T("SN"), sCurSN.Trim(), m_sAppIniName);
			}
		}
		else // Kein Magic Header vorhanden.
		{
			EEProm.ClearEEProm();				
			EEProm.WriteSN(_T("12345678901234567"));
			EEProm.WriteSubsystemID(TASHA_DEVICE_ID, TASHA_VENDOR_ID);
		}
		EEProm.DumpEEProm();
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::ForceNewIFrame(WORD wSource, WORD wStreamID)
{
	// Force a I-Frame
//	Beep(1000, 100);
//	ML_TRACE(_T("################### Force I-Frame for stream 1 #############################\n"));
	CString sSection; 
	sSection.Format(_T("Slaves\\Source%02u\\EncoderParams\\StreamID%d"), wSource, wStreamID);
	
	EncoderParam EncParm;
	EncParm.nStreamID			= wStreamID;
	EncParm.eRes				= (ImageRes)ReadConfigurationInt(sSection, _T("Resolution"), eImageResMid, m_sAppIniName);
	EncParm.nBitrate			= ReadConfigurationInt(sSection, _T("Bitrate"), 64, m_sAppIniName);
	EncParm.nFps				= ReadConfigurationInt(sSection, _T("Framerate"), 12, m_sAppIniName);
	EncParm.nIFrameIntervall	= ReadConfigurationInt(sSection, _T("I-FrameIntervall"), 6, m_sAppIniName);
	EncParm.eType				= (DataType)ReadConfigurationInt(sSection,  _T("ImageType"), eTypeMpeg4, m_sAppIniName);
	DWORD dwUserData			= ReadConfigurationInt(sSection, _T("UserData"), 1, m_sAppIniName);
	DoRequestSetEncoderParam(dwUserData, wSource, EncParm);
	DoRequestNewSingleFrame(dwUserData, wSource);
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::OnIndicateError(int nErrorCode)
{
	// Should be overridden
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::OnIndicateWarning(int nWarningCode)
{
	// Should be overridden
}

/////////////////////////////////////////////////////////////////////////////
CString CCodec::GetHomeDir()
{
	CString sHomePath = _T("");
#ifndef _DEBUG
	_TCHAR szPath[_MAX_PATH];
	HINSTANCE hInstance = AfxGetInstanceHandle( );
	if (hInstance)
	{
		GetModuleFileName(hInstance, szPath, sizeof(szPath));
		sHomePath = szPath;
		int nI = sHomePath.ReverseFind(_T('\\'));
		if (nI != -1)
			sHomePath = sHomePath.Left(nI+1);
	}
#endif
	return sHomePath;
}
