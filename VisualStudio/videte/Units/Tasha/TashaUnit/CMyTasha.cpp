/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CMyTasha.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/CMyTasha.cpp $
// CHECKIN:		$Date: 2.02.05 12:32 $
// VERSION:		$Revision: 33 $
// LAST CHANGE:	$Modtime: 2.02.05 11:07 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TashaUnit.h"
#include "TashaUnitDlg.h"
#include "CMyTasha.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
CMyTasha::CMyTasha(CTashaUnitDlg* pWnd, int nBoardID, const CString &sAppIniName) : CCodec(nBoardID, sAppIniName)
{
	m_pWnd = pWnd;
}

//////////////////////////////////////////////////////////////////////
CMyTasha::~CMyTasha()
{
	TRACE(_T("CMyTasha::~CMyTasha()\n"));
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmInitComplete()
{
	// Call Baseclass
	return CCodec::OnConfirmInitComplete();
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam)
{
	// Parametercheck
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmSetEncoderParam(dwUserData, wSource, EncParam);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetEncoderParam(dwUserData, wSource, EncParam);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmSetBrightness(dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetBrightness(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmSetContrast(dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetContrast(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmSetSaturation(dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetSaturation(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam)
{
	// Parametercheck
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmGetEncoderParam(dwUserData, wSource, EncParam);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetEncoderParam(dwUserData, wSource, EncParam);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmGetBrightness(dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetBrightness(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmGetContrast(dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetContrast(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmGetSaturation(dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetSaturation(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetAnalogOut(DWORD dwCrosspointMask)
{
	// Call Baseclass
	CCodec::OnConfirmSetAnalogOut(dwCrosspointMask);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetAnalogOut(dwCrosspointMask);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetAnalogOut(DWORD dwCrosspointMask)
{
	// Call Baseclass
	CCodec::OnConfirmGetAnalogOut(dwCrosspointMask);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetAnalogOut(dwCrosspointMask);
}	

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetTerminationState(BYTE byTermMask)
{
	// Call Baseclass
	CCodec::OnConfirmSetTerminationState(byTermMask);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetTerminationState(byTermMask);
}	

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetTerminationState(BYTE byTermMask)
{
	// Call Baseclass
	CCodec::OnConfirmGetTerminationState(byTermMask);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetTerminationState(byTermMask);
}	

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetVideoEnableState(BOOL bState)
{
	// Call Baseclass
	CCodec::OnConfirmSetVideoEnableState(bState);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetVideoEnableState(bState);
}	

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetVideoEnableState(BOOL bState)
{
	// Call Baseclass
	CCodec::OnConfirmGetVideoEnableState(bState);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetVideoEnableState(bState);
}	

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetCrosspointEnableState(BOOL bState)
{
	// Call Baseclass
	CCodec::OnConfirmSetCrosspointEnableState(bState);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetCrosspointEnableState(bState);
}	

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetCrosspointEnableState(BOOL bState)
{
	// Call Baseclass
	CCodec::OnConfirmGetCrosspointEnableState(bState);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetCrosspointEnableState(bState);
}	

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetChannel(WORD wSource, WORD wNewSource)
{
	// Call Baseclass
	CCodec::OnConfirmSetChannel(wSource, wNewSource);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetChannel(wSource, wNewSource);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetMDMaskSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel)
{
	// Call Baseclass
	CCodec::OnConfirmSetMDMaskSensitivity(dwUserData, wSource, sLevel);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetMDMaskSensitivity(dwUserData, wSource, sLevel);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetMDMaskSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel)
{
	// Call Baseclass
	CCodec::OnConfirmGetMDMaskSensitivity(dwUserData, wSource, sLevel);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetMDMaskSensitivity(dwUserData, wSource, sLevel);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel)
{
	// Call Baseclass
	CCodec::OnConfirmSetMDAlarmSensitivity(dwUserData, wSource, sLevel);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetMDAlarmSensitivity(dwUserData, wSource, sLevel);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel)
{
	// Call Baseclass
	CCodec::OnConfirmGetMDAlarmSensitivity(dwUserData, wSource, sLevel);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetMDAlarmSensitivity(dwUserData, wSource, sLevel);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmSetMDTreshold(dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetMDTreshold(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmSetMaskTreshold(dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetMaskTreshold(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmSetMaskIncrement(dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetMaskIncrement(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmSetMaskDelay(dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetMaskDelay(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmChangePermanentMask(DWORD dwUserData, WORD wSource, int nX, int nY, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmChangePermanentMask(dwUserData, wSource, nX, nY, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmChangePermanentMask(dwUserData, wSource, nX, nY, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetMDTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmGetMDTreshold(dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetMDTreshold(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmGetMaskTreshold(dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetMaskTreshold(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmGetMaskIncrement(dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetMaskIncrement(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetMaskDelay(DWORD dwUserData, WORD wSource, int nValue)
{
	// Parametercheck
	nValue = min(max(nValue, 0), 100);
	wSource= min(max(wSource, 0), CHANNEL_COUNT);

	// Call Baseclass
	CCodec::OnConfirmGetMaskDelay(dwUserData, wSource, nValue);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetMaskDelay(dwUserData, wSource, nValue);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmStartCapture() 
{
	// Call Baseclass
	CCodec::OnConfirmStartCapture();

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmStartCapture();
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmStopCapture() 
{
	// Call Baseclass
	CCodec::OnConfirmStopCapture();

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmStopCapture();
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmPauseCapture() 
{
	// Call Baseclass
	CCodec::OnConfirmPauseCapture();

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmPauseCapture();
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmResumeCapture()
{
	// Call Baseclass
	CCodec::OnConfirmResumeCapture();

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmResumeCapture();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnIndicationNewCodecData(DATA_PACKET* pDataPacket)
{
	BOOL bResult = FALSE;

	// Liefert TRUE, wenn kein Field fehlte. ansonsten synchronisiert der Stream sich
	// auf das nächste I-Frame.
	if (CCodec::OnIndicationNewCodecData(pDataPacket))
	{
		if (m_pWnd)
			bResult = m_pWnd->OnReceiveNewCodecData(pDataPacket);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask)
{
	BOOL bResult = FALSE;

	// Call Baseclass
	CCodec::OnConfirmScanForCameras(dwUserData, dwCameraMask);

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmScanForCameras(dwUserData, dwCameraMask);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose)
{
	BOOL bResult = FALSE;

	// Call Baseclass
	CCodec::OnConfirmSetRelayState(wRelayID, bOpenClose);

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmSetRelayState(wRelayID, bOpenClose);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetRelayState(DWORD dwRelayStateMask)
{
	BOOL bResult = FALSE;

	// Call Baseclass
	CCodec::OnConfirmGetRelayState(dwRelayStateMask);

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmGetRelayState(dwRelayStateMask);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetAlarmState(DWORD dwAlarmStateMask)
{
	BOOL bResult = FALSE;

	// Call Baseclass
	CCodec::OnConfirmGetAlarmState(dwAlarmStateMask);

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmGetAlarmState(dwAlarmStateMask);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetAlarmEdge(DWORD dwAlarmEdge)
{
	BOOL bResult = FALSE;

	// Call Baseclass
	CCodec::OnConfirmSetAlarmEdge(dwAlarmEdge);

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmSetAlarmEdge(dwAlarmEdge);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetAlarmEdge(DWORD dwAlarmEdge)
{
	BOOL bResult = FALSE;

	// Call Baseclass
	CCodec::OnConfirmGetAlarmEdge(dwAlarmEdge);

	if (m_pWnd)
		bResult = m_pWnd->OnConfirmGetAlarmEdge(dwAlarmEdge);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnNotifySlaveInitReady(WORD wSource)
{
	// Call Baseclass
	CCodec::OnNotifySlaveInitReady(wSource);

	if (m_pWnd)
		m_pWnd->OnNotifySlaveInitReady(wSource);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmOpenDevice(int nBoardID, int nErrors)
{
	// Call Baseclass
	CCodec::OnConfirmOpenDevice(nBoardID, nErrors);

	// OnConfirmOpenDevice darf nur aus dem Userinterface thread aufgerufen werden!
	if (m_pWnd)
		m_pWnd->PostMessage(WM_ON_CONFIRM_OPEN_DEVICE, nBoardID, nErrors);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetPermanentMask(DWORD dwUserData, WORD wSource, DWORD dwAddr, DWORD dwLen)
{
	// Call Baseclass
	return CCodec::OnConfirmSetPermanentMask(dwUserData, wSource, dwAddr, dwLen);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmClearPermanentMask(DWORD dwUserData, WORD wSource)
{
	BOOL bResult = FALSE;

	// Call Baseclass
	if (CCodec::OnConfirmClearPermanentMask(dwUserData, wSource))
	{
		if (m_pWnd)
			bResult = m_pWnd->OnConfirmClearPermanentMask(dwUserData, wSource);
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmInvertPermanentMask(DWORD dwUserData, WORD wSource)
{
	// Call Baseclass
	CCodec::OnConfirmInvertPermanentMask(dwUserData, wSource);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmInvertPermanentMask(dwUserData, wSource);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmEnableWatchdog(int nBF533ResponseTimeOut)
{
	// Call Baseclass
	return CCodec::OnConfirmEnableWatchdog(nBF533ResponseTimeOut);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmTriggerWatchdog(int nWatchdogTimeOut)
{
	// Call Baseclass
	return CCodec::OnConfirmTriggerWatchdog(nWatchdogTimeOut);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetPowerLED(BOOL bState)
{
	// Call Baseclass
	CCodec::OnConfirmSetPowerLED(bState);

	if (m_pWnd)
		m_pWnd->OnConfirmSetPowerLED(bState);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetResetLED(BOOL bState)
{
	// Call Baseclass
	CCodec::OnConfirmSetResetLED(bState);

	if (m_pWnd)
		m_pWnd->OnConfirmSetResetLED(bState);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnIndicationVideoStreamAborted(int nChannel)
{
	// Call Baseclass
	CCodec::OnIndicationVideoStreamAborted(nChannel);

	if (m_pWnd)
	{
#ifdef USE_MULTITHREADED_MESSAGE_HANDLING
		m_pWnd->OnIndicationVideoStreamAborted(nChannel, 0);
#else		
		m_pWnd->PostMessage(WM_ON_INDICATION_VIDEOSTREAM_ABORTED, nChannel);
#endif
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnIndicationVideoStreamResumed(int nChannel)
{
	// Call Baseclass
	CCodec::OnIndicationVideoStreamResumed(nChannel);

	if (m_pWnd)
	{
#ifdef USE_MULTITHREADED_MESSAGE_HANDLING
		m_pWnd->OnIndicationVideoStreamResumed(nChannel, 0);
#else
		m_pWnd->PostMessage(WM_ON_INDICATION_VIDEOSTREAM_RESUMED, nChannel);
#endif
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnIndicationCameraStatusChanged(int nChannel, BOOL bState)
{
	// Call Baseclass
	CCodec::OnIndicationCameraStatusChanged(nChannel, bState);

	BOOL bResult = FALSE;

	if (m_pWnd)
		bResult = m_pWnd->OnIndicationCameraStatusChanged(nChannel, bState);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnIndicationAlarmStateChanged(DWORD dwAlarmMask)
{
	BOOL bResult = FALSE;

	// Call Baseclass
	CCodec::OnIndicationAlarmStateChanged(dwAlarmMask);

	if (m_pWnd)
		bResult = m_pWnd->OnIndicationAlarmStateChanged(dwAlarmMask);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnIndicationAdapterSelectStateChanged(DWORD dwAdapterSelectMask)
{
	BOOL bResult = FALSE;

	// Call Baseclass
	CCodec::OnIndicationAdapterSelectStateChanged(dwAdapterSelectMask);

	if (m_pWnd)
		bResult = m_pWnd->OnIndicationAdapterSelectStateChanged(dwAdapterSelectMask);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnIndicationPCKeysStateChanged(DWORD dwPCKeysStateChanged)
{
	BOOL bResult = FALSE;

	// Call Baseclass
	CCodec::OnIndicationPCKeysStateChanged(dwPCKeysStateChanged);

	if (m_pWnd)
		bResult = m_pWnd->OnIndicationPCKeysStateChanged(dwPCKeysStateChanged);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnIndicationInformation(int nMessage, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{
	// Call Baseclass
	return CCodec::OnIndicationInformation(nMessage,dwParam1, dwParam2, dwParam3);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnIndicationNoBF533Response(int nChannel)
{
	CCodec::OnIndicationNoBF533Response(nChannel);
	
	if (m_pWnd)
	{
#ifdef USE_MULTITHREADED_MESSAGE_HANDLING
		m_pWnd->OnIndicationNoBF533Response(nChannel, 0);
#else
		m_pWnd->PostMessage(WM_ON_INDICATION_NO_BF533_RESPONSE, nChannel);
#endif
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnIndicationNoBF535Response()
{
	CCodec::OnIndicationNoBF535Response();
	
	if (m_pWnd)
	{
#ifdef USE_MULTITHREADED_MESSAGE_HANDLING
		m_pWnd->OnIndicationNoBF535Response(0, 0);
#else
		m_pWnd->PostMessage(WM_ON_INDICATION_NO_BF535_RESPONSE);
#endif
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetVariationRegister(WORD wValue)
{
	// Call Baseclass
	return CCodec::OnConfirmGetVariationRegister(wValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmTermination()
{
	// Call Baseclass
	return CCodec::OnConfirmTermination();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetPCIFrameBufferAddress(DWORD dwPCIAddr)
{
	// Call Baseclass
	return CCodec::OnConfirmSetPCIFrameBufferAddress(dwPCIAddr);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmEPLDdataTransfer(DWORD dwAddr, DWORD dwLen)
{
	// Call Baseclass
	return CCodec::OnConfirmEPLDdataTransfer(dwAddr, dwLen);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmBoot533(int nBFNr, DWORD dwAddr, DWORD dwLen)
{
	// Call Baseclass
	return CCodec::OnConfirmBoot533(nBFNr, dwAddr, dwLen);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmTransferBootloader(DWORD dwAddr, DWORD dwLen)
{
	// Call Baseclass
	return CCodec::OnConfirmTransferBootloader(dwAddr, dwLen);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmWriteToEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr)
{
	// Call Baseclass
	return CCodec::OnConfirmWriteToEEProm(dwAddr, dwLen, dwStartAddr);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmReadFromEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr)
{
	// Call Baseclass
	return CCodec::OnConfirmReadFromEEProm(dwAddr, dwLen, dwStartAddr);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmSetNR(DWORD dwUserData, WORD wSource, BOOL bEnable)
{
	// Call Baseclass
	CCodec::OnConfirmSetNR(dwUserData, wSource, bEnable);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmSetNR(dwUserData, wSource, bEnable);
}

//////////////////////////////////////////////////////////////////////
BOOL CMyTasha::OnConfirmGetNR(DWORD dwUserData, WORD wSource, BOOL bEnable)
{
	// Call Baseclass
	CCodec::OnConfirmGetNR(dwUserData, wSource, bEnable);

	if (!m_pWnd)
		return FALSE;

	return m_pWnd->OnConfirmGetNR(dwUserData, wSource, bEnable);
}

/////////////////////////////////////////////////////////////////////////////
void CMyTasha::OnIndicateError(int nErrorCode)
{
	// Should be overridden
}

/////////////////////////////////////////////////////////////////////////////
void CMyTasha::OnIndicateWarning(int nWarningCode)
{
	// Should be overridden
}
