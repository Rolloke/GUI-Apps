/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CipcOutputTashaUnit.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/TashaUnit/CipcOutputTashaUnit.h $
// CHECKIN:		$Date: 5.01.04 9:54 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCOutputTashaUnit_H_
#define _CIPCOutputTashaUnit_H_

#include "CipcOutput.h"
#include "TimedMessage.h"

#define OUTPUT_TYPE_CAMERA	1
#define OUTPUT_TYPE_RELAY	2

class CCodec;	   
class CVideoJob;

class CIPCOutputTashaUnit : public CIPCOutput
{
public:
	void OnConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask);
	CIPCOutputTashaUnit(CTashaUnitDlg* pMainWnd, CCodec *pCodec,
					   int iType, const char *shmName);
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
	virtual void OnRequestSetDisplayWindow(WORD wGroupID, const CRect &rect);
	virtual void OnRequestSetOutputWindow(WORD wGroupID, const CRect &rect, OverlayDisplayMode odm);
	virtual void OnRequestUnitVideo(WORD wGroupID,
								    BYTE byCam,
								    BYTE byNextCam,
								    Resolution res,
									Compression comp,
									CompressionType ct,
									int nNumPictures,
									DWORD dwUserID);
	
	
	void OnRequestIdlePictures(int iNumPics);

	void OnRequestSync(DWORD dwTickSend,DWORD dwType, DWORD dwUser);

	virtual void OnRequestGetMask(CSecID camID, DWORD dwUserID);
	virtual void OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);

	void IndicationDataReceived(const DATA_PACKET* pData, BOOL bSignal);
	void IndicationVideoState(WORD wCamSubID, BOOL bState);

	EncoderState  GetEncoderState();

	BOOL EncoderStart(CVideoJob *pCurrentJob);
	BOOL EncoderStop(CVideoJob *pCurrentJob);
	BOOL DecoderStart(CVideoJob *pCurrentJob);
	BOOL DecoderStop(CVideoJob *pCurrentJob);

	void OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue);

	void OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue);

	void OnConfirmGetMDTreshold(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmGetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmGetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmGetMaskDelay(DWORD dwUserData, WORD wSource, int nValue);

	void OnConfirmSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue);
	void OnConfirmClearPermanentMask(DWORD dwUserData, WORD wSource);
	void OnConfirmInvertPermanentMask(DWORD dwUserData, WORD wSource);

	void OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose);

	void OnConfirmGetRelayState(DWORD dwRelayStateMask);
	
	LPBITMAPINFO CIPCOutputTashaUnit::CreateDIB(const DATA_PACKET* pPacket);

private:
	CCodec*			m_pCodec;
	CTashaUnitDlg*	m_pMainWnd;
	WORD			m_wGroupID;
	int				m_iType;
	DWORD			m_dwHardwareState;

	// see below in picture data Resolution	m_res;
	// see below in picture data Compression m_comp;
	BOOL			m_bOK;
	// picture data

	Resolution		m_res;			// Gewählte Kameraauflösung
	Compression		m_comp;			// Gewählter Kompressionsgrad.
	CSecID			m_camID;
	ImageRes		m_eImageRes;
	
	LPSTR			m_lpSmallTestbild;
	DWORD			m_dwSmallTestbildLen;

	LPSTR			m_lpLargeTestbild;
	DWORD			m_dwLargeTestbildLen;

	LPBITMAPINFO	m_lpBMI;
};

#endif	// _CIPCOutputTashaUnit_H_

