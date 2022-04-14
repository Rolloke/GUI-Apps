// CCodec.cpp: implementation of the CCodec class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TashaUnit.h"
#include "CCodec.h"
#include "TashaUnitDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define		PAGE_SIZE			0x1000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CCodec::CCodec(CTashaUnitDlg* pWnd, int nDSPNumber /*=0*/)
{
	TMStatus		Status;
	tmmanVersion	Version;

	m_nDSPNumber			= nDSPNumber;
	m_hDSP					= 0;
	m_nCRTHandle			= 0;
	m_hMessageSynch			= NULL;
	m_hMessage				= 0;
	m_pMessageLoopThread	= NULL;
	m_bRun					= FALSE;
	m_pWnd					= pWnd;
	
	m_dwSMBufferSize		= SM_BUFFER_SIZE;
	m_pDataPacket			= NULL;
	m_hSMBufferHandle		= NULL;

	m_eEncoderState			= eEncoderStateUnvalid;

	Version.Major = constTMManDefaultVersionMajor;
	Version.Minor = constTMManDefaultVersionMinor;
		
	Status = tmmanNegotiateVersion(constTMManDefault, &Version);

	if (Status == statusSuccess)
		WK_TRACE("HOST: TMMAN-Version: %d.%d\n", Version.Major, Version.Minor);
	else
		WK_TRACE_ERROR("HOST: tmmanNegotiateVersion failed Status[%x]\n", Status );
}

//////////////////////////////////////////////////////////////////////
CCodec::~CCodec()
{
	Close();
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::Open(const CString& sImagePath)
{
	TMStatus	Status;
	BOOL bResult = FALSE;

	CRunTimeParameterBlock	CRTParam;

	m_hExitEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	if (m_hExitEvent == 0)
		return FALSE;

	ZeroMemory(&CRTParam, sizeof(CRTParam));
	CRTParam.OptionBitmap		= 0;
	CRTParam.StdInHandle		= (DWORD)GetStdHandle ( STD_INPUT_HANDLE );
	CRTParam.StdOutHandle		= (DWORD)GetStdHandle ( STD_OUTPUT_HANDLE );
	CRTParam.StdErrHandle		= (DWORD)GetStdHandle ( STD_ERROR_HANDLE );
	CRTParam.VirtualNodeNumber	= 0;
	CRTParam.CRTThreadCount		= 1;

	CRTParam.OptionBitmap |= constCRunTimeFlagsUseSynchObject ;
#ifdef _DEBUG
	CRTParam.OptionBitmap |= constCRunTimeFlagsAllocConsole ;
#else
	CRTParam.OptionBitmap |= constCRunTimeFlagsNoConsole ;
#endif
	CRTParam.SynchronizationObject = (DWORD)m_hExitEvent;

	// Open the DSP
	Status = tmmanDSPOpen ( m_nDSPNumber, &m_hDSP);

	if (Status == statusSuccess)
	{
		// Einen biderektionalen Messagechannel zwischen Host und Target öffnen
		if (OpenMessageChannel())
		{
			// Download the Target Executable
			Status = tmmanDSPLoad(m_hDSP, constTMManDefault, (signed char *)(LPCSTR)sImagePath);
			
			if (Status == statusSuccess)
			{
				tmmanDSPReset(m_hDSP);
				cruntimeInit();
				UInt8* pArg[1] = {0};

				pArg[0] = (UInt8*)(LPCSTR)sImagePath;
				if (cruntimeCreate(m_nDSPNumber, 1,	pArg,	&CRTParam,	&m_nCRTHandle ) == True)	
				{
					if (OpenDataChannel())
					{				
						// Run the Target Executable
						Status = tmmanDSPStart(m_hDSP);
						if (Status == statusSuccess)
						{
							m_evTmmInitComplete.ResetEvent();
							if (WaitForSingleObject(m_evTmmInitComplete, 5000) == WAIT_TIMEOUT)
								WK_TRACE_WARNING("HOST: TmmInitComplete timeout\n");
							else
								bResult = TRUE;
						}
						else
							WK_TRACE_ERROR("HOST: tmmanDSPStart failed Status[0x%x]\n", Status);
					}
					else
						WK_TRACE_ERROR("HOST: Can't open Data channel\n");
				}
				else
					WK_TRACE_ERROR("HOST: cruntimeCreate failed\n");
			}
			else
				WK_TRACE_ERROR("HOST: tmmanDSPLoad: failed[0x%x]\n", Status);
		}
		else
			WK_TRACE_ERROR("HOST: Can't open Message channel\n");
	}
	else
		WK_TRACE_ERROR("HOST: tmmanDSPOpen failed Status[0x%x]\n", Status);


	return bResult;
}

//////////////////////////////////////////////////////////////////////
void CCodec::Close()
{
	UInt32  dwExitCode = 0;

	if (m_hDSP)
	{
		// Target MessageLoop Thread terminieren.
		SendMessage(TMM_QUIT, 0, 0);
		
		// wait until the server's exit function is called 
		if (WaitForSingleObject (m_hExitEvent, 1000) == WAIT_OBJECT_0)
			WK_TRACE("HOST: Target terminated\n");
		else
			WK_TRACE_WARNING("HOST: WaitForSingleObject (ExitEvent) timeout\n");
		
		CloseMessageChannel();
		CloseDataChannel();
		
		tmmanDSPStop(m_hDSP); 
		tmmanDSPClose(m_hDSP);
		m_hDSP = NULL;
	}

	if (m_nCRTHandle)
	{
		cruntimeExit();			
		cruntimeDestroy(m_nCRTHandle, &dwExitCode);			
   		m_nCRTHandle = 0;
	}

	if (m_hExitEvent)
	{
		CloseHandle (m_hExitEvent);
		m_hExitEvent = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OpenMessageChannel()
{
	TMStatus	Status;
	BOOL bResult = FALSE;
	//m_hMessageSynch = CreateSemaphore(NULL, 0, 1000, NULL);
	m_hMessageSynch = CreateEvent( NULL, FALSE, FALSE, NULL );
	if (m_hMessageSynch != NULL)
	{
		Status = tmmanMessageCreate( 
							m_hDSP, 
							MESSAGE_NAME, /* message ID - should be the same on the host */
							(UInt32)m_hMessageSynch,
							constTMManModuleHostUser, /* Synchronizaton Flags */
							&m_hMessage);

		if ( Status == statusSuccess )
			bResult = TRUE;
		else
			WK_TRACE_ERROR ("HOST: tmmanMessageCreate failed Status[%x]\n", Status);

	}
	else
		WK_TRACE_ERROR("HOST: CreateEvent failed Status[%x]\n", GetLastError());

	// Message empfangs Thread anlegen.
	m_bRun	= TRUE;
	m_pMessageLoopThread = AfxBeginThread(MessageLoop, this);
	if (m_pMessageLoopThread)
	{
		m_pMessageLoopThread->m_bAutoDelete = FALSE;
		m_pMessageLoopThread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL/*THREAD_PRIORITY_NORMAL*/);
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::CloseMessageChannel()
{
	m_bRun = FALSE;

	// Warte bis 'MessageLoop Thread' beendet ist
	if (m_pMessageLoopThread)
	{
		if (WaitForSingleObject(m_pMessageLoopThread->m_hThread, 1000) == WAIT_TIMEOUT)
			WK_TRACE_WARNING("HOST: WaitForSingleObject (Exit MessageLoopThread) timeout\n");
	}	

	WK_DELETE(m_pMessageLoopThread); //Autodelete = FALSE;

	if (m_hMessageSynch)
	{
		CloseHandle (m_hMessageSynch);
		m_hMessageSynch = NULL;
	}
	
	if (m_hMessage)
	{
		tmmanMessageDestroy (m_hMessage);
		m_hMessage = NULL;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OpenDataChannel()
{
	TMStatus	Status;
	BOOL bResult = FALSE;
	if (m_hDSP)
	{
		Status = tmmanSharedMemoryCreate(m_hDSP, BUFFER_NAME,
										 m_dwSMBufferSize+sizeof(DataPacket),
										 (UInt32*)&m_pDataPacket,
										 &m_hSMBufferHandle);
		if (Status == statusSuccess)
		{
			ZeroMemory((void*)m_pDataPacket, m_dwSMBufferSize+sizeof(DataPacket));
			bResult = TRUE;
		}
		else
			WK_TRACE_ERROR("HOST: tmmanSharedMemoryCreate failed Status[0x%x]\n", Status);
	}
	else
		WK_TRACE_ERROR("HOST: Dsp not open\n");

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::CloseDataChannel()
{
	if (m_hSMBufferHandle)
	{
		tmmanSharedMemoryDestroy(m_hSMBufferHandle);
		m_hSMBufferHandle = NULL;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
//	WK_TRACE("HOST: DoRequestSetBrightness (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);
	return SendMessage(TMM_REQUEST_SET_BRIGHTNESS, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
//	WK_TRACE("HOST: DoRequestSetContrast (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);
	return SendMessage(TMM_REQUEST_SET_CONTRAST, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
//	WK_TRACE("HOST: DoRequestSetSaturation (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);
	return SendMessage(TMM_REQUEST_SET_SATURATION, dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetBrightness(DWORD dwUserData, WORD wSource)
{
//	WK_TRACE("HOST: DoRequestGetBrightness (UserData=0x%x Source=%d)\n", dwUserData, wSource);
	return SendMessage(TMM_REQUEST_GET_BRIGHTNESS, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetContrast(DWORD dwUserData, WORD wSource)
{
//	WK_TRACE("HOST: DoRequestGetContrast (UserData=0x%x Source=%d)\n", dwUserData, wSource);
	return SendMessage(TMM_REQUEST_GET_CONTRAST, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetSaturation(DWORD dwUserData, WORD wSource)
{
//	WK_TRACE("HOST: DoRequestGetSaturation (UserData=0x%x Source=%d)\n", dwUserData, wSource);
	return SendMessage(TMM_REQUEST_GET_SATURATION, dwUserData, wSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
//	WK_TRACE("HOST: OnConfirmSetBrightness (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetBrightness(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
//	WK_TRACE("HOST: OnConfirmSetContrast (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetContrast(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
//	WK_TRACE("HOST: OnConfirmSetSaturation (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetSaturation(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
//	WK_TRACE("HOST: OnConfirmGetBrightness (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetBrightness(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
//	WK_TRACE("HOST: OnConfirmGetContrast (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetContrast(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
//	WK_TRACE("HOST: OnConfirmGetSaturation (UserData=0x%x Source=%d Value=%d)\n", dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetSaturation(dwUserData, wSource, nValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestResumeCapture() 
{
//	WK_TRACE("HOST: DoRequestResumeCapture\n");
	return SendMessage(TMM_REQUEST_RESUME_CAPTURE, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestPauseCapture() 
{
//	WK_TRACE("HOST: DoRequestPauseCapture\n");
	return SendMessage(TMM_REQUEST_PAUSE_CAPTURE, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestStopCapture() 
{
//	WK_TRACE("HOST: DoRequestStopCapture\n");
	return SendMessage(TMM_REQUEST_STOP_CAPTURE, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestStartCapture() 
{
//	WK_TRACE("HOST: DoRequestStartCapture\n");
	return SendMessage(TMM_REQUEST_START_CAPTURE, 0, 0);
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmResumeCapture()
{
	BOOL bResult = FALSE;

//	WK_TRACE("HOST: OnConfirmResumeCapture\n");
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
BOOL CCodec::OnConfirmPauseCapture() 
{
	BOOL bResult = FALSE;

//	WK_TRACE("HOST: OnConfirmPauseCapture\n");
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

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmStopCapture() 
{
	BOOL bResult = FALSE;

//	WK_TRACE("HOST: OnConfirmStopCapture\n");

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

//////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmStartCapture() 
{
	BOOL bResult = FALSE;

//	WK_TRACE("HOST: OnConfirmStart\n");

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

//////////////////////////////////////////////////////////////////////
BOOL CCodec::SendMessage(int nMessageID, DWORD dwParam1/*=0*/, DWORD dwParam2/*=0*/, DWORD dwParam3/*=0*/,DWORD dwParam4/*=0*/)
{
	TMStatus	Status;
	BOOL bResult = FALSE;

	tmmanPacket	Packet;

	if (!m_hMessage)
		return FALSE;

	Packet.Argument[0] = nMessageID;
	Packet.Argument[1] = dwParam1;
	Packet.Argument[2] = dwParam2;
	Packet.Argument[3] = dwParam3;
	Packet.Argument[4] = dwParam4;

	Status = tmmanMessageSend(m_hMessage, &Packet);

	if (Status == statusSuccess)
		bResult = TRUE;
	else
		WK_TRACE_ERROR("HOST: tmmanMessageSend Status[%x]\n", Status);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
UINT CCodec::MessageLoop(LPVOID pData)
{	 
	CCodec*	pThis	 = (CCodec*)pData;  // Pointer auf's Codec-Objekt

	if (!pThis)
	{
		WK_TRACE_ERROR("HOST: MessageLoop pCodec = NULL\n");
		return 0;
	}

	int		nMessageID	= 0;
	DWORD	dwParam1	= 0;
	DWORD	dwParam2	= 0;
	DWORD	dwParam3	= 0;
	DWORD	dwParam4	= 0;
	do
	{
		// Warte auf nächste Message
		WaitForSingleObject(pThis->m_hMessageSynch, 100);
		while (pThis->m_bRun && pThis->ReceiveMessage(nMessageID, dwParam1, dwParam2, dwParam3, dwParam4))
		{
//			WK_TRACE("HOST: MessageID=%d Param1=%d Param2=%d\n", nMessageID, dwParam1, dwParam2, dwParam3, dwParam4);
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
					pThis->OnReceiveNewCodecData();
					break;
				case TMM_CONFIRM_SET_INPUT_SOURCE:
					pThis->OnConfirmSetInputSource((WORD)dwParam1, dwParam2);
					break;
				case TMM_CONFIRM_GET_INPUT_SOURCE:
					pThis->OnConfirmGetInputSource((WORD)dwParam1, dwParam2);
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
				case TMM_CONFIRM_INIT_COMPLETE:
					pThis->OnConfirmInitComplete();
					break;
			}
		}
	}while ((nMessageID != TMM_QUIT) && pThis->m_bRun);


	WK_TRACE("HOST: Exit Message Loop\n");
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::ReceiveMessage(int &nMessageID, DWORD &dwParam1, DWORD &dwParam2, DWORD &dwParam3, DWORD &dwParam4)
{
	BOOL bResult = FALSE;
	TMStatus	Status	= 0;
	
	if (m_hMessage)
	{
		tmmanPacket	Packet;

		// Lese message
		Status = tmmanMessageReceive(m_hMessage, &Packet);
		if (Status == statusSuccess)
		{
			nMessageID	= Packet.Argument[0];
			dwParam1	= Packet.Argument[1];
			dwParam2	= Packet.Argument[2];
			dwParam3	= Packet.Argument[3];
			dwParam4	= Packet.Argument[4];
			bResult		= TRUE;
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnReceiveNewCodecData()
{
	if (m_pDataPacket->bValid)
	{
		// Daten kopieren
		DataPacket* pCopyPacket = (DataPacket*) new BYTE[sizeof(DataPacket)+m_pDataPacket->dwDataLen];
		
		memcpy(pCopyPacket, m_pDataPacket, sizeof(DataPacket)+m_pDataPacket->dwDataLen);

		// Und das Confirm so schnell es geht zurück
		DoConfirmReceiveNewCodecData();

		if (m_pWnd)
			m_pWnd->OnReceiveNewCodecData(pCopyPacket);

		WK_DELETE(pCopyPacket);
	}
	else
		WK_TRACE_WARNING("Unvalid data packet\n");

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmReceiveNewCodecData()
{
	// Aus performance gründen hier schon quittieren
	m_pDataPacket->bValid = FALSE;
	return SendMessage(TMM_CONFIRM_NEW_CODEC_DATA);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetInputSource(SOURCE_SELECTION &SrcSel1, SOURCE_SELECTION &SrcSel2)
{
	BOOL		bResult = FALSE;
	TMStatus	Status	= 0;

//	WK_TRACE("HOST: DoRequestSetInputSource Source=%d\n", SrcSel1.wSource);
				 
	SOURCE_SELECTION* pSrcSel1  = NULL;
	SOURCE_SELECTION* pSrcSel2	= NULL;
	UInt32 hSrcSel1 = 0;
	UInt32 hSrcSel2 = 0;
	Status = tmmanSharedMemoryCreate(m_hDSP, SRC_SEL1, sizeof(SOURCE_SELECTION), (UInt32*)&pSrcSel1, &hSrcSel1);
	if (Status == statusSuccess)
	{
		memcpy((void*)pSrcSel1, &SrcSel1, sizeof(SOURCE_SELECTION));
		   
		Status = tmmanSharedMemoryCreate(m_hDSP, SRC_SEL2, sizeof(SOURCE_SELECTION), (UInt32*)&pSrcSel2,	&hSrcSel2);
		if (Status == statusSuccess)
		{
			memcpy((void*)pSrcSel2, &SrcSel2, sizeof(SOURCE_SELECTION));
			m_evSrcSelection.ResetEvent();
			if (SendMessage(TMM_REQUEST_SET_INPUT_SOURCE, (DWORD)pSrcSel1, (DWORD)pSrcSel2))
			{
				// Warte bis Umschaltung komplett.
				if (WaitForSingleObject(m_evSrcSelection, 1000) == WAIT_OBJECT_0)
					bResult = TRUE;
				else
					WK_TRACE_WARNING("HOST: SetInputSource timeout\n");
			}
			tmmanSharedMemoryDestroy(hSrcSel1);
		}
		else
			WK_TRACE_ERROR("HOST: tmmanSharedMemoryCreate failed Status[0x%x]\n", Status);
	
		tmmanSharedMemoryDestroy(hSrcSel2);
	}
	else
		WK_TRACE_ERROR("HOST: tmmanSharedMemoryCreate failed Status[0x%x]\n", Status);
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetInputSource(WORD wSource, DWORD dwProcessID)
{
	BOOL bResult = FALSE;

//	WK_TRACE("HOST: OnConfirmSetInputSource (Source=%d ProcessID=0x%x)\n", wSource, dwProcessID);
	m_evSrcSelection.SetEvent();

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmSetInputSource(wSource, dwProcessID);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetInputSource()
{
//	WK_TRACE("HOST: DoRequestGetInputSource\n");
	return SendMessage(TMM_REQUEST_GET_INPUT_SOURCE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetInputSource(WORD wSource, DWORD dwProcessID)
{
//	WK_TRACE("HOST: OnConfirmGetInputSource (Source=%d, UserID=0x%x)\n", wSource, dwProcessID);

	BOOL bResult = FALSE;

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmGetInputSource(wSource, dwProcessID);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestScanForCameras(DWORD dwUserData)
{
	WK_TRACE("HOST: DoRequestScanForCameras (UserData=0x%x)\n");
	return SendMessage(TMM_REQUEST_SCAN_FOR_CAMERAS, dwUserData);
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
//	WK_TRACE("HOST: DoRequestSetRelayState (Relay=%d State=%d\n", wRelayID, bOpenClose);
	return SendMessage(TMM_REQUEST_SET_RELAIS_STATE, wRelayID, bOpenClose);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose)
{
//	WK_TRACE("HOST: OnConfirmSetRelayState (Relay=%d State=%d)\n", wRelayID, bOpenClose);

	BOOL bResult = FALSE;

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmSetRelayState(wRelayID, bOpenClose);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetRelayState()
{
//	WK_TRACE("HOST: DoRequestGetRelayState\n");
	return SendMessage(TMM_REQUEST_GET_RELAIS_STATE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetRelayState(DWORD dwRelayStateMask)
{
//	WK_TRACE("HOST: OnConfirmGetRelayState (RelayStateMask=0x%x)\n", dwRelayStateMask);

	BOOL bResult = FALSE;

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmGetRelayState(dwRelayStateMask);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetAlarmState()
{
//	WK_TRACE("HOST: DoRequestGetAlarmState\n");
	return SendMessage(TMM_REQUEST_GET_ALARM_STATE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetAlarmState(DWORD dwAlarmStateMask)
{
//	WK_TRACE("HOST: OnConfirmGetAlarmState (AlarmState=0x%x)\n", dwAlarmStateMask);

	BOOL bResult = FALSE;

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmGetAlarmState(dwAlarmStateMask);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestSetAlarmEdge(DWORD dwAlarmEdge)
{
//	WK_TRACE("HOST: DoRequestSetAlarmEdge (AlarmEdge=0x%x)\n", dwAlarmEdge);
	return SendMessage(TMM_REQUEST_SET_ALARM_EDGE, dwAlarmEdge);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmSetAlarmEdge(DWORD dwAlarmEdge)
{
//	WK_TRACE("HOST: OnConfirmSetAlarmEdge (AlarmEdge=0x%x)\n", dwAlarmEdge);

	BOOL bResult = FALSE;

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmSetAlarmEdge(dwAlarmEdge);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoRequestGetAlarmEdge()
{
//	WK_TRACE("HOST: DoRequestGetAlarmEdge\n");
	return SendMessage(TMM_REQUEST_GET_ALARM_EDGE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmGetAlarmEdge(DWORD dwAlarmEdge)
{
//	WK_TRACE("HOST: OnConfirmGetAlarmEdge (AlarmEdge=0x%x)\n",dwAlarmEdge);

	BOOL bResult = FALSE;

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmGetAlarmEdge(dwAlarmEdge);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmInitComplete()
{
	WK_TRACE("HOST: OnConfirmInitComplete\n");

	m_evTmmInitComplete.SetEvent();

	return TRUE;
}