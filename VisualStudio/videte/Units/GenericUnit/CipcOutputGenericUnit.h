/////////////////////////////////////////////////////////////////////////////
// PROJECT:		GenericUnit
// FILE:		$Workfile: CipcOutputGenericUnit.h $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/CipcOutputGenericUnit.h $
// CHECKIN:		$Date: 19.10.00 8:46 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 19.10.00 8:26 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCOutputGenericUnit_H_
#define _CIPCOutputGenericUnit_H_

#include "CipcOutput.h"

#define OUTPUT_TYPE_GENERIC_CAMERA	1
#define OUTPUT_TYPE_GENERIC_RELAY		2

class CIPCOutputGenericUnit : public CIPCOutput
{
public:
	CIPCOutputGenericUnit(CGenericUnitDlg* pMainWnd, CCodec *pCodec,
					   int iType, const char *shmName, const CString &sAppIniName);
	~CIPCOutputGenericUnit();
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


	DWORD	GetAllRelaisState();
	void	SetAllRelaisState(DWORD dwRelaisState);

	// 
	// PART: picture functions
	//
	// OnRequestXXX, virtual
	virtual void OnRequestUnitJpegEncoding(WORD wGroupID,
										   BYTE byCam,
										   BYTE byNextCam,
										   Resolution res,
										   Compression comp,
										   int nNumPictures,
										   DWORD dwUserID);
	
	
	void OnRequestIdlePictures(int iNumPics);

	void OnRequestSync(DWORD dwTickSend,DWORD dwType, DWORD dwUser);

	void IndicationDataReceived(const JPEG* pJpeg, BOOL bSignal);
	void IndicationVideoState(WORD wCamSubID, BOOL bState);

	int  GetEncoderState();
	void SelectCamera(CSecID camIDCurr, CSecID camIDNext, DWORD dwProzessID, DWORD dwPics);

	BOOL EncoderStart(CVideoJob *pCurrentJob);
	BOOL EncoderStop(CVideoJob *pCurrentJob);

private:
	CCodec*			m_pCodec;
	CGenericUnitDlg*	m_pMainWnd;
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
	WORD			m_wFormat;
	
	DWORD			m_dwDefaultBPF[COMPRESSION_26+4];

	LPSTR			m_lpSmallTestbild;
	DWORD			m_dwSmallTestbildLen;

	LPSTR			m_lpLargeTestbild;
	DWORD			m_dwLargeTestbildLen;

};

#endif	// _CIPCOutputGenericUnit_H_

