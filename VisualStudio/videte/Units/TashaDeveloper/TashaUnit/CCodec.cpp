// CCodec.cpp: implementation of the CCodec class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TashaUnit.h"
#include "CCodec.h"
#include "TashaUnitDlg.h"
#include "CircularBuffer.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CCriticalSection CCodec::m_csMessageHandling;

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
CCodec::CCodec(CTashaUnitDlg* pWnd, int nDSPNumber, const CString &sAppIniName)
{
	m_nDSPNumber				= nDSPNumber;
	m_hDSP						= NULL;
	m_pWnd						= pWnd;
	
	m_eEncoderState				= eEncoderStateUnvalid;
	m_bEstablishMessageChannel	= FALSE;
	m_bFrameSync				= TRUE;
	m_bRun						= TRUE;
	m_dwCamMask					= 0; 			 

	m_sAppIniName				= sAppIniName;

	// Tracemeldungen (An=TRUE / Aus=FALSE)
	m_bTraceStartEncoder		= TRUE;
	m_bTraceStopEncoder			= TRUE;
	m_bTracePauseEncoder		= TRUE;
	m_bTraceResumeEncoder		= TRUE;
	m_bTraceSetBrightness		= TRUE;
	m_bTraceSetContrast			= TRUE;
	m_bTraceSetSaturation		= TRUE;
	m_bTraceSetRelais			= TRUE;
	m_bTraceSetAlarmEdge		= TRUE;
	m_bTraceGetBrightness		= TRUE;
	m_bTraceGetContrast			= TRUE;
	m_bTraceGetSaturation		= TRUE;
	m_bTraceGetRelais			= TRUE;
	m_bTraceGetAlarmEdge		= TRUE;
	m_bTraceAlarmIndication		= TRUE;
	m_bTraceAnalogOutSwitch		= TRUE;
	m_bTraceSetMDTreshold		= TRUE;
	m_bTraceSetMaskTreshold		= TRUE;
	m_bTraceSetMaskIncrement	= TRUE;
	m_bTraceSetMaskDelay		= TRUE;
	m_bTraceChangePermanentMask	= TRUE;
	m_bTraceSetPermanentMask	= TRUE;
	m_bTraceGetMDTreshold		= TRUE;
	m_bTraceGetMaskTreshold		= TRUE;
	m_bTraceGetMaskIncrement	= TRUE;
	m_bTraceGetMaskDelay		= TRUE;
	m_bTraceClearPermanentMask	= TRUE;
	m_bTraceInvertPermanentMask	= TRUE;


	// Defaultmäßig sind Netz und ISDN und Backup sperrren
	WriteConfigurationInt(_T("EEProm"), _T("ISDN"), 0, m_sAppIniName);
	WriteConfigurationInt(_T("EEProm"), _T("NET"), 0, m_sAppIniName);
	WriteConfigurationInt(_T("EEProm"), _T("BackUp"), 0, m_sAppIniName);
	WriteConfigurationInt(_T("EEProm"), _T("BackUpToDVD"), 0, m_sAppIniName);
	WriteConfigurationString(_T("EEProm"), _T("SN"), _T(""), m_sAppIniName);

	m_pPerf1 = new CWK_PerfMon(_T("CommunicationDelay"));
	m_pPerf2 = new CWK_PerfMon(_T("AcknoledgeDelay"));
	m_pPerf3 = new CWK_PerfMon(_T("Capturetime"));

	// Buffer enthält die Bilddaten aller 8 Channels
	m_pFrameBuffer = (BYTE*) new BYTE[CHANNEL_SIZE*CHANNEL_COUNT/2+1024];

	// Jeder Kanal erhält zur Framesynchronisation einen Circularbuffer
	for (int nChannel = 0; nChannel < CHANNEL_COUNT; nChannel++)
	{
		m_pCirBuff[nChannel] = new CCircularBuffer(nChannel, MAGIC_MARKER_START, MAGIC_MARKER_END, 2*1024*1024);
		m_nVidPres[nChannel] = 0;
	}

	// Diese Semaphore teilt der 'ReadDataThread' mit, daß Bilddaten abgeholt werden können
	m_hFieldReadySemaphore = CreateSemaphore(NULL, 0, 100, NULL);

	if (!m_hFieldReadySemaphore)
		WK_TRACE_ERROR("Create FieldReadySemaphore failed\n");

	// Dieser Thread holt die Daten aus den Circularbuffern und reicht sie weiter.
	m_pReadDataThread = AfxBeginThread(ReadDataThread, this);
	if (m_pReadDataThread)
	{
		m_pReadDataThread->m_bAutoDelete = FALSE;
		m_pReadDataThread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	}
}

//////////////////////////////////////////////////////////////////////
CCodec::~CCodec()
{	
	m_bRun = FALSE;

    // Warte bis 'ReadDataThread' beendet ist.
	if (m_pReadDataThread)
		WaitForSingleObject(m_pReadDataThread->m_hThread, 2000);
	WK_DELETE(m_pReadDataThread); //Autodelete = FALSE;

	// Framebuffer freigeben
	WK_DELETE(m_pFrameBuffer);

	// Circularbuffer freigeben
	for (int nChannel = 0; nChannel < CHANNEL_COUNT; nChannel++)
		WK_DELETE(m_pCirBuff[nChannel]);

	Close();

	WK_DELETE(m_pPerf1);
	WK_DELETE(m_pPerf2);
	WK_DELETE(m_pPerf3);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::Open(const CString& sImagePath)
{
	enum mdsErrorCode Status;
	int nDllVersion		= 0;
	int	nDriverVersion	= 0;
	int nFirmwareVersion= 0;
	BOOL bResult		= FALSE;

	// Adresse des 8MByte großen DMA-Buffers holen.
	mem_FrameBuffer.dwLen	= 16*CHANNEL_COUNT*CHANNEL_SIZE+1024;
	if (!HACCGetFramebuffer(mem_FrameBuffer))
	{
		AfxMessageBox("Konnte Framebufferadresse nicht ermitteln\n", MB_ICONERROR);
		return FALSE;
	}

	// 1MByte großen Transferbuffer (Altera, EEProm, BF533-Bootimage) anlegen
	mem_TransferBuffer.dwLen		= 1024*1024;
	mem_TransferBuffer.pLinAddr		= NULL;
	mem_TransferBuffer.pPhysAddr	= NULL;
	if (!HACCAllocFramebuffer(mem_TransferBuffer))
	{
		AfxMessageBox("Konnte Transferbuffer nicht anlegen\n", MB_ICONERROR);
		return FALSE;
	}

	// MDS-Treiber öffnen
	Status = mdsBoardOpen(MDS_BOARD_HAWK35, &m_nDSPNumber, 0, 0, &m_hDSP);

	if (Status == MDS_ERROR_NO_ERROR)
	{
		// Get DLL version
		Status = mdsBoardGetLibraryVersion(&nDllVersion);
  
		// Check return value
		if (Status == MDS_ERROR_NO_ERROR)
		{
			// Get driver version
			Status = mdsBoardGetDriverVersion(m_hDSP, &nDriverVersion);

			// Check return value
			if (Status == MDS_ERROR_NO_ERROR)
			{
				// Get firmware version
			//	Status = mdsBoardGetFirmwareVersion(m_hDSP,	&nFirmwareVersion, MDS_DSP_INDEX_A);	

				// Check return value
				if (Status == MDS_ERROR_NO_ERROR)
				{ 
					//Informational Message
					WK_TRACE(_T("TashaUnit detected\n"));
					CString sText;
					sText.Format("DLL Version: major %d minor %d\n", (nDllVersion & 0x0000FF00) >> 8,
																	 (nDllVersion & 0x03FF0000) >> 16);
					WK_TRACE(_T("%s\n"), sText);

					sText.Format("Driver Version: major %d minor %d build %d\n", (nDriverVersion & 0x0000FF00) >> 8,
																				 (nDriverVersion & 0x000000FF),
																				 (nDriverVersion & 0x03FF0000) >> 16);
					WK_TRACE(_T("%s\n"), sText);

 					sText.Format("Firmware Version: major %d minor %d wee %d\n", (nFirmwareVersion & 0x00ff0000) >> 16,
																				 (nFirmwareVersion & 0x0000FF00) >> 8,
																				 (nFirmwareVersion & 0x000000ff));
					WK_TRACE(_T("%s\n"), sText);

					// Download program to DSP
					CString sImage(sImagePath);
					Status = mdsBoardDownLoad(m_hDSP, sImage.GetBuffer(MAX_PATH), MDS_DSP_INDEX_A);
					sImage.ReleaseBuffer(-1);
					
					// Check return value
					if (Status == MDS_ERROR_NO_ERROR)
					{ 
						WK_TRACE("Downloading Firmware <%s> to Tasha complete\n", sImagePath);
						if (OpenMessageChannel())
						{				
							m_evTmmInitComplete.ResetEvent();
							if (WaitForSingleObject(m_evTmmInitComplete, 5000) == WAIT_TIMEOUT)
								WK_TRACE_WARNING("HOST: TmmInitComplete timeout\n");
							else
							{
								// Alterafile laden
								if (TransferEPLDdata(EPLD_FILENAME))
								{
									// Alle BF533 nacheinander booten
									BOOL bErr = FALSE;
									CString sFileName;

									for (int nChannel = 0; nChannel < CHANNEL_COUNT; nChannel++)
									{
										sFileName.Format(BOOT_IMAGE_533, nChannel);
										if (!TransferBF533BootData(nChannel, sFileName))
											bErr = TRUE;
									}
							
								//	if (!bErr)
										bResult = DoRequestSetPCIFrameBufferAddress((DWORD)mem_FrameBuffer.pPhysAddr);
								}
							}
						}
						else
							WK_TRACE_ERROR("HOST: Can't open Message channel\n");
					}
					else
					{
						//Give more specific error information
						switch(Status)
						{		
							case MDS_ERROR_FILE_ACCESS :
								WK_TRACE_ERROR(_T("Failed to open %s for download.\n"), sImagePath);
								mdsBoardClose(m_hDSP);
								m_hDSP = NULL;
								break;
							default :
								WK_TRACE_ERROR(_T("Failed to download %s to Tasha.\n"), sImagePath);
								mdsBoardClose(m_hDSP);
								m_hDSP = NULL ;
							break;
						}
					}				
				}
				else
				{
					WK_TRACE_ERROR(_T("Failed getting firmware version.\n"));
					WK_TRACE_ERROR(_T("Aborting interrupt test.\n"));
					mdsBoardClose(m_hDSP);
					m_hDSP = NULL;
				}							
			}
			else
			{	
				WK_TRACE_ERROR(_T("Failed getting driver version.\n"));
				mdsBoardClose(m_hDSP);
				m_hDSP = NULL;
			}
		}
		else
		{ 
			WK_TRACE_ERROR(_T("CCodec::CCodec Failed getting DLL version (%d)\n"), Status);
			mdsBoardClose(m_hDSP);
			m_hDSP = NULL;
		}
	}
	else
	{
		switch(Status)
		{
			case MDS_ERROR_BOARD_IN_USE :
				WK_TRACE_ERROR(_T("Failed to open Tasha board. Board is in use by another application.\n"));
				break;     
			case MDS_ERROR_BOARD_NOT_PRESENT :
				WK_TRACE_ERROR(_T("Failed to open Tasha board. Board is not present.\n"));
				WK_TRACE_ERROR(_T("Board is not present.\n"));
				break;      
			default :
				WK_TRACE_ERROR(_T("Failed to open Tasha board.\n"));
			return 1;
		}
	}

	return bResult;
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
				WK_TRACE("HOST: Target terminated\n");
			else
				WK_TRACE_WARNING("HOST: WaitForSingleObject (ExitEvent) timeout\n");
	
			CloseMessageChannel();
		}
		mdsBoardClose(m_hDSP);
		m_hDSP = NULL;
	}

	if (mem_TransferBuffer.pLinAddr)
		HACCFreeFramebuffer(mem_TransferBuffer);
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
			WK_TRACE(_T("Open MessageChannel success\n"));
			bResult = TRUE;
			m_bEstablishMessageChannel = TRUE;
		}
		else
			WK_TRACE_ERROR(_T("Unable to set interrupt callback for Tasha.\n"));
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
			WK_TRACE_ERROR(_T("Unable to clear interrupt callback for Hawk35.\n"));
	}
	
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetChannel(WORD wSource, WORD wNewSource)
{
	WK_TRACE("HOST: DoRequestSetChannel (Source=%d, NewChannel=%d)\n", wSource, wNewSource);
	return SendMessage(TMM_REQUEST_SET_CHANNEL, wSource, wNewSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetChannel(WORD wSource, WORD wNewSource)
{
	WK_TRACE("HOST: OnConfirmSetChannel (Source=%d, NewChannel=%d)\n", wSource, wNewSource);
	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetChannel(wSource, wNewSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetBrightness)
		WK_TRACE("HOST: DoRequestSetBrightness (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);
	
	// Bug im Frontend
	if ((nValue >=60) && (nValue <=63))
		return FALSE;

	return SendMessage(TMM_REQUEST_SET_BRIGHTNESS, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetBrightness)
		WK_TRACE("HOST: OnConfirmSetBrightness (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);
	
	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u"),wSource);
	WriteConfigurationInt(sBuffer, _T("Brightness"), nValue, m_sAppIniName);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetBrightness(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetContrast)
		WK_TRACE("HOST: DoRequestSetContrast (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);
	return SendMessage(TMM_REQUEST_SET_CONTRAST, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetContrast)
		WK_TRACE("HOST: OnConfirmSetContrast (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u"),wSource);
	WriteConfigurationInt(sBuffer, _T("Contrast"), nValue, m_sAppIniName);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetContrast(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetSaturation)
		WK_TRACE("HOST: DoRequestSetSaturation (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	return SendMessage(TMM_REQUEST_SET_SATURATION, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetSaturation)
		WK_TRACE("HOST: OnConfirmSetSaturation (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u"),wSource);
	WriteConfigurationInt(sBuffer, _T("Saturation"), nValue, m_sAppIniName);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetSaturation(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetBrightness(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetBrightness)
		WK_TRACE("HOST: DoRequestGetBrightness (UserData=0x%x Source=%d)\n", dwUserData, wSource);
	return SendMessage(TMM_REQUEST_GET_BRIGHTNESS, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceGetBrightness)
		WK_TRACE("HOST: OnConfirmGetBrightness (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetBrightness(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetContrast(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetContrast)
		WK_TRACE("HOST: DoRequestGetContrast (UserData=0x%x Source=%d)\n", dwUserData, wSource);
	return SendMessage(TMM_REQUEST_GET_CONTRAST, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceGetContrast)
		WK_TRACE("HOST: OnConfirmGetContrast (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetContrast(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetSaturation(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetSaturation)
		WK_TRACE("HOST: DoRequestGetSaturation (UserData=0x%x Source=%d)\n", dwUserData, wSource);
	return SendMessage(TMM_REQUEST_GET_SATURATION, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceGetSaturation)
		WK_TRACE("HOST: OnConfirmGetSaturation (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetSaturation(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMDTreshold)
		WK_TRACE("HOST: DoRequestSetMDTreshold (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	return SendMessage(TMM_REQUEST_SET_THRESHOLD_MD, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMDTreshold)
		WK_TRACE("HOST: OnConfirmSetMDTreshold (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u\\MD"),wSource);
	WriteConfigurationInt(sBuffer, _T("MDTreshold"), nValue, m_sAppIniName);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetMDTreshold(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMaskTreshold)
		WK_TRACE("HOST: DoRequestSetMaskTreshold (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);
	
	return SendMessage(TMM_REQUEST_SET_THRESHOLD_MASK, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMaskTreshold)
		WK_TRACE("HOST: OnConfirmSetMaskTreshold (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u\\MD"),wSource);
	WriteConfigurationInt(sBuffer, _T("MaskTreshold"), nValue, m_sAppIniName);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetMaskTreshold(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMaskIncrement)
		WK_TRACE("HOST: DoRequestSetMaskIncrement (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	return SendMessage(TMM_REQUEST_SET_INCREMENT_MASK, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMaskIncrement)
		WK_TRACE("HOST: OnConfirmSetMaskIncrement (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u\\MD"),wSource);
	WriteConfigurationInt(sBuffer, _T("MaskIncrement"), nValue, m_sAppIniName);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetMaskIncrement(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceSetMaskDelay)
		WK_TRACE("HOST: DoRequestSetMaskDelay (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	m_pPerf1->Start();				

	return SendMessage(TMM_REQUEST_SET_DELAY_MASK, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue)
{
	m_pPerf1->Stop();
	
	if (m_bTraceSetMaskDelay)
		WK_TRACE("HOST: OnConfirmSetMaskDelay (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u\\MD"),wSource);
	WriteConfigurationInt(sBuffer, _T("MaskDelay"), nValue, m_sAppIniName);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetMaskDelay(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestChangePermanentMask(DWORD dwUserData, WORD wSource, int nX, int nY, int nValue)
{
	if (m_bTraceChangePermanentMask)
		WK_TRACE("HOST: DoRequestChangePermanentMask (UserData=0x%x Source=%d nX=%d, nY=%d Value=%d)\n", dwUserData, wSource, nX, nY, nValue);

	return SendMessage(TMM_REQUEST_CHANGE_PERMANENT_MASK, dwUserData, wSource, MAKELONG(nY, nX), nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmChangePermanentMask(DWORD dwUserData, WORD wSource, int nX, int nY, int nValue)
{
	BOOL bResult = FALSE;

	if (m_bTraceChangePermanentMask)
		WK_TRACE("HOST: OnConfirmChangePermanentMask (UserData=0x%x Source=%d nX=%d, nY=%d Value=%d)\n", dwUserData, wSource, nX, nY, nValue);

	MASK_STRUCT* pPermanentMask	 = new MASK_STRUCT;

	if (ReadPermanentMaskFromRegistry(pPermanentMask, wSource))
	{
		pPermanentMask->byMask[nY][nX] = nValue;
		bResult = SavePermanentMaskToRegistry(pPermanentMask, wSource);
	}
	WK_DELETE(pPermanentMask);
								    
	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmChangePermanentMask(dwUserData, wSource, nX, nY, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetMDTreshold(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetMDTreshold)
		WK_TRACE("HOST: DoRequestGetMDTreshold (UserData=0x%x Source=%d)\n", dwUserData, wSource);
	
	return SendMessage(TMM_REQUEST_GET_THRESHOLD_MD, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetMDTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceGetMDTreshold)
		WK_TRACE("HOST: OnConfirmGetMDTreshold (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetMDTreshold(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetMaskTreshold(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetMaskTreshold)
		WK_TRACE("HOST: DoRequestGetMaskTreshold (UserData=0x%x Source=%d)\n", dwUserData, wSource);
	
	return SendMessage(TMM_REQUEST_GET_THRESHOLD_MASK, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceGetMaskTreshold)
		WK_TRACE("HOST: OnConfirmGetMaskTreshold (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetMaskTreshold(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetMaskIncrement(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetMaskIncrement)
		WK_TRACE("HOST: DoRequestGetMaskIncrement (UserData=0x%x Source=%d)\n", dwUserData, wSource);
	return SendMessage(TMM_REQUEST_GET_INCREMENT_MASK, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceGetMaskIncrement)
		WK_TRACE("HOST: OnConfirmGetMaskIncrement (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetMaskIncrement(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetMaskDelay(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceGetMaskDelay)
		WK_TRACE("HOST: DoRequestGetMaskDelay (UserData=0x%x Source=%d)\n", dwUserData, wSource);
	
	return SendMessage(TMM_REQUEST_GET_DELAY_MASK, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetMaskDelay(DWORD dwUserData, WORD wSource, int nValue)
{
	if (m_bTraceGetMaskDelay)
		WK_TRACE("HOST: OnConfirmGetMaskDelay (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetMaskDelay(dwUserData, wSource, nValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestStartCapture() 
{
	if (m_bTraceStartEncoder)
		WK_TRACE("HOST: DoRequestStartCapture\n");
	return SendMessage(TMM_REQUEST_START_CAPTURE, 0, 0);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmStartCapture() 
{
	BOOL bResult = FALSE;

	if (m_bTraceStartEncoder)
		WK_TRACE("HOST: OnConfirmStartCapture\n");

	if (CanStart())
	{
		if (m_pWnd)
		{
			m_eEncoderState = eEncoderStateOn;
			if (m_pWnd->OnConfirmStartCapture())
				bResult = TRUE;
		}
			
	}
	else
		WK_TRACE_WARNING("Can't start capture because wrong state (State=%d)\n", m_eEncoderState);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestStopCapture() 
{
	if (m_bTraceStopEncoder)
		WK_TRACE("HOST: DoRequestStopCapture\n");
	
	return SendMessage(TMM_REQUEST_STOP_CAPTURE, 0, 0);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmStopCapture() 
{
	BOOL bResult = FALSE;

	if (m_bTraceStopEncoder)
		WK_TRACE("HOST: OnConfirmStopCapture\n");

	if (CanStop())
	{
		if (m_pWnd)
		{
			m_eEncoderState = eEncoderStateOff;
			if (m_pWnd->OnConfirmStopCapture())
				bResult = TRUE;
		}
			
	}
	else
		WK_TRACE_WARNING("Can't stop capture because wrong state (State=%d)\n", m_eEncoderState);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestPauseCapture() 
{
	if (m_bTracePauseEncoder)
		WK_TRACE("HOST: DoRequestPauseCapture\n");
	m_pPerf1->ShowResults();
	m_pPerf2->ShowResults();
	m_pPerf3->ShowResults();

	return SendMessage(TMM_REQUEST_PAUSE_CAPTURE, 0, 0);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmPauseCapture() 
{
	BOOL bResult = FALSE;

	if (m_bTracePauseEncoder)
		WK_TRACE("HOST: OnConfirmPauseCapture\n");
	
	if (CanPause())
	{
		if (m_pWnd)
		{
			m_eEncoderState = eEncoderStatePause;
			if (m_pWnd->OnConfirmPauseCapture())
				bResult = TRUE;
		}
	}
	else
		WK_TRACE_WARNING("Can't pause capture because wrong state (State=%d)\n", m_eEncoderState);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestResumeCapture() 
{
	if (m_bTraceResumeEncoder)
		WK_TRACE("HOST: DoRequestResumeCapture\n");
	return SendMessage(TMM_REQUEST_RESUME_CAPTURE, 0, 0);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmResumeCapture()
{
	BOOL bResult = FALSE;

	if (m_bTraceResumeEncoder)
		WK_TRACE("HOST: OnConfirmResumeCapture\n");
	
	if (CanResume())
	{
		if (m_pWnd)
		{
			m_eEncoderState = eEncoderStateOn;
			if (m_pWnd->OnConfirmResumeCapture())
				bResult = TRUE;
		}
	}
	else
		WK_TRACE_WARNING("Can't resume capture because wrong state (State=%d)\n", m_eEncoderState);

	return bResult;
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

//////////////////////////////////////////////////////////////////////
BOOL CCodec::SavePermanentMaskToRegistry(MASK_STRUCT* pPermanentMask, WORD wSource)
{
	CString sBuffer;
	sBuffer.Format(_T("Slaves\\Source%02u\\MD"),wSource);

	return WriteConfigurationBinary(sBuffer, _T("PermanentMask"), (BYTE*)pPermanentMask, sizeof(MASK_STRUCT), m_sAppIniName);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::SendMessage(int nMessageID, DWORD dwParam1/*=0*/, DWORD dwParam2/*=0*/, DWORD dwParam3/*=0*/,DWORD dwParam4/*=0*/)
{
	BOOL bResult = FALSE;

	if (m_hDSP)
	{
		m_csMessageHandling.Lock();

		enum mdsErrorCode Status;

		MessageStruct Message;
		Message.nMessageID	= nMessageID;
		Message.dwParam1	= dwParam1;
		Message.dwParam2	= dwParam2;
		Message.dwParam3	= dwParam3;
		Message.dwParam4	= dwParam4;
		Message.bAck		= FALSE;

		// Message in Sharedmemory ablegen
		Status = mdsBoardWriteMemory(m_hDSP, MDS_DSP_MEM_DATA, sizeof(Message), PC2DSPMESSAGEBUFFER, &Message, MDS_DSP_INDEX_A);
    
		// check return value
		if (Status == MDS_ERROR_NO_ERROR)
		{
			m_pPerf2->Start();

			// send interrupt to DSP
			Status = mdsBoardInterruptDsp(m_hDSP, MDS_DSP_INDEX_A,	0, 0);	

			// Check return value
			if (Status == MDS_ERROR_NO_ERROR)
			{
				// Auf die Empfangsbestätigung warten.
				DWORD dwTC = GetTickCount();
				do
				{
			 		mdsBoardReadMemory(m_hDSP,	MDS_DSP_MEM_DATA, sizeof(Message), PC2DSPMESSAGEBUFFER, &Message, MDS_DSP_INDEX_A);
				}while(!Message.bAck && (GetTickCount() - dwTC)	> 100);
				m_pPerf2->Stop();

				if (!Message.bAck)
					WK_TRACE_ERROR(_T("Wait for Acknoledge timeout\n"));

				bResult = Message.bAck;
			}
			else
			{
				WK_TRACE_ERROR(_T("Failed to interrupt DSP on Hawk35.\n"));
			}

		}
		else
		{
			WK_TRACE_ERROR(_T("Error writing to DSP data memory.\n"));
		}

		m_csMessageHandling.Unlock();
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::ReceiveMessage(int &nMessageID, DWORD &dwParam1, DWORD &dwParam2, DWORD &dwParam3, DWORD &dwParam4)
{
	BOOL bResult = FALSE;
	if (m_hDSP)
	{
		enum mdsErrorCode Status;

		MessageStruct Message;

		// Lese message
 		Status = mdsBoardReadMemory(m_hDSP,	MDS_DSP_MEM_DATA, sizeof(Message), DSP2PCMESSAGEBUFFER, &Message, MDS_DSP_INDEX_A);

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
			Status = mdsBoardWriteMemory(m_hDSP, MDS_DSP_MEM_DATA, sizeof(Message), DSP2PCMESSAGEBUFFER, &Message, MDS_DSP_INDEX_A);
	  
			// check return value
			if (Status == MDS_ERROR_NO_ERROR)	
				bResult = TRUE;
			else
				WK_TRACE_ERROR(_T("Error can't send acknoledge.\n"));
		}
		else
		{
			WK_TRACE_ERROR(_T("Error reading from DSP data memory.\n"));
		}
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void __stdcall CCodec::MessageReceiveCallback(LPVOID pData)
{	
	CCodec*	pThis	 = (CCodec*)pData;  // Pointer auf's Codec-Objekt
	
	if (!pThis)
	{
		WK_TRACE_ERROR("HOST: MessageReceiveCallback pCodec = NULL\n");
		return;
	}
	pThis->m_csMessageHandling.Lock();

	int		nMessageID	= 0;
	DWORD	dwParam1	= 0;
	DWORD	dwParam2	= 0;
	DWORD	dwParam3	= 0;
	DWORD	dwParam4	= 0;

	if (pThis->ReceiveMessage(nMessageID, dwParam1, dwParam2, dwParam3, dwParam4))
	{
		switch (nMessageID)
		{
			case TMM_CONFIRM_SET_BRIGHTNESS:
				pThis->OnConfirmSetBrightness(dwParam1, (WORD)dwParam2, (int)dwParam3);
				break;
			case TMM_CONFIRM_SET_CONTRAST:
				pThis->OnConfirmSetContrast(dwParam1, (WORD)dwParam2, (int)dwParam3);
				break;
			case TMM_CONFIRM_SET_SATURATION:
				pThis->OnConfirmSetSaturation(dwParam1, (WORD)dwParam2, (int)dwParam3);
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
			case TMM_NOTIFY_ALARM:
				pThis->OnIndicationAlarm(dwParam1);
				break;
			case TMM_NOTIFY_INFORMATION:
				pThis->OnIndicationInformation((int)dwParam1, dwParam2, dwParam3, dwParam4);
				break;
			case TMM_NOTIFY_SLAVE_INIT_READY:
				pThis->OnNotifySlaveInitReady((WORD)dwParam1);
				break;
			default:
				WK_TRACE_WARNING("HOST: Unknown Message (MessageID=%d)\n", nMessageID);

		}
	}
	pThis->m_csMessageHandling.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnReceiveNewCodecDataEx(int nSport, DWORD dwBufferOffset, DWORD dwBufferLen)
{
	BOOL bResult = FALSE;
	if (dwBufferLen == CHANNEL_SIZE*CHANNEL_COUNT/2)
	{	
		if (mem_FrameBuffer.pLinAddr != NULL)
		{
			DWORD dwFrameBufferAddr = (DWORD)mem_FrameBuffer.pLinAddr+dwBufferOffset;
			
			// Lese den Framebuffer des zugehörigen Channels aus
			if (HACCReadMemoryX(m_pFrameBuffer, dwFrameBufferAddr, dwBufferLen, FALSE))
			{
				// Die Daten in die Circularbuffer hängen.
				for (int nI = 0; nI < CHANNEL_COUNT/2; nI++)
				{
					int nChannel = nSport*CHANNEL_COUNT/2+nI;
					m_pCirBuff[nChannel]->Add(m_pFrameBuffer+nI*CHANNEL_SIZE, CHANNEL_SIZE);
				}
				ReleaseSemaphore(m_hFieldReadySemaphore, 1, NULL);
				bResult = TRUE;
			}
		}
	}
	else
		WK_TRACE("Packet size too large (%d>%d)\n", dwBufferLen, CHANNEL_SIZE*CHANNEL_COUNT/2);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// Diese Funktion wird später durch OnReceiveNewCodecDataEx komplett ersetzt.
BOOL CCodec::OnReceiveNewCodecData(int nSport, DWORD dwBufferOffset, DWORD dwBufferLen)
{
	BOOL bResult = FALSE;

	if (dwBufferLen == CHANNEL_SIZE*CHANNEL_COUNT/2)
	{	
		if (mem_FrameBuffer.pLinAddr != NULL)
		{
			DWORD dwFrameBufferAddr = (DWORD)mem_FrameBuffer.pLinAddr+dwBufferOffset;
			
			// Lese den Framebuffer des zugehörigen Channels aus
			if (HACCReadMemoryX(m_pFrameBuffer, dwFrameBufferAddr, dwBufferLen, FALSE))
			{
				DATA_PACKET* pDataPacket = NULL;

				for (int nI = 0; nI < CHANNEL_COUNT/2; nI++)
				{
					int nChannel = nSport*CHANNEL_COUNT/2+nI;
					pDataPacket = (DATA_PACKET*)(m_pFrameBuffer+nI*CHANNEL_SIZE);
					TRACE("MagicHeader=0x%x\n", pDataPacket->dwStartMarker);
					pDataPacket->dwStartMarker= MAGIC_MARKER_START;
					pDataPacket->wSource = nChannel;
					pDataPacket->bValid	 = TRUE;
					pDataPacket->wSizeH	 = 240;
					pDataPacket->wSizeV	 = 136;
					pDataPacket->wBytesPerPixel = 2;
					pDataPacket->wSize	 = sizeof(DATA_PACKET);
					pDataPacket->eType	 = eTypeYUV422;
					pDataPacket->dwImageDataLen = pDataPacket->wSizeH*pDataPacket->wSizeV*pDataPacket->wBytesPerPixel;

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
									if (m_pWnd)
									{
										if (pDataPacket->eType == eTypeYUV422)
											bResult = m_pWnd->OnReceiveNewCodecData(pDataPacket);
									}
								}
								else
									WK_TRACE_ERROR("Size of DATA_PACKET wrong (Channel=%d) (%d != %d)\n", nChannel, pDataPacket->wSize, sizeof(DATA_PACKET)); 
							}
							else
								WK_TRACE_ERROR("Imagesize wrong (Channel=%d HSize=%d VSize=%d BPP=%d Size=%d Bytes\n", nChannel, pDataPacket->wSizeH, pDataPacket->wSizeV, pDataPacket->wBytesPerPixel, pDataPacket->dwImageDataLen);
						}
						else
							WK_TRACE_WARNING("Unvalid data packet (Channel=%d)\n", nChannel);
					}
				}
			}
		}
	}
	else
		WK_TRACE("Packet size too large (%d>%d)\n", dwBufferLen, CHANNEL_SIZE*CHANNEL_COUNT/2);

	return bResult;
}
 
/////////////////////////////////////////////////////////////////////////////
UINT CCodec::ReadDataThread(LPVOID pData)
{
	static DWORD dwLastFieldID = 0;
	static CSystemTime LastTime; 

	CCodec*	pThis	 = (CCodec*)pData;  // Pointer auf's Codec-Objekt
	
	// Thread solange ausführen, bis m_bRun = FALSE
	while (pThis->m_bRun)
	{
		// Soll der Frameheader gesucht werden?
		if (pThis->m_bFrameSync)
		{
			// Warte bis neue Daten eintreffen.
			if (WaitForSingleObject(pThis->m_hFieldReadySemaphore, 500) == WAIT_OBJECT_0)
			{
				DATA_PACKET* pDataPacket = NULL;

				for (int nChannel = 0; nChannel < CHANNEL_COUNT; nChannel++)
				{
					do
					{	
						// Führt darüber Buch, über welche Kanäle Bilddaten kommen.
						pThis->m_nVidPres[nChannel]--;
						pThis->m_nVidPres[nChannel] = max(pThis->m_nVidPres[nChannel],0);
						
						// Hole nächstes Frame aus dem Circularbuffer
						pDataPacket = (DATA_PACKET*)pThis->m_pCirBuff[nChannel]->GetNextFrame();

						// Frame aus dem Circularbuffer gelesen?
						if (pDataPacket)
						{
							// Ist das Struktalignment im Blackfin das selbe wie hier?
							if (pDataPacket->wSize == sizeof(DATA_PACKET))
							{			
								// Liegen Daten vor?		    
								if (pDataPacket->bValid)
								{  	
									// Ist die Prüfsumme korrekt?
									DWORD dwCheckSum = pThis->GetCheckSum(pDataPacket);
									if (pDataPacket->dwCheckSum == dwCheckSum)
									{
										if (pDataPacket->bVidPresent)
											pThis->m_nVidPres[nChannel] = 100;

										if (pDataPacket->eType == eTypeYUV422)
										{
											// Timestamp setzen, wenn nicht schon erfolgt.
											if (!pDataPacket->TimeStamp.bValid)
											{
												CSystemTime	TimeStamp;

												TimeStamp.GetLocalTime();
												pDataPacket->TimeStamp.wYear	= TimeStamp.wYear;
												pDataPacket->TimeStamp.wMonth	= TimeStamp.wMonth;
												pDataPacket->TimeStamp.wDay		= TimeStamp.wDay;
	  											pDataPacket->TimeStamp.wHour	= TimeStamp.wHour;
												pDataPacket->TimeStamp.wMinute	= TimeStamp.wMinute;
												pDataPacket->TimeStamp.wSecond	= TimeStamp.wSecond;
												pDataPacket->TimeStamp.wMSecond	= TimeStamp.wMilliseconds;
											}
											
											// Hat sich die permanente Maske verändert?...Dann die Registry aktualisieren.
											if (pDataPacket->PermanentMask.bHasChanged)
												pThis->SavePermanentMaskToRegistry(&pDataPacket->PermanentMask, pDataPacket->wSource);
											
											if (pDataPacket->Debug.bValid)
												WK_TRACE("BF533 Message:%s",pDataPacket->Debug.sText);
											
											//	WK_TRACE("Cycle counter=%dys\n", pDataPacket->dwProzessTime);

											// Packet weiterreichen...
											if (pThis->m_pWnd)
												pThis->m_pWnd->OnReceiveNewCodecData(pDataPacket);
										}
									}
									else
										WK_TRACE_WARNING("Unvalid checksum (Channel=%d) (%d != %d)\n", nChannel, pDataPacket->dwCheckSum, dwCheckSum);									
								}
								else
									WK_TRACE_WARNING("Unvalid data packet (Channel=%d)\n", nChannel);															
							}
							else
								WK_TRACE_ERROR("Size of DATA_PACKET wrong (Channel=%d) (%d != %d)\n", nChannel, pDataPacket->wSize, sizeof(DATA_PACKET)); 
						}
					}
					while (pDataPacket);

					// Stellt eine Maske zusammen, die darüber Auskunft gibt, von welcher Kamera Daten gekommen sind
					// ToDo: Annahme: Die Kanalnummer entspricht der Kameranummer.
					if (pThis->m_nVidPres[nChannel] > 0)
					{
						if (!TSTBIT(pThis->m_dwCamMask, nChannel))
							pThis->m_pWnd->OnReceivedCameraStatus(nChannel, TRUE);
						pThis->m_dwCamMask = SETBIT(pThis->m_dwCamMask, nChannel);
					}
					else
					{
						if (TSTBIT(pThis->m_dwCamMask, nChannel))
							pThis->m_pWnd->OnReceivedCameraStatus(nChannel, FALSE);
						pThis->m_dwCamMask = CLRBIT(pThis->m_dwCamMask, nChannel);
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
BOOL CCodec::DoConfirmReceiveNewCodecData()
{
	return SendMessage(TMM_CONFIRM_NEW_CODEC_DATA);
}

//////////////////////////////////////////////////////////////////////
DWORD CCodec::GetCheckSum(const DATA_PACKET* pPacket)
{
	DWORD dwCheckSum = 0;

	BYTE* pImageData = (BYTE*)&pPacket->pImageData;

//	for (DWORD dwI = 0; dwI < pPacket->dwImageDataLen; dwI++)	// ToDo: Später wird die Prüfsumme über das komplette Bild berechnet
	for (DWORD dwI = 0; dwI < 100; dwI++)
		dwCheckSum += pImageData[dwI];

	return dwCheckSum;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestScanForCameras(DWORD dwUserData)
{
	BOOL bResult = FALSE;
	WK_TRACE("HOST: DoRequestScanForCameras (UserData=0x%x)\n");

	// Schickt die in 'ReadDataThread' gesammelten Informationen zurück.
	if (m_pWnd)
		bResult = OnConfirmScanForCameras(dwUserData, m_dwCamMask);

	return bResult;
	
//	return SendMessage(TMM_REQUEST_SCAN_FOR_CAMERAS, dwUserData);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask)
{
	WK_TRACE("HOST: OnConfirmScanForCameras (UserData=0x%x, CameraMask=0x%x)\n", dwUserData, dwCameraMask);
	
	BOOL bResult = FALSE;

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmScanForCameras(dwUserData, dwCameraMask);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetRelayState(WORD wRelayID, BOOL bOpenClose)
{
	WK_TRACE("HOST: DoRequestSetRelayState (Relay=%d State=%d)\n", wRelayID, bOpenClose);


	MASK_STRUCT* pPermanentMask	 = NULL;

	pPermanentMask = new MASK_STRUCT;
	memset(pPermanentMask, 255, sizeof(MASK_STRUCT));
	pPermanentMask->bValid		= TRUE;
	pPermanentMask->nDimH		= 45;
	pPermanentMask->nDimV		= 36;

	DoRequestSetPermanentMask(0, 3, (BYTE*)pPermanentMask, sizeof(MASK_STRUCT), FALSE);

	WK_DELETE(pPermanentMask);

	return SendMessage(TMM_REQUEST_SET_RELAIS_STATE, wRelayID, bOpenClose);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose)
{
	WK_TRACE("HOST: OnConfirmSetRelayState (Relay=%d State=%d)\n", wRelayID, bOpenClose);

	BOOL bResult = FALSE;

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmSetRelayState(wRelayID, bOpenClose);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetRelayState()
{
	WK_TRACE("HOST: DoRequestGetRelayState\n");
	return SendMessage(TMM_REQUEST_GET_RELAIS_STATE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetRelayState(DWORD dwRelayStateMask)
{
	WK_TRACE("HOST: OnConfirmGetRelayState (RelayStateMask=0x%x)\n", dwRelayStateMask);

	BOOL bResult = FALSE;

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmGetRelayState(dwRelayStateMask);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetAlarmState()
{
	WK_TRACE("HOST: DoRequestGetAlarmState\n");
	return SendMessage(TMM_REQUEST_GET_ALARM_STATE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetAlarmState(DWORD dwAlarmStateMask)
{
	WK_TRACE("HOST: OnConfirmGetAlarmState (AlarmState=0x%x)\n", dwAlarmStateMask);

	BOOL bResult = FALSE;

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmGetAlarmState(dwAlarmStateMask);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetAlarmEdge(DWORD dwAlarmEdge)
{
	WK_TRACE("HOST: DoRequestSetAlarmEdge (AlarmEdge=0x%x)\n", dwAlarmEdge);
	return SendMessage(TMM_REQUEST_SET_ALARM_EDGE, dwAlarmEdge);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetAlarmEdge(DWORD dwAlarmEdge)
{
	WK_TRACE("HOST: OnConfirmSetAlarmEdge (AlarmEdge=0x%x)\n", dwAlarmEdge);

	BOOL bResult = FALSE;

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmSetAlarmEdge(dwAlarmEdge);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetAlarmEdge()
{
	WK_TRACE("HOST: DoRequestGetAlarmEdge\n");
	return SendMessage(TMM_REQUEST_GET_ALARM_EDGE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetPCIFrameBufferAddress(DWORD dwPCIAddr)
{
	BOOL bResult = FALSE;

	WK_TRACE("HOST: DoRequestSetPCIFrameBufferAddress (Addr=0x%08x)\n", dwPCIAddr);

	m_evSetPCIFrameBufferAddress.ResetEvent();
	if (SendMessage(TMM_REQUEST_SET_FRAMEBUFF_ADDR, dwPCIAddr))
	{
		if (WaitForSingleObject(m_evSetPCIFrameBufferAddress, 2000) == WAIT_TIMEOUT)
			WK_TRACE_WARNING("DoRequestSetPCIFrameBufferAddress: timeout\n");
		else
			bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetPCIFrameBufferAddress(DWORD dwPCIAddr)
{
	WK_TRACE("HOST: OnConfirmSetPCIFrameBufferAddress (Addr=0x%08x)\n", dwPCIAddr);
	m_evSetPCIFrameBufferAddress.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetAlarmEdge(DWORD dwAlarmEdge)
{
	WK_TRACE("HOST: OnConfirmGetAlarmEdge (AlarmEdge=0x%x)\n",dwAlarmEdge);

	BOOL bResult = FALSE;

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmGetAlarmEdge(dwAlarmEdge);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmInitComplete()
{
	WK_TRACE("HOST: OnConfirmInitComplete\n");
	Beep(440, 100);
	m_evTmmInitComplete.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestTermination()
{
	WK_TRACE("HOST: DoRequestTermination\n");

	BOOL bResult = FALSE;

	m_evTmmTermination.ResetEvent();
	if (SendMessage(TMM_REQUEST_TERMINATION))
	{
		if (WaitForSingleObject(m_evTmmTermination, 5000) == WAIT_TIMEOUT)
			WK_TRACE_WARNING("HOST: DoRequestTermination timeout\n");
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
	if (file.Open(sFilename, CFile::modeRead))
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

	if (dwLen < mem_TransferBuffer.dwLen)
	{
		// EPLD-Daten zum BF535 schicken.
		if (HACCWriteMemoryX((void*)pData, (DWORD)mem_TransferBuffer.pLinAddr, dwLen, FALSE))
		{
			WK_TRACE("HOST: DoRequestEPLDdataTransfer (Addr=0x%x, dwLen=%d)\n", mem_TransferBuffer.pPhysAddr, dwLen);

			// BF535 über das Eintreffen der neuen EPLD-Daten informieren.
			m_evEPLDdataTransfer.ResetEvent();
			if (SendMessage(TMM_REQUEST_EPLD_DATA_TRANSFER, (DWORD)mem_TransferBuffer.pPhysAddr, dwLen))
			{
				if (WaitForSingleObject(m_evEPLDdataTransfer, 2000) == WAIT_TIMEOUT)
					WK_TRACE_WARNING("DoRequestEPLDdataTransfer: timeout\n");
				else
					bResult = TRUE;
			}
		}
		else
			WK_TRACE_ERROR("DoRequestEPLDdataTransfer: download data failes\n");
	}
	else
		WK_TRACE_ERROR("DoRequestEPLDdataTransfer: Data to large (%d > %d)\n", dwLen, mem_TransferBuffer.dwLen);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmEPLDdataTransfer(DWORD dwAddr, DWORD dwLen)
{
	WK_TRACE("HOST: OnConfirmEPLDdataTransfer Addr=0x%x, dwLen=%d\n", dwAddr, dwLen);

	m_evEPLDdataTransfer.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::TransferBF533BootData(int nBFNr, const CString &sFilename)
{
	BOOL bResult = FALSE;
	BYTE* pBuffer = NULL;
	
	CFile file;

	if (file.Open(sFilename, CFile::modeRead))
	{
		int nLen = file.GetLength();
		pBuffer = new BYTE[nLen];
		
		if (file.Read(pBuffer, nLen) == (DWORD)nLen)
			bResult = DoRequestBoot533(nBFNr, pBuffer, nLen);

		WK_DELETE(pBuffer);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestBoot533(int nBFNr, const BYTE* pData, DWORD dwLen)
{
	BOOL bResult = FALSE;

	if (dwLen < mem_TransferBuffer.dwLen)
	{
		// Bootdatei zum BF535 schicken.
		if (HACCWriteMemoryX((void*)pData, (DWORD)mem_TransferBuffer.pLinAddr, dwLen, FALSE))
		{
			WK_TRACE("HOST: DoRequestBoot533 (BF533 Nr.%d, Addr=0x%x dwLen=%d)\n", nBFNr, (DWORD)mem_TransferBuffer.pPhysAddr, dwLen);

			// BF535 über das Eintreffen der neuen BF533-Bootdaten informieren.
			m_evBootBF533.ResetEvent();
			if (SendMessage(TMM_REQUEST_BOOT_BF533, nBFNr, (DWORD)mem_TransferBuffer.pPhysAddr, dwLen))
			{
				if (WaitForSingleObject(m_evBootBF533, 2000) == WAIT_TIMEOUT)
					WK_TRACE_WARNING("DoRequestBoot533: timeout\n");
				else
					bResult = TRUE;
			}
		}
		else
			WK_TRACE_ERROR("DoRequestBoot533: failed\n");
	}
	else
		WK_TRACE_ERROR("DoRequestBoot533: Data to large (%d > %d)\n", dwLen, mem_TransferBuffer.dwLen);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmBoot533(int nBFNr, DWORD dwAddr, DWORD dwLen)
{
	WK_TRACE("HOST: OnConfirmBoot533 (BF533 Nr.%d, Addr=0x%x, dwLen=%d)\n" , nBFNr, dwAddr, dwLen);

	m_evBootBF533.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnNotifySlaveInitReady(WORD wSource)
{
	WK_TRACE("HOST: OnNotifySlaveInitReady (BF533 Nr.%d)\n" , wSource);

	// Der BF533 ist startklar
	LoadConfig(wSource);

	if (m_pWnd)
		m_pWnd->OnNotifySlaveInitReady(wSource);

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

	if (file.Open(sFilename, CFile::modeRead))
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
//						TRACE("Addr=%d\t%d\t%d\n", nI, pBuffer1[nI], pBuffer2[nI]);
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
		WK_TRACE_ERROR("EEPROM Readback test failed\n");
		AfxMessageBox(IDS_EEPROM_READBACK_FAILED, MB_OK);
	}

	return bResult && !bError;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestWriteToEEProm(const BYTE* pData, DWORD dwLen, DWORD dwStartAddr)
{
	BOOL bResult = FALSE;

	if (dwLen < mem_TransferBuffer.dwLen)
	{
		// EPLD-Daten zum BF535 schicken.
		if (HACCWriteMemoryX((void*)pData, (DWORD)mem_TransferBuffer.pLinAddr, dwLen, FALSE))
		{
			WK_TRACE("HOST: DoRequestWriteToEEProm (Addr=0x%x, dwLen=%d, dwStartAddr=0x%x)\n", mem_TransferBuffer.pPhysAddr, dwLen, dwStartAddr);

			// BF535 über das Eintreffen der neuen EEProm-Daten informieren.
			m_evEEPromDataTransfer.ResetEvent();
			if (SendMessage(TMM_REQUEST_WRITE_EEPROM, (DWORD)mem_TransferBuffer.pPhysAddr, dwLen, dwStartAddr))
			{
				if (WaitForSingleObject(m_evEEPromDataTransfer, 60000) == WAIT_TIMEOUT)
					WK_TRACE_WARNING("DoRequestWriteToEEProm: timeout\n");
				else
					bResult = TRUE;
			}
		}
		else
			WK_TRACE_ERROR("DoRequestWriteToEEProm: download data failes\n");
	}
	else
		WK_TRACE_ERROR("DoRequestWriteToEEProm: Data to large (%d > %d)\n", dwLen, mem_TransferBuffer.dwLen);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmWriteToEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr)
{
	WK_TRACE("HOST: OnConfirmWriteToEEProm Addr=0x%x, dwLen=%d dwStartAddr=0x%x\n", dwAddr, dwLen, dwStartAddr);

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

	if (dwLen < mem_TransferBuffer.dwLen)
	{
		WK_TRACE("HOST: DoRequestReadFromEEProm (Addr=0x%x, dwLen=%d, dwStartAddr=0x%x)\n", mem_TransferBuffer.pPhysAddr, dwLen, dwStartAddr);

		// BF535 über das Eintreffen der neuen EEProm-Daten informieren.
		m_evEEPromDataTransfer.ResetEvent();
		if (SendMessage(TMM_REQUEST_READ_EEPROM, (DWORD)mem_TransferBuffer.pPhysAddr, dwLen, dwStartAddr))
		{
			if (WaitForSingleObject(m_evEEPromDataTransfer, 5000) == WAIT_TIMEOUT)
				WK_TRACE_WARNING("DoRequestReadFromEEProm: timeout\n");
			else
				bResult = HACCReadMemoryX((void*)pData, (DWORD)mem_TransferBuffer.pLinAddr, dwLen, FALSE);
		}
	}
	else
		WK_TRACE_ERROR("DoRequestReadFromEEProm: Data to large (%d > %d)\n", dwLen, mem_TransferBuffer.dwLen);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetPermanentMask(DWORD dwUserData, WORD wSource, const BYTE* pData, DWORD dwLen, BOOL bWait)
{
	BOOL bResult = FALSE;

	if (dwLen < mem_TransferBuffer.dwLen)
	{
		// Permanente Maske zum BF535 schicken.
		if (HACCWriteMemoryX((void*)pData, (DWORD)mem_TransferBuffer.pLinAddr, dwLen, FALSE))
		{
			if (m_bTraceSetPermanentMask)
				WK_TRACE("HOST: DoRequestSetPermanentMask (dwUserData=0x%x wSource=%d Addr=0x%x, dwLen=%d)\n", dwUserData, wSource, mem_TransferBuffer.pPhysAddr, dwLen);

			// BF535 über das Eintreffen der neuen EEProm-Daten informieren.
			m_evSetPermanentMask.ResetEvent();
			if (SendMessage(TMM_REQUEST_SET_PERMANENT_MASK, dwUserData, wSource, (DWORD)mem_TransferBuffer.pPhysAddr, dwLen))
			{
				if (bWait)
				{
					if (WaitForSingleObject(m_evSetPermanentMask, 2000) == WAIT_TIMEOUT)
						WK_TRACE_WARNING("DoRequestSetPermanentMask: timeout (wSource=%d)\n", wSource);
					else
						bResult = TRUE;
				}
				else bResult = TRUE;
			}
		}
		else
			WK_TRACE_ERROR("DoRequestSetPermanentMask: download permanent mask failed (wSource=%d)\n", wSource);
	}
	else
		WK_TRACE_ERROR("DoRequestSetPermanentMask: Data to large (wSource=%d, %d > %d)\n", wSource, dwLen, mem_TransferBuffer.dwLen);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetPermanentMask(DWORD dwUserData, WORD wSource, DWORD dwAddr, DWORD dwLen)
{
	if (m_bTraceSetPermanentMask)
		WK_TRACE("HOST: OnConfirmSetPermanentMask dwUserData=0x%x, wSource=%d Addr=0x%x, dwLen=%d\n", dwUserData, wSource, dwAddr, dwLen);

	m_evSetPermanentMask.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestClearPermanentMask(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceClearPermanentMask)
		WK_TRACE("HOST: DoRequestClearPermanentMask (UserData=0x%x Source=%d)\n", dwUserData, wSource);
	
	return SendMessage(TMM_REQUEST_CLEAR_PERMANENT_MASK, dwUserData, wSource);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmClearPermanentMask(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceClearPermanentMask)
		WK_TRACE("HOST: OnConfirmClearPermanentMask (UserData=0x%x Source=%d\n", dwUserData, wSource);
	
	if (!m_pWnd)
		return FALSE;

	// Die Maske in der Registrierung aktualisieren.
	MASK_STRUCT* pPermanentMask = new MASK_STRUCT;
	if (pPermanentMask)
	{
		pPermanentMask->bValid		= TRUE;
		pPermanentMask->nDimH		= 45;
		pPermanentMask->nDimV		= 36;

		for (int nY=0; nY < pPermanentMask->nDimV; nY++)
		{
			for (int nX=0; nX < pPermanentMask->nDimH; nX++)
				pPermanentMask->byMask[nY][nX] = 0xff;
		}
		SavePermanentMaskToRegistry(pPermanentMask, wSource);
	}
	WK_DELETE(pPermanentMask);

	return m_pWnd->OnConfirmClearPermanentMask(dwUserData, wSource);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestInvertPermanentMask(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceInvertPermanentMask)
		WK_TRACE("HOST: DoRequestInvertPermanentMask (UserData=0x%x Source=%d)\n", dwUserData, wSource);
	
	return SendMessage(TMM_REQUEST_INVERT_PERMANENT_MASK, dwUserData, wSource);
}	 

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmInvertPermanentMask(DWORD dwUserData, WORD wSource)
{
	if (m_bTraceInvertPermanentMask)
		WK_TRACE("HOST: OnConfirmInvertPermanentMask (UserData=0x%x Source=%d)\n", dwUserData, wSource);
	
	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmInvertPermanentMask(dwUserData, wSource);
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
	if (file.Open(sFilename, CFile::modeRead))
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
				WK_TRACE("%s // RecordType=%d AddrOffset=%d dwAddr=%d dwNum=%d\n", sLine, wRecordType, dwExtLinAddr, dwAddr, dwNum, wRecordType);
				WK_TRACE_WARNING("Unsupported RecordType (0x%x) in IntelHEX-file...loading aborded\n", wRecordType);
				bErr = TRUE;
			}
			if (wRecordType == EXT_LIN_ADDR_RECORD)
				dwExtLinAddr = HexToInt(sLine.Mid(9,2))<<24 | HexToInt(sLine.Mid(11,2))<<16;
			if (wRecordType == DATA_RECORD || wRecordType == EXT_LIN_ADDR_RECORD)
				dwMaxAddr = max(dwMaxAddr, dwExtLinAddr+dwAddr+dwNum);
		}

//		WK_TRACE("Maximal Address=%d\n", dwMaxAddr);
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
				
//				WK_TRACE("%s // RecordType=%d AddrOffset=%d dwAddr=%d dwNum=%d\n", sLine, wRecordType, dwExtLinAddr, dwAddr, dwNum, wRecordType);
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
	WK_TRACE("HOST: OnConfirmReadFromEEProm Addr=0x%x, dwLen=%d dwStartAddr=0x%x\n", dwAddr, dwLen, dwStartAddr);

	m_evEEPromDataTransfer.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmTermination()
{
	WK_TRACE("HOST: OnConfirmTermination\n");

	m_evTmmTermination.SetEvent();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnIndicationAlarm(DWORD dwAlarmMask)
{
	WK_TRACE("HOST: OnIndicationAlarm (AlarmMask=0x%x)\n", dwAlarmMask);

	BOOL bResult = FALSE;

	if (m_pWnd)
		bResult = m_pWnd->OnIndicationAlarm(dwAlarmMask);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnIndicationInformation(int nMessage, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{
	switch (nMessage)
	{
		case INFO_SPORT_TIMING:
			{
				// dwParam1=SPORT0/SPORT1, dwParam2=cycles, dwParam3=SPORTx_COUNT_RX
				int		nSportNr	= dwParam1;
				int		nCycles		= dwParam2;
				int		nCounter	= dwParam3;
				float	fTime		= (float)nCycles/300.0;
				
				if (nSportNr == 0)
					WK_TRACE("HOST: SPORT_TIMING: cycles=%d\ttime=%f ys\tSPORT0_COUNT_RX=%d\n", nCycles, fTime, nCounter);
				else if (nSportNr == 1)
					WK_TRACE("HOST: SPORT_TIMING: cycles=%d\ttime=%f ys\tSPORT1_COUNT_RX=%d\n\n", nCycles, fTime, nCounter);					
			}
			break;
		case INFO_UNKNOWN_MESSAGE_ID:
			WK_TRACE_WARNING("BF535: Unknown MessageID (%d)\n", (int)dwParam2);
			break;
		case INFO_TOO_MANY_DESCRIPTORS:
			WK_TRACE_WARNING("BF535: Too many descriptors (%d)\n", (int)dwParam2);
			break;
		case INFO_EPLD_PROGRAMMING_FAILED:
			WK_TRACE_WARNING("BF535: EPLD Programming failed\n");
			break;
		case INFO_WRITE_TO_EEPROM_FAILED:
			WK_TRACE_WARNING("BF535: Write to EEProm failed\n");
			break;
		case INFO_READ_FROM_EEPROM_FAILED:
			WK_TRACE_WARNING("BF535: Read from EEProm failed\n");
			break;
		case INFO_TOO_MANY_CHANNELS:
			WK_TRACE_WARNING("BF535: Too many channels\n");
			break;	
		case INFO_BOOT533_FAILED:
			WK_TRACE_WARNING("BF535: Boot BF533 Nr. %d failed\n", dwParam1);
			break;	
		case INFO_SEND_SPI_COMMAND_TIMEOUT:
			WK_TRACE_WARNING("BF535: Send SPI Command %d, Param=%d to Slave %d timeout\n", dwParam1, dwParam2, dwParam3);
			break;
		case INFO_SEND_SPI_COMMAND_PENDING:
			WK_TRACE_WARNING("BF535: Send SPI Command %d, Param=%d to Slave %d pending\n", dwParam1, dwParam2, dwParam3);
			break;
		case INFO_SLAVE_PF_REQUEST:
			WK_TRACE("BF533: INFO_SLAVE_PF_REQUEST (Slave=%d)\n", dwParam1);
			break;
		case INFO_PPI_PREAMBEL_ERROR:
			WK_TRACE("BF533: INFO_PPI_PREAMBEL_ERROR (Slave=%d)\n", dwParam1);
			break;
		case INFO_PPI_PREAMBEL_ERROR_NO_CORR:
			WK_TRACE("BF533: INFO_PPI_PREAMBEL_ERROR_NO_CORR (Slave=%d)\n", dwParam1);
			break;	
		case INFO_PPI_ERROR:
			WK_TRACE("BF533: INFO_PPI_ERROR (Slave=%d)\n", dwParam1);
			break;	
		default:
			WK_TRACE_WARNING("BF533: Unknown Message (MessageId=%d)\n", nMessage);
			break;
	}

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
	BOOL bResult = FALSE;

	_TCHAR szBuffer[SIZE];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	_TCHAR szSourceType[SIZE];
	ZeroMemory(szSourceType, sizeof(szSourceType));

	_TCHAR szVideoFormat[SIZE];
	ZeroMemory(szVideoFormat, sizeof(szVideoFormat));

	_TCHAR szFilter[SIZE];
	ZeroMemory(szFilter, sizeof(szFilter));

	// Angaben über zu protokollierene Funktionen aus der Registry
	ReadDebugConfiguration();

	wsprintf(szBuffer, _T("Slaves\\Source%02u"), wSource);
	
	// Jede Kamera hat ihre eigenen Einstellungen
	DoRequestSetBrightness(0, wSource, ReadConfigurationInt(szBuffer, _T("Brightness"), 50, m_sAppIniName));
	DoRequestSetContrast(0, wSource, ReadConfigurationInt(szBuffer, _T("Contrast"), 50, m_sAppIniName));
	DoRequestSetSaturation(0, wSource, ReadConfigurationInt(szBuffer, _T("Saturation"), 50, m_sAppIniName));

	wsprintf(szBuffer, _T("Slaves\\Source%02u\\MD"), wSource);

	//MD-Parameter laden
	DoRequestSetMDTreshold(0, wSource,		ReadConfigurationInt(szBuffer, _T("MDTreshold"), 7, m_sAppIniName));
	DoRequestSetMaskTreshold(0, wSource,	ReadConfigurationInt(szBuffer, _T("MaskTreshold"), 7, m_sAppIniName));
	DoRequestSetMaskIncrement(0, wSource,	ReadConfigurationInt(szBuffer, _T("MaskIncrement"), 6, m_sAppIniName));
	DoRequestSetMaskDelay(0, wSource,		ReadConfigurationInt(szBuffer, _T("MaskDelay"), 0, m_sAppIniName));
	
	MASK_STRUCT* pPermanentMask	 = new MASK_STRUCT;
	if (!ReadPermanentMaskFromRegistry(pPermanentMask, wSource))
	{
		pPermanentMask->bValid		= TRUE;
		pPermanentMask->nDimH		= 45;
		pPermanentMask->nDimV		= 36;

		for (int nY=0; nY < pPermanentMask->nDimV; nY++)
		{
			for (int nX=0; nX < pPermanentMask->nDimH; nX++)
				pPermanentMask->byMask[nY][nX] = 0xff;
		}
		SavePermanentMaskToRegistry(pPermanentMask, wSource);
	}

	bResult = DoRequestSetPermanentMask(0, wSource, (BYTE*)pPermanentMask, sizeof(MASK_STRUCT), FALSE);

	WK_DELETE(pPermanentMask);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SaveConfig(WORD wSource)
{
	return TRUE;
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
	m_bTraceInvertPermanentMask	= (BOOL)ReadConfigurationInt(_T("Debug\\Trace"), _T("InvertPermanentMask"),	m_bTraceInvertPermanentMask,		m_sAppIniName);

	// Und im Logfile protokollieren
	WK_TRACE(_T("TraceStartEncoder=%s\n"),			(m_bTraceStartEncoder		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceStopEncoder=%s\n"),			(m_bTraceStopEncoder		? _T("On") : _T("Off")));
	WK_TRACE(_T("TracePauseEncoder=%s\n"),			(m_bTracePauseEncoder		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceResumeEncoder=%s\n"),			(m_bTraceResumeEncoder		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceSetBrighness=%s\n"),			(m_bTraceSetBrightness		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceSetContrast=%s\n"),			(m_bTraceSetContrast		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceSetSaturation=%s\n"),			(m_bTraceSetSaturation		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceSetRelais=%s\n"),				(m_bTraceSetRelais			? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceSetAlarmEdge=%s\n"),			(m_bTraceSetAlarmEdge		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceGetBrighness=%s\n"),			(m_bTraceGetBrightness		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceGetContrast=%s\n"),			(m_bTraceGetContrast		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceGetSaturation=%s\n"),			(m_bTraceGetSaturation		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceGetRelais=%s\n"),				(m_bTraceGetRelais			? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceGetAlarmEdge=%s\n"),			(m_bTraceGetAlarmEdge		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceSetMDTreshold=%s\n"),			(m_bTraceSetMDTreshold		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceSetMaskTreshold=%s\n"),		(m_bTraceSetMaskTreshold	? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceSetMaskIncrement=%s\n"),		(m_bTraceSetMaskIncrement	? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceSetMaskDelay=%s\n"),			(m_bTraceSetMaskDelay		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceSetPermanentMask=%s\n"),		(m_bTraceSetPermanentMask	? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceChangePermanentMask=%s\n"),	(m_bTraceChangePermanentMask? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceGetMDTreshold=%s\n"),			(m_bTraceGetMDTreshold		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceGetMaskTreshold=%s\n"),		(m_bTraceGetMaskTreshold	? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceGetMaskIncrement=%s\n"),		(m_bTraceGetMaskIncrement	? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceGetMaskDelay=%s\n"),			(m_bTraceGetMaskDelay		? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceClearPermanentMask=%s\n"),	(m_bTraceClearPermanentMask	? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceInvertPermanentMask=%s\n"),	(m_bTraceInvertPermanentMask? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceAlarmIndication=%s\n"),		(m_bTraceAlarmIndication	? _T("On") : _T("Off")));
	WK_TRACE(_T("TraceAnalogOutSwitch=%s\n"),		(m_bTraceAnalogOutSwitch	? _T("On") : _T("Off")));
}

