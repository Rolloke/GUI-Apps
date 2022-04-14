/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CipcOutputTashaUnit.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/CipcOutputTashaUnit.h $
// CHECKIN:		$Date: 3.02.05 16:45 $
// VERSION:		$Revision: 24 $
// LAST CHANGE:	$Modtime: 3.02.05 16:34 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCOutputTashaUnit_H_
#define _CIPCOutputTashaUnit_H_

#include "CipcOutput.h"
#include "TimedMessage.h"

#define OUTPUT_TYPE_CAMERA	1
#define OUTPUT_TYPE_RELAY	2

// Der Kontrastumfang/Saturation des Tasha Frontends haben in Wirklichkeit einen
// Wertebereich von 0....127!, Da z.Z. aber die Absolutwerte und nicht Prozentwerte
// an DTS/Idip verschickt werden, wird hier zu korrekten Umrechnung die gleichen Werte
// wie bei JaCob/SaVic angenommen. Sie dienen ausschlieﬂlich zur Umrechnung von Prozent
// auf den JaCob/SaVic Wertebereich und umgekehrt. Innerhalb der TashaUnit/Dlls wird
// alles in Prozent behandelt.
// Wertebereich der Helligkeit
#define MAX_BRIGHTNESS				255
#define MAX_CONTRAST				512
#define MAX_SATURATION				512

typedef struct
{
	DWORD			dwUserData;
	Resolution		Resolution;
	Compression		Compression;
	CompressionType	Ct;
	BOOL			bEncoderState;
	EncoderParam	EncParam;
}VideoStream;

class CCodec;	   
class CVideoJob;

class CIPCOutputTashaUnit : public CIPCOutput
{
public:
	void OnConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask);
	CIPCOutputTashaUnit(CTashaUnitDlg* pMainWnd, CCodec *pCodec, int iType, LPCTSTR shmName);
	~CIPCOutputTashaUnit();
	BOOL IsValid();

	//
	void OnRequestDisconnect();
   // RequestXXX
	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestSetRelay(CSecID relayID, BOOL bClosed);
	virtual void OnRequestCurrentState(WORD wGroupID);
	virtual void OnRequestHardware(WORD wGroupID);
	virtual void OnRequestSetGroupID(WORD wGroupID);

	virtual void OnRequestGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						DWORD dwUserData);

	virtual void OnRequestSetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwUserData);


	// 
	// PART: picture functions
	//
	// OnRequestXXX, virtual
	virtual	void OnRequestStartVideo(CSecID camID,			// camera nr.
									 Resolution res,		// resolution
									 Compression comp,		// image size or bitrate
									 CompressionType ct,	// JPEG, YUV, MPEG4 ...
									 int iFPS,
									 int iIFrameInterval, 
									 DWORD dwUserData		// unique server id
						 			);

	virtual void OnRequestStopVideo(CSecID camID, DWORD dwUserData);
	
	virtual void OnRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID);
	virtual void OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);

	// Evtl. Fehlermeldungen abfangen
	virtual void OnCommandSendError(DWORD dwCmd);

	void IndicationDataReceived(const DATA_PACKET* pData);
	void OnIndicationVideoStateChanged(WORD wCamSubID, BOOL bState);
	void OnIndicationVideoStreamStateChanged(WORD wCamSubID, BOOL bState);
	void OnIndicationSlaveInitReady(WORD wCamSubID);

	EncoderState  GetEncoderState();

	void OnConfirmSetMDMaskSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel);
	void OnConfirmGetMDMaskSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel);
	void OnConfirmSetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel);
	void OnConfirmGetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel);

	void OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue);

	void OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue);

	void OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose);

	void OnConfirmGetRelayState(DWORD dwRelayStateMask);
	void OnConfirmSetTerminationState(BYTE byTermMask);
	void OnConfirmGetTerminationState(BYTE byTermMask);
	
	LPBITMAPINFO CIPCOutputTashaUnit::CreateDIB(const DATA_PACKET* pPacket);

	void OnResetButtonPressed();

private:
	CCodec*			m_pCodec;
	CTashaUnitDlg*	m_pMainWnd;
	WORD			m_wGroupID;
	int				m_iType;
	DWORD			m_dwHardwareState;
	CEvent			m_evGetTermMask;
	CEvent			m_evSetTermMask;
	BYTE			m_byTermMask;

	BOOL			m_bOK;
	// picture data

	VideoStream		m_VideoStream[MAX_STREAMS][MAX_CAMERAS];

	ImageRes		m_eImageRes;
	
	DWORD			m_dwCrossPointMask;
	DWORD			m_dwOnCommandSendErrorCounter;

	LPBITMAPINFO	m_lpBMI;
};

#endif	// _CIPCOutputTashaUnit_H_

