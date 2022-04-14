/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CipcOutputTashaUnit.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/CipcOutputTashaUnit.h $
// CHECKIN:		$Date: 6.12.01 11:40 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 6.12.01 11:39 $
// BY AUTHOR:	$Author: Martin $
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
	virtual void OnRequestUnitJpegEncoding(WORD wGroupID,
										   BYTE byCam,
										   BYTE byNextCam,
										   Resolution res,
										   Compression comp,
										   int nNumPictures,
										   DWORD dwUserID);
	
	
	void OnRequestIdlePictures(int iNumPics);

	void OnRequestSync(DWORD dwTickSend,DWORD dwType, DWORD dwUser);

	virtual void OnRequestGetMask(CSecID camID, DWORD dwUserID);
	virtual void OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);

	void IndicationDataReceived(const DataPacket* pData, BOOL bSignal);
	void IndicationVideoState(WORD wCamSubID, BOOL bState);

	EncoderState  GetEncoderState();
	void SelectCamera(CSecID camIDCurr, CSecID camIDNext, DWORD dwProzessID, DWORD dwPics);

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

	void OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose);

	void OnConfirmGetRelayState(DWORD dwRelayStateMask);
	void OnConfirmGetInputSource(WORD wSource);

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
	DWORD			m_dwBPF;
	ImageRes		m_eImageRes;
	
	DWORD			m_dwDefaultBPF[COMPRESSION_26+4];

	BOOL			m_bAllowHardDecode;	// TRUE -> Tasha kann decodieren.

	LPSTR			m_lpSmallTestbild;
	DWORD			m_dwSmallTestbildLen;

	LPSTR			m_lpLargeTestbild;
	DWORD			m_dwLargeTestbildLen;

};

#endif	// _CIPCOutputTashaUnit_H_

