/////////////////////////////////////////////////////////////////////////////
// PROJECT:		Tashafirmare
// FILE:		$Workfile: CCodec.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CCodec.cpp $
// CHECKIN:		$Date: 14.12.01 13:20 $
// VERSION:		$Revision: 99 $
// LAST CHANGE:	$Modtime: 14.12.01 13:20 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////
#include "tm.h"
#include "CAlarm.h"
#include "CRelay.h"
#include "CVideoIn.h"
#include "CVideoOut.h"
#include "CCodec.h"
#include "CMotionDetection.h"
#include "CPerfMon.h"
#include "CJpeg.h"
#include "CMDPoints.h"

#include <WINNT\tmmanapi.h>
//#include <AppSem.h>

#define HEADER "\nTARGET: TriMedia Video-In to Video-Out Example Program Version 2.0\n"

CCodec* CCodec::m_pThis = NULL;

///////////////////////////////////////////////////////////////////////////////////
CCodec::CCodec()
{
    SetDP();
    DP((HEADER));
    WK_TRACE(HEADER);

	eMode Mode				= modeField;

	switch (Mode)
	{
		case modeFrame:
			m_nWidth				= 720;
			m_nHeight				= 576;
			m_yFieldStride			= 768;
			m_uvFieldStride			= m_yFieldStride>>1;
			m_viYUVMode				= viFULLRES;
			m_bCaptureFrame			= TRUE;
			break;
		case modeField:
			m_nWidth				= 720;
			m_nHeight				= 288;
			m_yFieldStride			= 768;
			m_uvFieldStride			= m_yFieldStride>>1;
			m_viYUVMode				= viFULLRES;
			m_bCaptureFrame			= FALSE;
			break;
		case modeCIF:
			m_nWidth				= 352;
			m_nHeight				= 288;
			m_yFieldStride			= 384;
			m_uvFieldStride			= m_yFieldStride>>1;
			m_viYUVMode				= viHALFRES;
			m_bCaptureFrame			= FALSE;
			break;
	}

	m_voYUVMode				= vo422_COSITED_UNSCALED;

	m_pVideoIn				= NULL;
	m_pVideoOut				= NULL;
	m_pJpeg					= NULL;
	
	m_nMmNum				= 0;
	m_u64FrameCounter		= 0;
	m_pThis					= this;
	m_pJpeg					= new CJpeg;

	m_hDSP					= 0;
	m_hMessageSynch			= 0;
	m_bPause				= FALSE;

	m_pDataPacket			= NULL;
	m_hSMBufferHandle		= 0;
	m_SemNewDataInQueue		= 0;
	m_dwSMBufferSize		= 0;

	m_dwAlarmState			= 0;

	// Zwei Semaphoren anlegen, um den Mainthread mitzuteilen, das die anderen Treads
	// sich beendent haben.
	sm_create("ExBUConf", 0, SM_PRIOR, &m_SemExitBufferUpdateThreadConfirm);
	sm_create("ExDTConf", 0, SM_PRIOR, &m_SemExitDataTransferThreadConfirm);
		  
	// Ein neues Bild liegt zur Bearbeitung bereit
	sm_create("BufferMMReady", 0, SM_PRIOR, &m_SemBufferMMReady);

	strcpy(m_sFilePattern, "d:\\Jpeg1\\J_%06d.jpg\0");

	m_pPerf1	= new CPerfMon("Summe");
	m_pPerf2	= new CPerfMon("MsgLatency");

	for (int nI=0; nI< MAX_DATA_PACKETS; nI++)
		m_pDataQueue[nI] = NULL;

	m_nRIndex = 0;			 
	m_nWIndex = 0;

	memset(&m_ReqSrcSel1, 0, sizeof(m_ReqSrcSel1));
	memset(&m_ReqSrcSel2, 0, sizeof(m_ReqSrcSel2));

	m_bRun = FALSE;
}

///////////////////////////////////////////////////////////////////////////////////
CCodec::~CCodec()
{
	sm_delete(m_SemExitBufferUpdateThreadConfirm);
	sm_delete(m_SemExitDataTransferThreadConfirm);
	sm_delete(m_SemBufferMMReady);		  

	WK_DELETE(m_pJpeg);
	WK_DELETE(m_pPerf2);
	WK_DELETE(m_pPerf1);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::Open()
{
	BOOL		bResult = FALSE;
	TMStatus	Status;

	if (OpenMessageChannel())
	{
		if (OpenDataChannel())
		{
			m_pVideoIn	= new CVideoIn(m_SemBufferMMReady);

			if (m_pVideoIn)
			{
				m_pVideoOut = new CVideoOut;
				if (m_pVideoIn->viOpenAPI())
				{
					if (m_pVideoOut)
					{
						if (m_pVideoOut->voOpenAPI())
						{
							m_pMD = new CMotionDetection;
							
							bResult = DoConfirmInitComplete();
						}
					}
				}
			}
		}
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
void CCodec::Close()
{
    FreeBuffers();

	WK_DELETE(m_pVideoIn);
	WK_DELETE(m_pVideoOut);
	WK_DELETE(m_pMD);

#ifndef TM_DEBUG
	CloseMessageChannel();
	CloseDataChannel();
#endif
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OpenMessageChannel()
{
	BOOL		bResult = FALSE;
	TMStatus	Status	= 0;

    Status = tmmanDSPOpen (0, &m_hDSP);

#ifdef TM_DEBUG
	return TRUE;
#endif
	if (Status == statusSuccess)
	{
		m_hMessageSynch = AppSem_create ( 1 );

		/* reset the event */
		AppSem_p(m_hMessageSynch);

		if (m_hMessageSynch != NULL)
		{
			Status = tmmanMessageCreate (m_hDSP, 
										MESSAGE_NAME, 
										(UInt32)m_hMessageSynch,
										constTMManModuleTargetKernel,
										&m_hMessage);
			if (Status == statusSuccess)
				bResult = TRUE;
			else
				WK_TRACE_ERROR("TARGET: tmmanMessageCreate failed Status[%x]\n", Status );

		}
		else
			WK_TRACE_ERROR("TARGET: AppSem_create failed Status[0x%x]\n", Status);
	}
	else
		WK_TRACE("TARGET: tmmanDSPOpen failed Status[%x]\n", Status);

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::CloseMessageChannel()
{
	if (m_hMessageSynch)
		AppSem_delete(m_hMessageSynch);

	if (m_hMessage)
		tmmanMessageDestroy(m_hMessage); 

	if (m_hDSP)
		tmmanDSPClose(m_hDSP);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OpenDataChannel()
{
	BOOL		bResult	= FALSE;
	BOOL		bAllocErr	= FALSE;
	
	TMStatus	Status	= 0;

	if (m_hDSP)
	{
#ifdef TM_DEBUG
		m_pDataPacket = (DataPacket*)allocSz(SM_BUFFER_SIZE);
		memset(m_pDataPacket, 0, SM_BUFFER_SIZE);
		m_dwSMBufferSize = SM_BUFFER_SIZE;
#else
		Status = tmmanSharedMemoryOpen(m_hDSP, BUFFER_NAME, &m_dwSMBufferSize,
				(UInt32*)&m_pDataPacket, &m_hSMBufferHandle);
#endif
		if (Status  == statusSuccess)
		{
			// Array zur Aufnahme der Datenpackete, bevor sie an die TashaUnit verschickt werden
			for (int nI=0; nI< MAX_DATA_PACKETS; nI++)
			{
				m_pDataQueue[nI] = (DataPacket*)allocSz(m_dwSMBufferSize+sizeof(DataPacket));
				if (m_pDataQueue[nI] == NULL)
					 bAllocErr = TRUE;
			}
			
			if (!bAllocErr)
			{
				// Semaphore zur Signalisierung von neuen Daten
				sm_create("NewData", 0, SM_PRIOR, &m_SemNewDataInQueue);
				bResult = TRUE;
			}
		}
		else
			WK_TRACE_ERROR("TARGET: tmmanSharedMemoryOpen failed (0x%x)\n", Status);
	}
	else
		WK_TRACE_ERROR("TARGET: m_hDSP=NULL\n");

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::CloseDataChannel()
{
#ifdef TM_DEBUG
	if (m_pDataPacket)
		_cache_free((Pointer)m_pDataPacket);
#else
	if (m_hSMBufferHandle)
		tmmanSharedMemoryClose(m_hSMBufferHandle);
#endif

	for (int nI=0; nI< MAX_DATA_PACKETS; nI++)
		_cache_free((Pointer)m_pDataQueue[nI]);

	if (m_SemNewDataInQueue)
		sm_delete(m_SemNewDataInQueue);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestSetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	BOOL bResult = FALSE;

	WK_TRACE("TARGET: OnRequestSetBrightness (UserData=0x%x Source:%d Value=%d)\n", dwUserData, wSource, nValue);

	if (m_pVideoIn)
	{
		if (m_pVideoIn->SetBrightness(wSource, nValue))
			bResult = DoConfirmSetBrightness(dwUserData, wSource, nValue);
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	WK_TRACE("TARGET: DoConfirmSetBrightness (UserData=0x%x Source:%d Value=%d)\n", dwUserData, wSource, nValue);

	return SendMessage(TMM_CONFIRM_SET_BRIGHTNESS, dwUserData, wSource, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestSetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	BOOL bResult = FALSE;

	WK_TRACE("TARGET: OnRequestSetContrast (UserData=0x%x Source:%d Value=%d)\n", dwUserData, wSource, nValue);

	if (m_pVideoIn)
	{
		if (m_pVideoIn->SetContrast(wSource, nValue))
			bResult = DoConfirmSetContrast(dwUserData, wSource, nValue);
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	WK_TRACE("TARGET: DoConfirmSetContrast (UserData=0x%x Source:%d Value=%d)\n", dwUserData, wSource, nValue);

	return SendMessage(TMM_CONFIRM_SET_CONTRAST, dwUserData, wSource, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestSetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	BOOL bResult = FALSE;

	WK_TRACE("TARGET: OnRequestSetSaturation (UserData=0x%x Source:%d Value=%d)\n", dwUserData, wSource, nValue);

	if (m_pVideoIn)
	{
		if (m_pVideoIn->SetSaturation(wSource, nValue))
			bResult = DoConfirmSetSaturation(dwUserData, wSource, nValue);
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	WK_TRACE("TARGET: DoConfirmSetSaturation (UserData=0x%x Source:%d Value=%d)\n", dwUserData, wSource, nValue);

	return SendMessage(TMM_CONFIRM_SET_SATURATION, dwUserData, wSource, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestGetBrightness(DWORD dwUserData, WORD wSource)
{
	BOOL bResult = FALSE;

	WK_TRACE("TARGET: OnRequestGetBrightness (UserData=0x%x Source:%d)\n", dwUserData, wSource);

	if (m_pVideoIn)
	{
		int nValue = 0;
		if (m_pVideoIn->GetBrightness(wSource, nValue))
			bResult = DoConfirmGetBrightness(dwUserData, wSource, nValue);
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	WK_TRACE("TARGET: DoConfirmGetBrightness (UserData=0x%x Source:%d Value=%d)\n", dwUserData, wSource, nValue);
	return SendMessage(TMM_CONFIRM_GET_BRIGHTNESS, dwUserData, wSource, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestGetContrast(DWORD dwUserData, WORD wSource)
{
	BOOL bResult = FALSE;

	WK_TRACE("TARGET: OnRequestGetContrast (UserData=0x%x Source:%d)\n", dwUserData, wSource);

	if (m_pVideoIn)
	{
		int nValue = 0;
		if (m_pVideoIn->GetContrast(wSource, nValue))
			bResult = DoConfirmGetContrast(dwUserData, wSource, nValue);
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	WK_TRACE("TARGET: DoConfirmGetContrast (UserData=0x%x Source:%d Value=%d)\n", dwUserData, wSource, nValue);
	return SendMessage(TMM_CONFIRM_GET_CONTRAST, dwUserData, wSource, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestGetSaturation(DWORD dwUserData, WORD wSource)
{
	BOOL bResult = FALSE;

	WK_TRACE("TARGET: OnRequestGetSaturation (UserData=0x%x Source:%d)\n", dwUserData, wSource);

	if (m_pVideoIn)
	{
		int nValue = 0;
		if (m_pVideoIn->GetSaturation(wSource, nValue))
			bResult = DoConfirmGetSaturation(dwUserData, wSource, nValue);
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	WK_TRACE("TARGET: DoConfirmGetSaturation (UserData=0x%x Source:%d Value=%d)\n", dwUserData, wSource, nValue);
	return SendMessage(TMM_CONFIRM_GET_SATURATION, dwUserData, wSource, nValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestResumeCapture() 
{
	WK_TRACE("TARGET: OnRequestResumeCapture\n");
	m_bPause = FALSE;
	return DoConfirmResumeCapture();
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmResumeCapture()
{
	WK_TRACE("TARGET: DoConfirmResumeCapture\n");
	return SendMessage(TMM_CONFIRM_RESUME_CAPTURE, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestPauseCapture() 
{
	WK_TRACE("TARGET: OnRequestPauseCapture\n");
	m_bPause = TRUE;
	return DoConfirmPauseCapture();
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmPauseCapture() 
{
	WK_TRACE("TARGET: DoConfirmPauseCapture\n");
	
	return SendMessage(TMM_CONFIRM_PAUSE_CAPTURE, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestStartCapture() 
{
	WK_TRACE("TARGET: OnRequestStartCapture\n");
	BOOL bResult = FALSE;

	if (StartCapture())
		bResult = DoConfirmStartCapture();

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmStartCapture() 
{
	WK_TRACE("TARGET: DoConfirmStartCapture\n");
	return SendMessage(TMM_CONFIRM_START_CAPTURE, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestStopCapture() 
{
	WK_TRACE("TARGET: OnRequestStopCapture\n");
	BOOL bResult = FALSE;

	if (StopCapture())
		bResult = DoConfirmStopCapture();
	
	m_bPause = FALSE;

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmStopCapture() 
{
	WK_TRACE("TARGET: DoConfirmStopCapture\n");
	return SendMessage(TMM_CONFIRM_STOP_CAPTURE, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestScanForCameras(DWORD dwUserData)
{
	BOOL bResult = FALSE;

	WK_TRACE("TARGET: OnRequestScanForCameras (UserData:0x%x)\n", dwUserData);

	// ToDo Scanning not yet implemented
	return DoConfirmScanForCameras(dwUserData, 0x0f);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask)
{
	WK_TRACE("TARGET: DoConfirmScanForCameras\n");
	return SendMessage(TMM_CONFIRM_SCAN_FOR_CAMERAS, dwUserData, dwCameraMask);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestSetRelayState(WORD wRelayID, BOOL bOpenClose)
{
	WK_TRACE("TARGET: OnRequestSetRelayState\n");


	DWORD dwRelayStateMask = m_Relay.GetRelay();
	if (bOpenClose)
		dwRelayStateMask = SETBIT(dwRelayStateMask, wRelayID);
	else
		dwRelayStateMask = CLRBIT(dwRelayStateMask, wRelayID);

	m_Relay.SetRelay(dwRelayStateMask);
		
	return DoConfirmSetRelayState(wRelayID, bOpenClose);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose)
{
	WK_TRACE("TARGET: DoConfirmSetRelayState (Relay=%d State=%d)\n", wRelayID, bOpenClose);
	
	return SendMessage(TMM_CONFIRM_SET_RELAIS_STATE, wRelayID, bOpenClose);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestGetRelayState()
{
	WK_TRACE("TARGET: OnRequestGetRelayState\n");

	return DoConfirmGetRelayState(m_Relay.GetRelay());
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmGetRelayState(DWORD dwRelayStateMask)
{
	WK_TRACE("TARGET: DoConfirmGetRelayState (RelayMask=0x%x)\n", dwRelayStateMask);
	
	return SendMessage(TMM_CONFIRM_GET_RELAIS_STATE, dwRelayStateMask);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestGetAlarmState()
{
	WK_TRACE("TARGET: OnRequestGetAlarmState\n");
	
	return DoConfirmGetAlarmState(m_Alarm.GetState());
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmGetAlarmState(DWORD dwAlarmStateMask)
{
	WK_TRACE("TARGET: DoConfirmGetAlarmState (RelayMask=0x%x)\n", dwAlarmStateMask);
	return SendMessage(TMM_CONFIRM_GET_ALARM_STATE, dwAlarmStateMask);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestSetAlarmEdge(DWORD dwAlarmEdgeMask)
{
	WK_TRACE("TARGET: OnRequestSetAlarmEdge\n");

	m_Alarm.SetEdge(dwAlarmEdgeMask);

	return DoConfirmSetAlarmEdge(dwAlarmEdgeMask);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmSetAlarmEdge(DWORD dwAlarmEdgeMask)
{
	WK_TRACE("TARGET: DoConfirmSetAlarmEdge (EdgeMask=0x%x)\n", dwAlarmEdgeMask);
	return SendMessage(TMM_CONFIRM_SET_ALARM_EDGE, dwAlarmEdgeMask);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestGetAlarmEdge()
{
	WK_TRACE("TARGET: OnRequestGetAlarmEdge\n");

	return DoConfirmGetAlarmEdge(m_Alarm.GetEdge());
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmGetAlarmEdge(DWORD dwAlarmEdgeMask)
{
	WK_TRACE("TARGET: DoConfirmGetAlarmEdge (EdgeMask=0x%x)\n", dwAlarmEdgeMask);
	return SendMessage(TMM_CONFIRM_GET_ALARM_EDGE, dwAlarmEdgeMask);
}

///////////////////////////////////////////////////////////////////////////////////
void CCodec::Run()
{														      
	if (AllocBuffers())
	{
		if (Open())
		{
			UInt32 dwStart = clock();
			CInt64	u64Start(CYCLES(),HICYCLES());	
			
			// Capturing starten
	//		OnRequestStartCapture();
			OnRequestStopCapture();

		#ifdef TM_DEBUG
			// Mainthread wartet, bis das Demo fertig ist
			WK_TRACE("TARGET: Press RETURN to exit\n");
			getchar();
		#else
			MessageLoop();
		#endif
			
			// Capturing stopen
			OnRequestStopCapture();
			CInt64	u64Stop(CYCLES(),HICYCLES());	
			
			CInt64 Time(u64Stop - u64Start);
			Time /= CLOCK_SPEED;
			WK_TRACE("TARGET: fps=%s\n", (LPCSTR)(m_u64FrameCounter / Time));

			Close();
		}
	}
}	      

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::StartCapture()
{	
	if (!m_bRun)
	{
		m_nMmNum = 0;
		
		m_bRun = TRUE;

		// Bufferhandling-Thread und Datatransfer-Thread anlegen und starten...
		t_create("BufferUpdate", 4, 10000, 10000, 0, &m_taskBufferUpdate);
		t_create("DataTransfer", 4, 10000, 10000, 0, &m_taskDataTransfer);
		t_start(m_taskDataTransfer, T_PREEMPT | T_TSLICE | T_ASR | T_ISR, DataTransferThread, 0);
		t_start(m_taskBufferUpdate, T_PREEMPT | T_TSLICE | T_ASR | T_ISR, mmBufUpdateThread,  0);
		
		// Buffer für die VideoInUnit freigeben
		for (int nI = 0; nI < NUM_BUF_ENTRIES; nI++)
			m_genBuf[nI].flag = VID_RDY_VI;
		
		// VideoInUnit starten
		if (m_pVideoIn)
			m_pVideoIn->viYUVAPI(m_viYUVMode, m_bCaptureFrame, &m_genBuf[0], 0, 21);

		// VideoOutUnit starten
		if (m_pVideoOut)			   
			m_pVideoOut->voYUVAPI(m_voYUVMode, &m_genBuf[0], 0, 0);
	
	}

	return TRUE;
}	      

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::StopCapture()
{														    
	if (m_bRun)
	{
		// Threads sollen sich beenden
		m_bRun = FALSE;

		// Warte bis sich der BufferUpdate-Thread beendet hat
		sm_p(m_SemExitBufferUpdateThreadConfirm, SM_WAIT, 0);

		// Warte bis sich der DataTransfer-Thread beendet hat
		sm_p(m_SemExitDataTransferThreadConfirm, SM_WAIT, 0);

		// BufferUpdate-Thread und DataTransfer-Thread zerstören
		t_delete(m_taskBufferUpdate);
		t_delete(m_taskDataTransfer);
		
		WK_TRACE("TARGET: Ok...all Threads stoped\n");

		// VideoInUnit stoppen
		if (m_pVideoIn)
			m_pVideoIn->viStop();
        
		// VideoOutUnit stoppen
		if (m_pVideoOut)
			m_pVideoOut->voStop();
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
void CCodec::mmBufUpdateThread()
{
    Int	mmtmpNum = 0;

	WK_TRACE("TARGET: Start Bufferhandling thread\n");
	
	while (m_pThis->m_bRun)
	{
		if (!m_pThis->m_bPause)
		{
			// Liegt ein neues Bild von der VideoIn-Unit vor?
			sm_p(m_pThis->m_SemBufferMMReady, SM_WAIT, 100);

			// Nächsten Buffer holen
			mmtmpNum = (m_pThis->m_nMmNum + 1) % NUM_BUF_ENTRIES;
    
			// Ist dieser Buffer bereit zur Bearbeitung?
			if (m_pThis->m_genBuf[mmtmpNum].flag == VID_RDY_MM)
			{
				// Kamera umschalten
				m_pThis->SetInputSource();
				m_pThis->m_genBuf[m_pThis->m_nMmNum].dwProzessID	= m_pThis->m_ReqSrcSel1.dwProzessID;
				m_pThis->m_genBuf[m_pThis->m_nMmNum].wSource		= m_pThis->m_ReqSrcSel1.wSource;

				// Hier erfolgt die Kompression und alles andere was mit dem Frame gemacht werden soll
				m_pThis->OnCaptureNewFrame(&m_pThis->m_genBuf[mmtmpNum]);

				// Buffer darf neu geschrieben werden.
				m_pThis->m_genBuf[m_pThis->m_nMmNum].flag			= VID_RDY_VI; /*VID_RDY_VO;*/

				// Merke dir den neuen VideoIn Buffer index
				m_pThis->m_nMmNum = mmtmpNum;
			}
		}
	}

	WK_TRACE("TARGET: Exit Bufferhandling thread\n");

	// Semaphore setzen -> Exit Confirm
	sm_v(m_pThis->m_SemExitBufferUpdateThreadConfirm);
}

///////////////////////////////////////////////////////////////////////////////////
void CCodec::OnCaptureNewFrame(const exVideoBuffer* pExVideoBuffer)
{
	// Ein wenig mit den Bildaten 'spielen'
//	DemoFilter(pExVideoBuffer);
	
	m_pPerf1->Start();

	// Motion detection durchführen
	CMDPoints Points;
	if (m_pMD->MotionCheck(0, pExVideoBuffer, Points))
	{
/*		int nXCross = Points.GetPointAt(0).m_cx * m_nWidth / 1000;
		int nYCross = Points.GetPointAt(0).m_cy * m_nHeight / 1000;
		
		UInt8* pY = pExVideoBuffer->videoBuffer.Y + nYCross * m_yFieldStride;
		for (int nX = 0; nX < m_nWidth; nX++)
			*pY++ = 255;
		
		pY = pExVideoBuffer->videoBuffer.Y + nXCross;
		for (int nY = 0; nY < m_nHeight; nY++, pY+=m_yFieldStride)
			*pY = 255;
*/	}

	// Daten komprimieren und in die PacketQueue hängen
	if (m_pJpeg->Compress(m_bCaptureFrame, pExVideoBuffer, Points, m_pDataQueue[m_nWIndex], m_dwSMBufferSize))
	{
		m_nWIndex++;
		m_nWIndex %= MAX_DATA_PACKETS;

		// Datatransfer thread melden, das ein neues Bild in der Queue steht.
		sm_v(m_SemNewDataInQueue);
	}

	m_pPerf1->Stop();

	m_u64FrameCounter++;
}

///////////////////////////////////////////////////////////////////////////////////
void CCodec::DataTransferThread()
{
	WK_TRACE("TARGET: Start DataTransferThread\n");
	
	while (m_pThis->m_bRun)
	{
		// Liegen neue Daten in der Queue?
		sm_p(m_pThis->m_SemNewDataInQueue, SM_WAIT, 10);
		while (m_pThis->m_bRun && m_pThis->m_pDataQueue[m_pThis->m_nRIndex]->bValid)
		{
			// Packet aus der Queue holen und zur TashaUnit schicken.
			m_pThis->DoReceiveNewCodecData();
		}
	
		// Alarmeingänge regelmäßig überwachen
		m_pThis->PollAlarm();
	}

	WK_TRACE("TARGET: Exit Datahandling thread\n");

	// Semaphore setzen -> Exit Confirm
	sm_v(m_pThis->m_SemExitDataTransferThreadConfirm);

}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoReceiveNewCodecData()
{
	// WK_TRACE("TARGET: DoReceiveNewCodecData\n");

	BOOL	bResult = FALSE;

#if (0)
	// Status der PacketQueue zur Kontrolle ausgeben
	int nCount = 0;
	for (int nI = 0; nI < MAX_DATA_PACKETS; nI++)
	{
		if (m_pThis->m_pDataQueue[nI]->bValid == TRUE)
			nCount++;
	}

	if (nCount > 1)
	{
		WK_TRACE("TARGET: Queue=%d\n", nCount);
		for (int nI = 0; nI < MAX_DATA_PACKETS; nI++)
		{
			if (m_pThis->m_pDataQueue[nI]->bValid == TRUE)
				WK_TRACE("1");
			else
				WK_TRACE("0");
		}
		WK_TRACE("\n");
	}
#endif

	DataPacket* pQueue  = m_pDataQueue[m_nRIndex];
	DataPacket* pPacket = m_pDataPacket;
	
	// Steht der SharedMemory Buffer zur verfügung?
	if (pPacket && (pPacket->bValid == FALSE) && (pQueue->bValid == TRUE))
	{
		memset(pPacket, 0, sizeof(DataPacket));
		pPacket->eType		= pQueue->eType;
		pPacket->eSubType	= pQueue->eSubType;
		pPacket->wSource	= pQueue->wSource;
		pPacket->bMotion	= pQueue->bMotion;
		
		// Die 5 größten Differenzen zurückliefern
		for (int nI = 0; nI < 5 ; nI++)
		{
			pPacket->Point[nI].cx		= pQueue->Point[nI].cx;
			pPacket->Point[nI].cy		= pQueue->Point[nI].cy;
			pPacket->Point[nI].nValue	= pQueue->Point[nI].nValue;
		}
		pPacket->TimeStamp.wDay		= pQueue->TimeStamp.wDay;
		pPacket->TimeStamp.wMonth	= pQueue->TimeStamp.wMonth;
		pPacket->TimeStamp.wYear	= pQueue->TimeStamp.wYear;
		pPacket->TimeStamp.wHour	= pQueue->TimeStamp.wHour;
		pPacket->TimeStamp.wMinute	= pQueue->TimeStamp.wMinute;
		pPacket->TimeStamp.wSecond	= pQueue->TimeStamp.wSecond;
		pPacket->TimeStamp.wMSecond	= pQueue->TimeStamp.wMSecond;
		pPacket->TimeStamp.bValid	= pQueue->TimeStamp.bValid;
		pPacket->dwProzessID		= pQueue->dwProzessID;
		pPacket->dwDataLen			= pQueue->dwDataLen;
		
		memcpy(&pPacket->dwData, &pQueue->dwData, pQueue->dwDataLen);
		pPacket->bValid			= TRUE;

		m_pPerf2->Start();
		if (SendMessage(TMM_NEW_CODEC_DATA))
		{
			// WK_TRACE("TARGET: SendMessage (R=%d W=%d Flag=%d) ok\n", m_pThis->m_nRIndex, m_pThis->m_nWIndex, pQueue->bValid);
			// Packet in der Queue freigenen
			pQueue->bValid = FALSE;
			m_nRIndex++;
			m_nRIndex %= MAX_DATA_PACKETS;
			bResult = TRUE;
		}
		else
		{
			WK_TRACE_WARNING("TARGET: SendMessage busy\n");
			AppModel_sleep(1);
		}
	}		
	else
	{
		WK_TRACE_WARNING("TARGET: Shared memory busy\n");
		AppModel_sleep(1);
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnConfirmReceiveNewCodecData()
{
//	WK_TRACE("TARGET: OnConfirmReceiveNewCodecData\n");

	// Buffer wieder freigeben, könnte auch schon im Host geschehen, so ist's aber schöner
	m_pDataPacket->bValid	= FALSE;
	m_pPerf2->Stop();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::PollAlarm()
{
	DWORD dwCurrentAlarmState	= 0;
	BOOL  bResult = FALSE;

	// CurrentAlarmstatusregister lesen.
	dwCurrentAlarmState = m_Alarm.GetState();

	if (dwCurrentAlarmState != m_dwAlarmState)
	{										  
		m_dwAlarmState = dwCurrentAlarmState;
					
		bResult = DoNotifyAlarm(dwCurrentAlarmState);
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoNotifyAlarm(DWORD dwAlarmState)
{
	WK_TRACE("DoNotifyAlarm (0x%x)\n", dwAlarmState);
	return SendMessage(TMM_NOTIFY_ALARM, dwAlarmState);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestSetInputSource(DWORD dwParam1, DWORD dwParam2)
{
	BOOL bResult		= FALSE;
	TMStatus	Status	= 0;

	DWORD dwSrcSel1Size = 0;
	DWORD dwSrcSel2Size = 0;
	SOURCE_SELECTION*	pSrcSel1	= NULL;
	SOURCE_SELECTION*	pSrcSel2	= NULL;
	UInt32 h_SrcSel1 = 0;
	UInt32 h_SrcSel2 = 0;

	Status = tmmanSharedMemoryOpen(m_hDSP, SRC_SEL1, &dwSrcSel1Size, (UInt32*)&pSrcSel1, &h_SrcSel1);
	if (Status  == statusSuccess)
	{
		Status = tmmanSharedMemoryOpen(m_hDSP, SRC_SEL2, &dwSrcSel2Size, (UInt32*)&pSrcSel2, &h_SrcSel2);
		if (Status  == statusSuccess)
		{
			memcpy(&m_ReqSrcSel1, pSrcSel1, sizeof(SOURCE_SELECTION));
			memcpy(&m_ReqSrcSel2, pSrcSel2, sizeof(SOURCE_SELECTION));
			WK_TRACE("TARGET: OnRequestSetInputSource (Source=%d, ProcessID=0x%x)\n", m_ReqSrcSel1.wSource, m_ReqSrcSel1.dwProzessID);
			tmmanSharedMemoryClose(h_SrcSel1);
		}
		tmmanSharedMemoryClose(h_SrcSel2);
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmSetInputSource(DWORD dwProcessID, WORD wSource)
{
	WK_TRACE("TARGET: DoConfirmSetInputSource: Source=%d ProzessID=0x%x\n", wSource, dwProcessID);

	return SendMessage(TMM_CONFIRM_SET_INPUT_SOURCE, wSource , dwProcessID);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::OnRequestGetInputSource()
{
	WK_TRACE("TARGET: OnRequestGetInputSource\n");

	BOOL bResult = FALSE;
	WORD wSource = VIDEO_SOURCE_NO;

	if (m_pVideoIn)
	{
		if (m_pVideoIn->GetInputSource(wSource))
		{
			bResult = DoConfirmGetInputSource(wSource);
		}
	}

	return TRUE;
}											 	  

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmGetInputSource(WORD wSource)
{
	WK_TRACE("TARGET: DoConfirmGetInputSource Source=%d\n", wSource);

	return SendMessage(TMM_CONFIRM_GET_INPUT_SOURCE, wSource);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetInputSource()
{
	BOOL bResult = FALSE;

	// Soll die Kamera gewechselt werden?
	if (m_ReqSrcSel1.bValid)
	{
		if (m_pVideoIn)
		{
			if (m_pVideoIn->SetInputSource(m_ReqSrcSel1))
			{
				m_ReqSrcSel1.bValid = FALSE;
				bResult = DoConfirmSetInputSource(m_ReqSrcSel1.dwProzessID, m_ReqSrcSel1.wSource);
			}
		}
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::DoConfirmInitComplete()
{
	WK_TRACE("TARGET: DoConfirmInitComplete\n");

	return SendMessage(TMM_CONFIRM_INIT_COMPLETE);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::AllocBuffers()
{
    Int         szY, szUV;
	BOOL	bResult = TRUE;

    szY		= (m_yFieldStride  * (m_nHeight + 4));
    szUV	= (m_uvFieldStride * (m_nHeight + 4));
    
	for (int nI = 0; nI < NUM_BUF_ENTRIES; nI++)
	{
        m_genBuf[nI].videoBuffer.Y			= allocSz(szY);
        m_genBuf[nI].videoBuffer.U			= allocSz(szUV);
        m_genBuf[nI].videoBuffer.V			= allocSz(szUV);
		m_genBuf[nI].videoBuffer.nWidth		= m_nWidth;
		m_genBuf[nI].videoBuffer.nHeight	= m_nHeight;
		m_genBuf[nI].videoBuffer.yPitch		= m_yFieldStride;
		m_genBuf[nI].videoBuffer.uvPitch	= m_uvFieldStride;
		m_genBuf[nI].flag					= VID_RDY_VI;
		m_genBuf[nI].wSource				= 0;
		m_genBuf[nI].dwProzessID			= 0;
		if ((m_genBuf[nI].videoBuffer.Y == NULL) ||
			(m_genBuf[nI].videoBuffer.U == NULL) ||
			(m_genBuf[nI].videoBuffer.V == NULL))
			 bResult = FALSE;
	}

	return bResult;
}
///////////////////////////////////////////////////////////////////////////////////
void CCodec::FreeBuffers()
{
    for (int nI = 0; nI < NUM_BUF_ENTRIES; nI++)
	{
        _cache_free((Pointer) m_genBuf[nI].videoBuffer.Y);
        _cache_free((Pointer) m_genBuf[nI].videoBuffer.U);
        _cache_free((Pointer) m_genBuf[nI].videoBuffer.V);
    }
}

///////////////////////////////////////////////////////////////////////////////////
UInt8* CCodec::allocSz(int bufSz)
{
    UInt8*      temp	= NULL;

    if ((temp = (UInt8*) _cache_malloc(bufSz, -1)) != NULL)
	{
	    memset((Pointer)temp, 0, bufSz);
		_cache_copyback((Pointer)temp, bufSz);
	}
	else
        WK_TRACE_ERROR("TARGET: _cache_malloc failed\n");

    return temp;
}

///////////////////////////////////////////////////////////////////////////////////
void CCodec::SetDP()
{
    DPsize(32000);  /* for debugging */
}

///////////////////////////////////////////////////////////////////////////////////
void CCodec::DemoFilter(const exVideoBuffer* pExVideoBuffer)
{
	UInt8*	pYBase	= pExVideoBuffer->videoBuffer.Y;
	UInt8*	pUBase	= pExVideoBuffer->videoBuffer.U;
	UInt8*	pVBase	= pExVideoBuffer->videoBuffer.V;

	static  int nXStart = 100;
	static  int nYStart = 100;
	static	int	nXInc	= 3;
	static	int	nYInc	= 3;
	int		nWidth		= 100; //400;
	int		nHeight		= 100; //300;
	
	if (nXStart + nWidth > m_nWidth)
		nXInc = -3;
	if (nXStart < 0)
		nXInc = 3;

	if (nYStart + nHeight > m_nHeight)
		nYInc = -3;
	if (nYStart < 0)
		nYInc = 3;

	nXStart += nXInc;
	nYStart += nYInc;

	for (int nY = nYStart; nY < nYStart + nHeight; nY++)
	{
		UInt8* pY = pYBase + nXStart   + nY * m_yFieldStride;
		UInt8* pU = pUBase + nXStart/2 + (nY * m_uvFieldStride);
		UInt8* pV = pVBase + nXStart/2 + (nY * m_uvFieldStride);

		for (int nX = nXStart; nX < nXStart + nWidth; nX+=2)
		{
			*pY	= *pY>>1;
			pY++;
			*pY	= *pY>>1;
			pY++;
			*pU++	= 127;
			*pV++	= 127;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
void CCodec::MessageLoop()
{
	int		nMessageID	= 0;
	DWORD	dwParam1	= 0;
	DWORD	dwParam2	= 0;
	DWORD	dwParam3	= 0;
	DWORD	dwParam4	= 0;
	do
	{
		// Warte auf nächste Message
		AppSem_p(m_hMessageSynch);

		while (ReceiveMessage(nMessageID, dwParam1, dwParam2, dwParam3, dwParam4))
		{
			//WK_TRACE("TARGET: MessageID=%d Param1=%d Param2=%d\n", nMessageID, dwParam1, dwParam2);
			switch (nMessageID)
			{
				case TMM_REQUEST_RESUME_CAPTURE:
					OnRequestResumeCapture();
					break;
				case TMM_REQUEST_PAUSE_CAPTURE:
					OnRequestPauseCapture();
					break;
				case TMM_REQUEST_START_CAPTURE:
					OnRequestStartCapture();
					break;
				case TMM_REQUEST_STOP_CAPTURE:
					OnRequestStopCapture();
					break;
				case TMM_REQUEST_SET_BRIGHTNESS:
					OnRequestSetBrightness(dwParam1, (WORD)dwParam2, (int)dwParam3);
					break;
				case TMM_REQUEST_SET_CONTRAST:
					OnRequestSetContrast(dwParam1, (WORD)dwParam2, (int)dwParam3);
					break;
				case TMM_REQUEST_SET_SATURATION:
					OnRequestSetSaturation(dwParam1, (WORD)dwParam2, (int)dwParam3);
					break;
				case TMM_REQUEST_GET_BRIGHTNESS:
					OnRequestGetBrightness(dwParam1, (WORD)dwParam2);
					break;
				case TMM_REQUEST_GET_CONTRAST:
					OnRequestGetContrast(dwParam1, (WORD)dwParam2);
					break;
				case TMM_REQUEST_GET_SATURATION:
					OnRequestGetSaturation(dwParam1, (WORD)dwParam2);
					break;
				case TMM_CONFIRM_NEW_CODEC_DATA:
					OnConfirmReceiveNewCodecData();
					break;
				case TMM_REQUEST_SET_INPUT_SOURCE:
					OnRequestSetInputSource(dwParam1, dwParam1);
					break;
				case TMM_REQUEST_GET_INPUT_SOURCE:
					OnRequestGetInputSource();
					break;
				case TMM_REQUEST_SCAN_FOR_CAMERAS:
					OnRequestScanForCameras(dwParam1);
					break;
				case TMM_REQUEST_SET_RELAIS_STATE:
					OnRequestSetRelayState((WORD)dwParam1, (BOOL)dwParam2);
					break;
				case TMM_REQUEST_GET_RELAIS_STATE:
					OnRequestGetRelayState();
					break;
				case TMM_REQUEST_GET_ALARM_STATE:
					OnRequestGetAlarmState();
					break;
				case TMM_REQUEST_SET_ALARM_EDGE:
					OnRequestSetAlarmEdge(dwParam1);
					break;
				case TMM_REQUEST_GET_ALARM_EDGE:
					OnRequestGetAlarmEdge();
					break;				
			}
		}
	}while (nMessageID != TMM_QUIT);

	// Host MessageLoop Thread terminieren.
	SendMessage(TMM_QUIT, 0, 0);
	AppModel_sleep(10);
}

///////////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SendMessage(int nMessageID, DWORD dwParam1/*=0*/, DWORD dwParam2/*=0*/, DWORD dwParam3/*=0*/, DWORD dwParam4/*=0*/)
{
	TMStatus	Status;
	BOOL bResult = FALSE;

	tmmanPacket	Packet;
	memset(&Packet, 0, sizeof(Packet));
#ifdef TM_DEBUG
	return TRUE;
#endif

	if (m_hMessage)
	{
		Packet.Argument[0] = nMessageID;
		Packet.Argument[1] = dwParam1;
		Packet.Argument[2] = dwParam2;
		Packet.Argument[3] = dwParam3;
		Packet.Argument[4] = dwParam4;

		Status = tmmanMessageSend(m_hMessage, &Packet);

		if (Status == statusSuccess)
			bResult = TRUE;
		else
			WK_TRACE_ERROR("TARGET: tmmanMessageSend failed Status[%x]\n", Status);
	}
	else
		WK_TRACE_ERROR("TARGET: Message channel not open\n");

	return bResult;

}


