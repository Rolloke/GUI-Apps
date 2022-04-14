/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CMyTasha.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/CMyTasha.h $
// CHECKIN:		$Date: 2.02.05 12:32 $
// VERSION:		$Revision: 20 $
// LAST CHANGE:	$Modtime: 2.02.05 11:07 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMYTASHA_H__BA1DC7C9_AB4C_401C_888D_33EC1679383A__INCLUDED_)
#define AFX_CMYTASHA_H__BA1DC7C9_AB4C_401C_888D_33EC1679383A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCodec;
class CTashaUnitDlg;
class CProgressDlg;
class CMyTasha : public CCodec   
{
public:
	CMyTasha(CTashaUnitDlg* pWnd, int nBoardID, const CString &sAppIniName);
	virtual ~CMyTasha();

	virtual	BOOL OnConfirmInitComplete();
	virtual BOOL OnConfirmOpenDevice(int nBoardID, int nErrors);
	virtual BOOL OnConfirmSetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam);
	virtual BOOL OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmChangePermanentMask(DWORD dwUserData, WORD wSource, int nX, int nY, int nValue);
	virtual BOOL OnConfirmGetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam);
	virtual BOOL OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue);

	virtual BOOL OnConfirmSetMDMaskSensitivity(DWORD dwUserData,  WORD wSource, const CString &sLevel);
	virtual BOOL OnConfirmGetMDMaskSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel);
	virtual BOOL OnConfirmSetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel);
	virtual BOOL OnConfirmGetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel);

	virtual BOOL OnConfirmGetMDTreshold(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmGetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmGetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmGetMaskDelay(DWORD dwUserData, WORD wSource, int nValue);
	virtual BOOL OnConfirmSetPermanentMask(DWORD dwUserData, WORD wSource, DWORD dwAddr, DWORD dwLen);
	virtual BOOL OnConfirmClearPermanentMask(DWORD dwUserData, WORD wSource);
	virtual BOOL OnConfirmInvertPermanentMask(DWORD dwUserData, WORD wSource);
	virtual BOOL OnConfirmStartCapture(); 
	virtual BOOL OnConfirmStopCapture(); 
	virtual BOOL OnConfirmPauseCapture(); 
	virtual BOOL OnConfirmResumeCapture(); 
	virtual BOOL OnConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask);
	virtual BOOL OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose);
	virtual BOOL OnConfirmGetRelayState(DWORD dwRelayStateMask);
	virtual BOOL OnConfirmGetAlarmState(DWORD dwAlarmStateMask);
	virtual BOOL OnConfirmSetAlarmEdge(DWORD dwAlarmEdge);
	virtual BOOL OnConfirmGetAlarmEdge(DWORD dwAlarmEdge);
	virtual BOOL OnConfirmSetChannel(WORD wSource, WORD wNewChannel);
	virtual BOOL OnConfirmSetAnalogOut(DWORD dwCrosspointMask);
	virtual BOOL OnConfirmGetAnalogOut(DWORD dwCrosspointMask);
	virtual BOOL OnConfirmSetTerminationState(BYTE byTermMask);
	virtual BOOL OnConfirmGetTerminationState(BYTE byTermMask);
	virtual BOOL OnConfirmSetVideoEnableState(BOOL bState);
	virtual BOOL OnConfirmGetVideoEnableState(BOOL bState);
	virtual BOOL OnConfirmSetCrosspointEnableState(BOOL bState);
	virtual BOOL OnConfirmGetCrosspointEnableState(BOOL bState);
	virtual BOOL OnConfirmEnableWatchdog(int nBF533ResponseTimeOut);
	virtual BOOL OnConfirmTriggerWatchdog(int nWatchdogTimeOut);
	virtual BOOL OnConfirmSetPowerLED(BOOL bState);
	virtual BOOL OnConfirmSetResetLED(BOOL bState);
	virtual BOOL OnNotifySlaveInitReady(WORD wSource);

	virtual BOOL OnConfirmGetVariationRegister(WORD wValue);
	virtual BOOL OnConfirmTermination();
	virtual BOOL OnConfirmSetPCIFrameBufferAddress(DWORD dwPCIAddr);
	virtual BOOL OnConfirmEPLDdataTransfer(DWORD dwAddr, DWORD dwLen);
	virtual BOOL OnConfirmBoot533(int nBFNr, DWORD dwAddr, DWORD dwLen);
	virtual BOOL OnConfirmTransferBootloader(DWORD dwAddr, DWORD dwLen);
	virtual BOOL OnConfirmWriteToEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr);
	virtual BOOL OnConfirmReadFromEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr);
	
	virtual BOOL OnIndicationVideoStreamAborted(int nChannel);
	virtual BOOL OnIndicationVideoStreamResumed(int nChannel);
	virtual BOOL OnIndicationCameraStatusChanged(int nChannel, BOOL bState);
	virtual BOOL OnIndicationAlarmStateChanged(DWORD dwAlarmMask);
	virtual BOOL OnIndicationAdapterSelectStateChanged(DWORD dwAlarmMask);
	virtual BOOL OnIndicationPCKeysStateChanged(DWORD dwAlarmMask);
	virtual BOOL OnIndicationInformation(int nMessage, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);
	virtual BOOL OnIndicationNewCodecData(DATA_PACKET* pDataPacket);
	virtual BOOL OnIndicationNoBF533Response(int nChannel);
	virtual BOOL OnIndicationNoBF535Response();
	virtual BOOL OnConfirmSetNR(DWORD dwUserData, WORD wSource, BOOL bEnable);
	virtual BOOL OnConfirmGetNR(DWORD dwUserData, WORD wSource, BOOL bEnable);

	virtual void OnIndicateError(int nErrorCode);
	virtual void OnIndicateWarning(int nWarningCode);


private:
	CTashaUnitDlg*	m_pWnd;

};

#endif // !defined(AFX_CMYTASHA_H__BA1DC7C9_AB4C_401C_888D_33EC1679383A__INCLUDED_)
