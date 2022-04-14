/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CipcOutputTashaUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/CipcOutputTashaUnit.cpp $
// CHECKIN:		$Date: 3.02.05 16:45 $
// VERSION:		$Revision: 74 $
// LAST CHANGE:	$Modtime: 3.02.05 16:34 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TashaUnitDlg.h"
#include "CIPCOutputTashaUnit.h"
#include "CIPCExtraMemory.h"
#include "CIPCStringDefs.h"
#include "VideoJob.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CWK_Timer	theTimer;

CIPCOutputTashaUnit::CIPCOutputTashaUnit(CTashaUnitDlg* pMainWnd, CCodec *pCodec,
							 int iType, LPCTSTR shmName) : CIPCOutput(shmName, FALSE)
{
	m_pCodec				= pCodec;						// Pointer auf CTasha-Objekt
	m_iType					= iType;						// OUTPUT_CAMERA, OUTPUT_RELAY
	m_wGroupID				= SECID_NO_GROUPID;				// Noch keine GroupID
	m_bOK					= TRUE;							// TRUE->Objekt ok
	m_eImageRes				= eImageResUnvalid;
	m_pMainWnd				= pMainWnd;
	m_dwCrossPointMask		= 0;							// Gibt den Zustand der analogen Ausgänge an
	m_byTermMask			= 0;
	m_dwOnCommandSendErrorCounter = 0;

	m_lpBMI = (LPBITMAPINFO)new BYTE[MAX_HSIZE*MAX_VSIZE*sizeof(WORD)+sizeof(BITMAPINFO)+1024];

	for (int nCamID = 0; nCamID < MAX_CAMERAS; nCamID++)
	{
		for (int nStreamID = 0; nStreamID < MAX_STREAMS; nStreamID++)
		{
			m_VideoStream[nStreamID][nCamID].dwUserData					= (DWORD)-1;
			m_VideoStream[nStreamID][nCamID].bEncoderState				= FALSE;
			m_VideoStream[nStreamID][nCamID].Compression				= COMPRESSION_NONE;
			m_VideoStream[nStreamID][nCamID].Resolution					= RESOLUTION_NONE;
			m_VideoStream[nStreamID][nCamID].Ct							= COMPRESSION_UNKNOWN;
			m_VideoStream[nStreamID][nCamID].EncParam.eRes				= eImageResUnvalid;
			m_VideoStream[nStreamID][nCamID].EncParam.eType				= eTypeUnvalid;
			m_VideoStream[nStreamID][nCamID].EncParam.nStreamID			= nStreamID;
			m_VideoStream[nStreamID][nCamID].EncParam.nBitrate			= 0;
			m_VideoStream[nStreamID][nCamID].EncParam.nIFrameIntervall	= 0;
			m_VideoStream[nStreamID][nCamID].EncParam.nFps				= 0;
		}
	}

	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputTashaUnit::~CIPCOutputTashaUnit()
{
	// TRACE(_T("CIPCOutputTashaUnit::~CIPCOutputTashaUnit\n"));
	m_bOK	= FALSE;

	StopThread();

	WK_DELETE(m_lpBMI);
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputTashaUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestGetValue(CSecID CId, const CString &sKey, DWORD dwUserData)
{
//	CString sValue = _T("");

	if (CId.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputTashaUnit::OnRequestGetValue\tWrong GroupID\n"));
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	WORD wSource = CId.GetSubID(); // Kameranummer 0...7

	if (sKey == CSD_BRIGHTNESS)
	{
		if (m_pCodec)
			m_pCodec->DoRequestGetBrightness(dwUserData, wSource);
	}
	else if (sKey == CSD_CONTRAST)
	{
		if (m_pCodec)
			m_pCodec->DoRequestGetContrast(dwUserData, wSource);
	}
	else if (sKey == CSD_SATURATION)
	{
		if (m_pCodec)
			m_pCodec->DoRequestGetSaturation(dwUserData, wSource);
	}
	else if (sKey == CSD_BRIGHTNESS_PERCENT)
	{
		if (m_pCodec)
			m_pCodec->DoRequestGetBrightness(dwUserData, wSource);
	}
	else if (sKey == CSD_CONTRAST_PERCENT)
	{
		if (m_pCodec)
			m_pCodec->DoRequestGetContrast(dwUserData, wSource);
	}
	else if (sKey == CSD_SATURATION_PERCENT)
	{
		if (m_pCodec)
			m_pCodec->DoRequestGetSaturation(dwUserData, wSource);
	}
	else if (sKey == CSD_SCAN_FOR_CAMERAS)
	{
		if (m_pCodec)
			m_pCodec->DoRequestScanForCameras(dwUserData); 
	}
	else if (sKey == CSD_MD_MASK_SENSITIVITY)
	{
		if (m_pCodec)
			m_pCodec->DoRequestGetMDMaskSensitivity(dwUserData, wSource);
	}
	else if (sKey == CSD_MD_ALARM_SENSITIVITY)
	{
		if (m_pCodec)
			m_pCodec->DoRequestGetMDAlarmSensitivity(dwUserData, wSource); 
	}
	else if (sKey == CSD_RESOLUTION)
	{
		CString sValue;
		sValue.Format(_T("%d"), (int)m_VideoStream[0][wSource].Ct);
		DoConfirmGetValue(CId, CSD_RESOLUTION, sValue, dwUserData);

	}
	else if (sKey == CSD_TERM)
	{
		if (m_pCodec)
		{
			m_evGetTermMask.ResetEvent();
			m_pCodec->DoRequestGetTerminationState();
			if (WaitForSingleObject(m_evGetTermMask, 1000) == WAIT_OBJECT_0)
			{
				CString sValue;
				sValue = (TSTBIT(m_byTermMask, wSource) ? CSD_ON : CSD_OFF);
				DoConfirmGetValue(CId, CSD_TERM, sValue, dwUserData);
			}
			else
				WK_TRACE_WARNING(_T("DoRequestGetTerminationState timeout\n"));
		}
	}

}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestSetValue(CSecID CId, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	if (CId.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputTashaUnit::OnRequestSetValue\tWrong GroupID\n"));
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	WORD wSource = CId.GetSubID(); // Kameranummer 0...7

	if (sKey == CSD_BRIGHTNESS)
	{
		if (m_pCodec)
		{
			int nValue = _ttoi(sValue);
			nValue = (int)((double)nValue * 100.0 / (double)MAX_BRIGHTNESS + 0.5);
			m_pCodec->DoRequestSetBrightness(dwUserData, wSource, nValue);
		}
	}
	else if (sKey == CSD_CONTRAST)
	{
		if (m_pCodec)
		{
			int nValue = _ttoi(sValue);
			nValue = (int)((double)nValue * 100.0 / (double)MAX_CONTRAST + 0.5);
			m_pCodec->DoRequestSetContrast(dwUserData, wSource, nValue);
		}
	}
	else if (sKey == CSD_SATURATION)
	{
		if (m_pCodec)
		{
			int nValue = _ttoi(sValue);
			nValue = (int)((double)nValue * 100.0 / (double)MAX_SATURATION + 0.5);
			m_pCodec->DoRequestSetSaturation(dwUserData, wSource, nValue);
		}
	}
	else if (sKey == CSD_BRIGHTNESS_PERCENT)
	{
		if (m_pCodec)
			m_pCodec->DoRequestSetBrightness(dwUserData, wSource, _ttoi(sValue));
	}
	else if (sKey == CSD_CONTRAST_PERCENT)
	{
		if (m_pCodec)
			m_pCodec->DoRequestSetContrast(dwUserData, wSource, _ttoi(sValue));
	}
	else if (sKey == CSD_SATURATION_PERCENT)
	{
		if (m_pCodec)
			m_pCodec->DoRequestSetSaturation(dwUserData, wSource, _ttoi(sValue));
	}
/*	else if (sKey == CSD_MD)
	{ // TODO ist evtl. Überflüssig, da die Bewegungserkennung auch bei UVV-Kassen aktiv sein darf.
		if (m_pCodec)
		{
			if (sValue == CSD_ON)
				m_pCodec->DoRequestSetMDAlarmSensitivity(dwUserData, wSource, CSD_NORMAL);	// TODO: CSD_NORMAL als default?
			else if (sValue == CSD_OFF)
				m_pCodec->DoRequestSetMDAlarmSensitivity(dwUserData, wSource, CSD_OFF);
		}
	}
*/	else if (sKey == CSD_MD_MASK_SENSITIVITY)
	{
		if (m_pCodec)
			m_pCodec->DoRequestSetMDMaskSensitivity(dwUserData, wSource, sValue);
	}
	else if (sKey == CSD_MD_ALARM_SENSITIVITY)
	{
		if (m_pCodec)
			m_pCodec->DoRequestSetMDAlarmSensitivity(dwUserData, wSource, sValue);
	}
	else if (sKey == CSD_V_OUT)
	{
		if (m_pCodec)
		{
			int nIn		= wSource;
			int nOut	= 0;
			// Irgendwo ist die Zuordnung der Videoausgänge gedreht. Dies wird hier korrigiert
			// CSD_PORT0 <- 2. Videoausgang laut Panel
			// CSD_PORT1 <- 1. Videoausgang laut Panel
			if (sValue == CSD_PORT0)
				nOut = 1;
			else if (sValue == CSD_PORT1)
				nOut = 0;
			else if (sValue == CSD_PORT2)	// TODO: Not use
				nOut = 2;
			else if (sValue == CSD_PORT3)	// TODO: Not used
				nOut = 3;

			m_dwCrossPointMask = m_dwCrossPointMask &~ (0xff<<(8*nOut));
			m_dwCrossPointMask |= nIn<<(8*nOut);
			m_pCodec->DoRequestSetAnalogOut(m_dwCrossPointMask);
		}
	}
	else if (sKey == CSD_LED1_ON)
	{
		if (m_pCodec)
			m_pCodec->DoRequestSetPowerLED(FALSE);	
	}
	else if (sKey == CSD_LED1_BLINK)
	{
		if (m_pCodec)
			m_pCodec->DoRequestSetPowerLED(TRUE);	
	}
	else if (sKey == CSD_LED2_ON)
	{
		if (m_pCodec)
			m_pCodec->DoRequestSetResetLED(FALSE);	
	}
	else if (sKey == CSD_LED2_OFF)
	{
		if (m_pCodec)
			m_pCodec->DoRequestSetResetLED(TRUE);	
	}
	else if (sKey == CSD_TERM)
	{
		if (m_pCodec)
		{
			m_evGetTermMask.ResetEvent();
			m_pCodec->DoRequestGetTerminationState();
			if (WaitForSingleObject(m_evGetTermMask, 1000) == WAIT_OBJECT_0)
			{
				if (sValue == CSD_ON)
					m_byTermMask = SETBIT(m_byTermMask, wSource);
				else if (sValue == CSD_OFF)
					m_byTermMask = CLRBIT(m_byTermMask, wSource);

				m_evSetTermMask.ResetEvent();
				if (m_pCodec->DoRequestSetTerminationState(m_byTermMask))
				{
					if (WaitForSingleObject(m_evSetTermMask, 1000) == WAIT_OBJECT_0)
						DoConfirmSetValue(CId, CSD_TERM, sValue, dwUserData);
					else
						WK_TRACE_WARNING(_T("DoRequestSetTerminationState timeout\n"));
				}
			}
			else
				WK_TRACE_WARNING(_T("DoRequestGetTerminationState timeout\n"));
		}
	}
	else if (sKey == CSD_RESOLUTION)
	{
/*	// TODO: Es ist zu überlegen, ob dieses Interface für die Tasha noch sinnvoll ist.
	if (m_VideoStream[0][wSource].bEncoderState)
		{
			OnRequestStartVideo(CId,	(Resolution)_wtoi(sValue),
										m_VideoStream[0][wSource].Compression,
										m_VideoStream[0][wSource].Ct,
										m_VideoStream[0][wSource].EncParam.nFps,
										m_VideoStream[0][wSource].EncParam.nIFrameIntervall,
										0);
		}
*/
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestVersionInfo(WORD wGroupID)
{
	DoConfirmVersionInfo(wGroupID, 3);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE(_T("CIPCOutputTashaUnit::OnRequestReset\tGroupID=%i\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputTashaUnit::OnRequestReset\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_pCodec)
		m_pCodec->ReadDebugConfiguration();

	// Alle Bildanforderungen disablen. Server muß die Streams neu anfordern!
	WK_TRACE(_T("CIPCOutputTashaUnit::OnRequestReset Dismiss VideoStream Info\n"));
	for (int nCamID = 0; nCamID < MAX_CAMERAS; nCamID++)
	{
		for (int nStreamID = 0; nStreamID < MAX_STREAMS; nStreamID++)
		{
			m_VideoStream[nStreamID][nCamID].bEncoderState				= FALSE;
			m_VideoStream[nStreamID][nCamID].dwUserData					= (DWORD)-1;
			m_VideoStream[nStreamID][nCamID].Compression				= COMPRESSION_NONE;
			m_VideoStream[nStreamID][nCamID].Resolution					= RESOLUTION_NONE;
			m_VideoStream[nStreamID][nCamID].Ct							= COMPRESSION_UNKNOWN;
			m_VideoStream[nStreamID][nCamID].EncParam.nStreamID			= nStreamID;
			m_VideoStream[nStreamID][nCamID].EncParam.eRes				= eImageResUnvalid;
			m_VideoStream[nStreamID][nCamID].EncParam.eType				= eTypeUnvalid;
			m_VideoStream[nStreamID][nCamID].EncParam.nBitrate			= 0;
			m_VideoStream[nStreamID][nCamID].EncParam.nFps				= 0;
			m_VideoStream[nStreamID][nCamID].EncParam.nIFrameIntervall	= 0;
		}
	}

	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestSetRelay(CSecID relayID, BOOL bOpenClose)
{
 	if (relayID.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputTashaUnit::OnRequestSetRelay\tWrong wGroupID\n"));

		DoIndicateError(0, relayID, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_iType != OUTPUT_TYPE_RELAY)
	{
		WK_TRACE_ERROR(_T("CIPCOutputTashaUnit::OnRequestSetRelay\tWrong Grouptype\n"));
		return;
	}

	if (m_pCodec)
		m_pCodec->DoRequestSetRelayState(relayID.GetSubID(), bOpenClose);
}

/////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose)
{
///	WK_TRACE(_T("HOST: OnConfirmSetRelayState (Relay=%d State=%d)\n"), wRelayID, bOpenClose);

	CSecID CId(m_wGroupID, wRelayID);

	// Und Server mitteilen.
	DoConfirmSetRelay(CId, bOpenClose);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestCurrentState(WORD wGroupID)
{
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputTashaUnit::OnRequestCurrentState\tWrong wGroupID\n"));
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_iType == OUTPUT_TYPE_RELAY)
	{
		if (m_pCodec)
			m_pCodec->DoRequestGetRelayState();
	}	
	else
		WK_TRACE_ERROR(_T("OnRequestCurrentState Unknown iType (%d)\n"), m_iType);
}

/////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetRelayState(DWORD dwRelayStateMask)
{
	DoConfirmCurrentState(m_wGroupID, dwRelayStateMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestHardware(WORD wGroupID)
{
	int iErrorCode = 0;

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputTashaUnit::OnRequestHardware\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	DoConfirmHardware(m_wGroupID, iErrorCode,
									FALSE, /* No SW-Compress */
									FALSE, /* No SW-DeCompress */
									TRUE,  /* Co-Compress */
									FALSE, /* No Co-DeCompress */
									FALSE);/* No Overlay */
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestDisconnect()
{
}

		 
//////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestStartVideo(CSecID camID,	Resolution res, Compression comp,
													CompressionType ct,	int iFPS, int iIFrameInterval, DWORD dwUserData)
{
	WORD wSource  = camID.GetSubID();
	int nStreamID = dwUserData;

	if (m_pCodec)
	{
//		WK_TRACE(_T("CIPCOutputTashaUnit::OnRequestStartVideo (Source=%d, UserID=0x%x, Res=%d, Comp=%d, ct=%d, Fps=%d, IFrameIntervall=%d)\n"),
//									wSource, dwUserData, res, comp, ct, iFPS, iIFrameInterval);

		EncoderParam EncParam;
		EncParam.nBitrate			= 2048;
		EncParam.nFps				= iFPS;
		EncParam.nIFrameIntervall	= iIFrameInterval;
		EncParam.nStreamID			= nStreamID;
		EncParam.eRes				= eImageResMid;

		// RESOLUTION_HIGH = für den 1. Stream: Halbbild (704x288), für den 2.Stream:CIF (352x288)
		// RESOLUTION_MID  = CIF (352x288)
		// RESOLUTION_LOW  = für den 1.Stream: CIF (352x288), für den 2. Stream:QCIF (176x144)
		switch (res)
		{
			case RESOLUTION_HIGH:
				if (nStreamID == 0)
					EncParam.eRes = eImageResHigh;	// 2CIF (Halbbild
				else if (nStreamID == 1)
					EncParam.eRes = eImageResMid;	// CIF bei Stream1
				break;
			case RESOLUTION_MID:
				EncParam.eRes = eImageResMid;	// CIF
				break;
			case RESOLUTION_LOW: 
				if (nStreamID == 0)
					EncParam.eRes = eImageResMid;	// CIF bei Stream0
				else if (nStreamID == 1)
					EncParam.eRes = eImageResLow;	// QCIF bei Stream1
				break; 
			default:
				EncParam.eRes = eImageResMid;
				WK_TRACE_WARNING(_T("CIPCOutputTashaUnit::OnRequestStartVideo Unsupported resolution (%d)"), res);
		}

		switch (ct)
		{
			case COMPRESSION_MPEG4:
				EncParam.eType = eTypeMpeg4;
				break;
			case COMPRESSION_YUV_422:
				EncParam.eType = eTypeYUV422;
				break;
			default:
				EncParam.eType = eTypeMpeg4;	
				WK_TRACE_WARNING(_T("CIPCOutputTashaUnit::OnRequestStartVideo Unsupported compressiontype (%d)"), ct);
		}

		switch (comp)
		{
			case COMPRESSION_NONE:
				EncParam.nBitrate	= 2048;
				break;
			case COMPRESSION_1:	
				EncParam.nBitrate	= 4000;
				break;
			case COMPRESSION_2:		
				EncParam.nBitrate	= 4000;
				break;	
			case COMPRESSION_3:		
				EncParam.nBitrate	= 4000;
				break;	 
			case COMPRESSION_4:		
				EncParam.nBitrate	= 3968;
				break;		
			case COMPRESSION_5:		
				EncParam.nBitrate	= 3840;
				break;		
			case COMPRESSION_6:		
				EncParam.nBitrate	= 3584;
				break;		
			case COMPRESSION_7:		
				EncParam.nBitrate	= 3328;
				break;		
			case COMPRESSION_8:		
				EncParam.nBitrate	= 3072;
				break;		
			case COMPRESSION_9:		
				EncParam.nBitrate	= 2816;
				break;		
			case COMPRESSION_10:	
				EncParam.nBitrate	= 2560;
				break;	
			case COMPRESSION_11:	
				EncParam.nBitrate	= 2304;
				break;		
			case COMPRESSION_12:	
				EncParam.nBitrate	= 2048;
				break;		
			case COMPRESSION_13:	
				EncParam.nBitrate	= 1792;
				break;		
			case COMPRESSION_14:	
				EncParam.nBitrate	= 1536;
				break;		
			case COMPRESSION_15:	
				EncParam.nBitrate	= 1280;
				break;		
			case COMPRESSION_16:	
				EncParam.nBitrate	= 1024;
				break;		
			case COMPRESSION_17:	
				EncParam.nBitrate	= 896;
				break;			
			case COMPRESSION_18:	
				EncParam.nBitrate	= 768;
				break;		
			case COMPRESSION_19:	
				EncParam.nBitrate	= 640;
				break;
			case COMPRESSION_20:	
				EncParam.nBitrate	= 512;
				break;			
			case COMPRESSION_21:	
				EncParam.nBitrate	= 384;
				break;		
			case COMPRESSION_22:	
				EncParam.nBitrate	= 320;
				break;		
			case COMPRESSION_23:	
				EncParam.nBitrate	= 256;
				break;			
			case COMPRESSION_24:	
				EncParam.nBitrate	= 192;
				break;			
			case COMPRESSION_25:	
				EncParam.nBitrate	= 128;
				break;			
			case COMPRESSION_26:	
				EncParam.nBitrate	= 64;
				break;
			default:
				WK_TRACE_WARNING(_T("CIPCOutputTashaUnit::OnRequestStartVideo Unsupported compression (%d)"), comp);
				EncParam.nBitrate = 2048;
		}

		if (nStreamID < MAX_STREAMS)
		{
			m_VideoStream[nStreamID][wSource].bEncoderState	= TRUE;

			// Hat ich einer der Encoderparameter für diesen Stream/Source geändert?
			if ((m_VideoStream[nStreamID][wSource].EncParam.eRes != EncParam.eRes) ||
				(m_VideoStream[nStreamID][wSource].EncParam.eType != EncParam.eType) ||
				(m_VideoStream[nStreamID][wSource].EncParam.nBitrate != EncParam.nBitrate) ||
				(m_VideoStream[nStreamID][wSource].EncParam.nFps != EncParam.nFps) ||
				(m_VideoStream[nStreamID][wSource].EncParam.nIFrameIntervall != EncParam.nIFrameIntervall))
			{
				m_VideoStream[nStreamID][wSource].dwUserData	= dwUserData;
				m_VideoStream[nStreamID][wSource].Compression	= comp;
				m_VideoStream[nStreamID][wSource].Resolution	= res;
				m_VideoStream[nStreamID][wSource].Ct			= ct;
				m_VideoStream[nStreamID][wSource].EncParam		= EncParam;
				m_pCodec->DoRequestSetEncoderParam(dwUserData, wSource, EncParam);
			}
				
			// Beim 2. Stream muß jedes Bild einzeln angefordert werden!
			if (nStreamID == 1)
				m_pCodec->DoRequestNewSingleFrame(dwUserData, wSource);
		} 
		else
			WK_TRACE_ERROR(_T("OnRequestStartVideo unsupported streamID (%d)\n"), nStreamID);
	}
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestStopVideo(CSecID camID, DWORD dwUserData)
{
	// Parameter auf '0' bzw. 'unvalid' besagt Encoder 'stop'

	WORD wSource = camID.GetSubID(); 
//	WK_TRACE(_T("CIPCOutputTashaUnit::OnRequestStopVideo (Source=%d UserID=0x%x)\n"), wSource, dwUserData);

	int nStreamID = dwUserData;
	if (nStreamID < MAX_STREAMS)
	{
		m_VideoStream[nStreamID][wSource].bEncoderState	= FALSE;

		// Erzwingt ein I-Frame, beim nächsten Start!
		if (nStreamID == 1)
		{
			m_VideoStream[nStreamID][wSource].dwUserData					= (DWORD)-1;
			m_VideoStream[nStreamID][wSource].bEncoderState					= FALSE;
			m_VideoStream[nStreamID][wSource].EncParam.eRes					= eImageResUnvalid;
			m_VideoStream[nStreamID][wSource].EncParam.eType				= eTypeUnvalid;
			m_VideoStream[nStreamID][wSource].EncParam.nStreamID			= nStreamID;
			m_VideoStream[nStreamID][wSource].EncParam.nBitrate				= 0;
			m_VideoStream[nStreamID][wSource].EncParam.nIFrameIntervall		= 0;
			m_VideoStream[nStreamID][wSource].EncParam.nFps					= 0;
		}
	}
	else
		WK_TRACE_ERROR(_T("OnRequestStopVideo unsupported streamID (%d)\n"), nStreamID);
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::IndicationDataReceived(const DATA_PACKET* pPacket)
{
	BOOL bResult = FALSE;
	CSystemTime	TimeStamp;

	if (!pPacket)
		return;

	DWORD			dwUserData  = 0;
	WORD			wSource		= pPacket->wSource;
	Resolution			res		= RESOLUTION_NONE;
	Compression			comp	= COMPRESSION_NONE;
	CompressionType		ct		= COMPRESSION_UNKNOWN;
	CIPCSavePictureType	picType	= SPT_FULL_PICTURE;

	if (pPacket->eSubType == eSubIFrame)
		picType	= SPT_FULL_PICTURE;
	else if (pPacket->eSubType == eSubPFrame)
		picType	= SPT_DIFF_PICTURE;
	else
		WK_TRACE(_T("CIPCOutputTashaUnit::IndicationDataReceived Unknown Picturetype (%d)\n"), pPacket->eSubType);

	int	nStreamID	= pPacket->dwStreamID;
	if (nStreamID < MAX_STREAMS)
	{
		if (m_VideoStream[nStreamID][wSource].bEncoderState)
		{
			dwUserData	= m_VideoStream[nStreamID][wSource].dwUserData;
			res			= m_VideoStream[nStreamID][wSource].Resolution;
			comp		= m_VideoStream[nStreamID][wSource].Compression;
			ct			= m_VideoStream[nStreamID][wSource].Ct;
			bResult		= TRUE;
		}
	}
	else
		WK_TRACE_ERROR(_T("IndicationDataReceived unsupported streamID (%d)\n"), nStreamID);

	if (bResult)
	{
		// Uhrzeit gültig?
		if (pPacket->TimeStamp.bValid)
		{
			TimeStamp.wYear			= pPacket->TimeStamp.wYear;
			TimeStamp.wMonth		= pPacket->TimeStamp.wMonth;
			TimeStamp.wDay			= pPacket->TimeStamp.wDay;
	  		TimeStamp.wHour			= pPacket->TimeStamp.wHour;
			TimeStamp.wMinute		= pPacket->TimeStamp.wMinute;
			TimeStamp.wSecond		= pPacket->TimeStamp.wSecond;
			TimeStamp.wMilliseconds = pPacket->TimeStamp.wMSecond;
			TimeStamp.wDayOfWeek	= 0;
		}
		else
		{
			TimeStamp.GetLocalTime();
		}
 
		// CSecID anhand der Kameranummer
		CSecID camID(m_wGroupID, wSource);
	
		if (this->GetIPCState() == CIPC_STATE_CONNECTED)
		{
			// Zum Stream passendes Picturerecord bauen...
			CIPCPictureRecord PictRec(this,
										(const unsigned char*)&pPacket->pImageData,
										pPacket->dwImageDataLen,
										camID,
										res,
										comp,
										ct,	
										TimeStamp,
										pPacket->bVidPresent,
										NULL,
										0,
										picType,
										pPacket->dwFieldID);

			// ... und ab damit
			if (pPacket->bMotion)
				DoIndicateVideo(PictRec, pPacket->Point[0].cx, pPacket->Point[0].cy, dwUserData, SECID_NO_ID);
			else
				DoIndicateVideo(PictRec, 0, 0, dwUserData, SECID_NO_ID);
		}
		else
			TRACE(_T("CIPCOutputTashaUnit::IndicationDataReceived...CIPC not connected\n"));

		//TRACE(_T("DoIndicateVideo (0x%x)\n"), camID);
	}
//	else
//		WK_TRACE(_T("CIPCOutputTashaUnit::IndicationDataReceived Stream %d disabled...discard frame\n"),wSource);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnIndicationVideoStateChanged(WORD wCamSubID, BOOL bVideoState)
{					  
	CString sError, sMsg;
		   									 
	if (!bVideoState)		    
	{ 
		sMsg.LoadString(IDS_RTE_NO_VIDEO);
		sError.Format(sMsg, wCamSubID+1);
		CWK_RunTimeError RTerr(REL_ERROR, RTE_CAMERA_MISSING, sError, wCamSubID+1, TRUE);
		RTerr.Send();

		DoIndicateError(0, CSecID(m_wGroupID, wCamSubID), CIPC_ERROR_CAMERA_NOT_PRESENT, bVideoState, sError);
	}
	else
	{
		sMsg.LoadString(IDS_RTE_VIDEO);
		sError.Format(sMsg, wCamSubID+1);

		CWK_RunTimeError RTerr(REL_ERROR, RTE_CAMERA_MISSING, sError, wCamSubID+1, FALSE);
		RTerr.Send();

		DoIndicateError(0, CSecID(m_wGroupID, wCamSubID), CIPC_ERROR_CAMERA_NOT_PRESENT, bVideoState, sError);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnIndicationVideoStreamStateChanged(WORD wCamSubID, BOOL bVideoState)
{					  
	CString sError, sMsg;
		   									 
	if (!bVideoState)		    
	{ 
		sMsg.LoadString(IDS_RTE_VIDEO_STREAM_ABORTED);
		sError.Format(sMsg, wCamSubID+1);
		CWK_RunTimeError RTerr(REL_ERROR, RTE_VIDEO_STREAM_ABORTED, sError, wCamSubID+1, TRUE);
		RTerr.Send();

		DoIndicateError(0, CSecID(m_wGroupID, wCamSubID), CIPC_ERROR_VIDEO_STREAM_ABORTED, bVideoState, sError);
	}
	else
	{
		sMsg.LoadString(IDS_RTE_VIDEO_STREAM_RESUMED);
		sError.Format(sMsg, wCamSubID+1);

		CWK_RunTimeError RTerr(REL_ERROR, RTE_VIDEO_STREAM_ABORTED, sError, wCamSubID+1, FALSE);
		RTerr.Send();

		DoIndicateError(0, CSecID(m_wGroupID, wCamSubID), CIPC_ERROR_VIDEO_STREAM_ABORTED, bVideoState, sError);
	}
}

/////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnIndicationSlaveInitReady(WORD wCamSubID)
{
	// Dem Server mitteilen, das der DSP bereit ist
	DoIndicateError(0, CSecID(m_wGroupID, wCamSubID), CIPC_ERROR_VIDEO_STREAM_ABORTED, TRUE, _T("OnIndicationSlaveInitReady"));
}

/////////////////////////////////////////////////////////////////////////
EncoderState CIPCOutputTashaUnit::GetEncoderState()
{
	if (!m_pCodec)
		return eEncoderStateUnvalid;

	return m_pCodec->GetEncoderState();
}


//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
	if (m_pCodec)
	{
		CIPCActivityMask mask(type);
		if (m_pCodec->GetMask(camID.GetSubID(), mask))
			DoConfirmGetMask(camID, dwUserID, mask);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
	if (m_pCodec)
	{
		if (m_pCodec->SetMask(camID.GetSubID(), mask))
			DoConfirmSetMask(camID, mask.GetType(), dwUserID);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

#if (1)	// TODO: Es sollte später auf jedenfall CSD_BRIGHTNESS_PERCENT verwendet werden
	nValue = (int)((double)nValue * (double)MAX_BRIGHTNESS / 100.0 + 0.5);
	sValue.Format(_T("%d"), nValue); 
	DoConfirmGetValue(CId, CSD_BRIGHTNESS, sValue, dwUserData);
#else
	sValue.Format(_T("%d"), nValue); 
	DoConfirmGetValue(CId, CSD_BRIGHTNESS_PERCENT, sValue, dwUserData);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

#if (1)	// TODO: Es sollte später auf jedenfall CSD_BRIGHTNESS_PERCENT verwendet werden
	nValue = (int)((double)nValue * (double)MAX_CONTRAST / 100.0 + 0.5);
	sValue.Format(_T("%d"), nValue); 
	DoConfirmGetValue(CId, CSD_CONTRAST, sValue, dwUserData);
#else
	sValue.Format(_T("%d"), nValue); 
	DoConfirmGetValue(CId, CSD_CONTRAST_PERCENT, sValue, dwUserData);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

#if (1)	// TODO: Es sollte später auf jedenfall CSD_BRIGHTNESS_PERCENT verwendet werden
	nValue = (int)((double)nValue * (double)MAX_SATURATION / 100.0 + 0.5);
	sValue.Format(_T("%d"), nValue); 
	DoConfirmGetValue(CId, CSD_SATURATION, sValue, dwUserData);
#else
	sValue.Format(_T("%d"), nValue); 
	DoConfirmGetValue(CId, CSD_SATURATION_PERCENT, sValue, dwUserData);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

#if (1)	// TODO: Es sollte später auf jedenfall CSD_BRIGHTNESS_PERCENT verwendet werden
	nValue = (int)((double)nValue * (double)MAX_BRIGHTNESS / 100.0 + 0.5);
	sValue.Format(_T("%d"), nValue); 
	DoConfirmSetValue(CId, CSD_BRIGHTNESS, sValue, dwUserData);
#else
	sValue.Format(_T("%d"), nValue); 
	DoConfirmSetValue(CId, CSD_BRIGHTNESS_PERCENT, sValue, dwUserData);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

#if (1)	// TODO: Es sollte später auf jedenfall CSD_BRIGHTNESS_PERCENT verwendet werden
	nValue = (int)((double)nValue * (double)MAX_CONTRAST / 100.0 + 0.5);
	sValue.Format(_T("%d"), nValue); 
	DoConfirmSetValue(CId, CSD_CONTRAST, sValue, dwUserData);
#else
	sValue.Format(_T("%d"), nValue); 
	DoConfirmSetValue(CId, CSD_CONTRAST_PERCENT, sValue, dwUserData);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

#if (1)	// TODO: Es sollte später auf jedenfall CSD_BRIGHTNESS_PERCENT verwendet werden
	nValue = (int)((double)nValue * (double)MAX_SATURATION / 100.0 + 0.5);
	sValue.Format(_T("%d"), nValue); 
	DoConfirmSetValue(CId, CSD_SATURATION, sValue, dwUserData);
#else
	sValue.Format(_T("%d"), nValue); 
	DoConfirmSetValue(CId, CSD_SATURATION_PERCENT, sValue, dwUserData);
#endif
}

//////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetMDMaskSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel)
{
	CSecID CId(m_wGroupID, wSource);

	DoConfirmSetValue(CId, CSD_MD_MASK_SENSITIVITY, sLevel, dwUserData);
}

//////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetMDMaskSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel)
{
	CSecID CId(m_wGroupID, wSource);

	DoConfirmGetValue(CId, CSD_MD_MASK_SENSITIVITY, sLevel, dwUserData);
}

//////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel)
{
	CSecID CId(m_wGroupID, wSource);

	DoConfirmSetValue(CId, CSD_MD_ALARM_SENSITIVITY, sLevel, dwUserData);
}

//////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel)
{
	CSecID CId(m_wGroupID, wSource);

	DoConfirmGetValue(CId, CSD_MD_ALARM_SENSITIVITY, sLevel, dwUserData);
}


//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask)
{
	CString sValue;

	sValue.Format(_T("%lu"), dwCameraMask); 
	
	CSecID CId(m_wGroupID, SECID_NO_SUBID);
	DoConfirmGetValue(CId, CSD_SCAN_FOR_CAMERAS, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetTerminationState(BYTE byTermMask)
{
	m_evSetTermMask.SetEvent();
}	

//////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetTerminationState(BYTE byTermMask)
{
	m_byTermMask = byTermMask;
	m_evGetTermMask.SetEvent();
}	

/////////////////////////////////////////////////////////////////////////////
LPBITMAPINFO CIPCOutputTashaUnit::CreateDIB(const DATA_PACKET* pPacket)
{
	if (m_lpBMI)
	{
		m_lpBMI->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
		m_lpBMI->bmiHeader.biWidth			= pPacket->wSizeH;
		m_lpBMI->bmiHeader.biHeight			= pPacket->wSizeV;
		m_lpBMI->bmiHeader.biPlanes			= 1;
		m_lpBMI->bmiHeader.biBitCount		= pPacket->wBytesPerPixel*8; 
		m_lpBMI->bmiHeader.biCompression	= mmioFOURCC('Y','4','2','2');
		m_lpBMI->bmiHeader.biSizeImage		= pPacket->dwImageDataLen;

		memcpy(m_lpBMI->bmiColors, &pPacket->pImageData, pPacket->dwImageDataLen);
	}

	return m_lpBMI;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnResetButtonPressed()
{
	// Serverreset auslösen
	DoRequestSetValue(SECID_NO_ID, CSD_RESET, _T(""), 0);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnCommandSendError(DWORD dwCmd)
{
	if (dwCmd == CIPC_OUTPUT_INDICATE_VIDEO)
	{
		m_dwOnCommandSendErrorCounter++;
		if (m_dwOnCommandSendErrorCounter == 500)
		{
			WK_TRACE_WARNING(_T("500 x OnCommandSendError(CIPC_OUTPUT_INDICATE_VIDEO)\n"));
			m_dwOnCommandSendErrorCounter = 0;
		}
	}

//	CIPCOutput::OnCommandSendError(dwCmd);
}
