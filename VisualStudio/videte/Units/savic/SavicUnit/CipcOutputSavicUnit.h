/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: CipcOutputSavicUnit.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/CipcOutputSavicUnit.h $
// CHECKIN:		$Date: 20.06.03 13:15 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 20.06.03 12:22 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCOutputSaVicUnit_H_
#define _CIPCOutputSaVicUnit_H_

#include "CipcOutput.h"
#include "TimedMessage.h"

#define OUTPUT_TYPE_SAVIC_CAMERA	1
#define OUTPUT_TYPE_SAVIC_RELAY		2

class CCodec;	   
class CVideoJob;

class CIPCOutputSaVicUnit : public CIPCOutput
{
public:
	CIPCOutputSaVicUnit(CSaVicUnitDlg* pMainWnd, CCodec *pCodec,
					   int iType, LPCTSTR shmName, const CString &sAppIniName);
	~CIPCOutputSaVicUnit();
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

	void OnRequestSync(DWORD dwTickSend, DWORD dwType, DWORD dwUser);

	virtual void OnRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID);
	virtual void OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);

	void IndicationDataReceived(const IMAGE* pImage, BOOL bSignal);
	void IndicationVideoState(WORD wCamSubID, BOOL bState);

	int  GetEncoderState();
	void SelectCamera(CSecID camIDCurr, CSecID camIDNext, DWORD dwProzessID, CompressionType compType, DWORD dwPics);

	BOOL EncoderStart(CVideoJob *pCurrentJob);
	BOOL EncoderStop(CVideoJob *pCurrentJob);

	void OnReceivedMotionDIB(WORD wSource, HANDLE hDIB);
	void OnResetButtonPressed();

private:
	CCodec*			m_pCodec;
	CSaVicUnitDlg*	m_pMainWnd;
	WORD			m_wGroupID;
	int				m_iType;
	DWORD			m_dwHardwareState;
	CString			m_sAppIniName;

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

	BOOL			m_bAllowHardDecode;	// TRUE -> SaVic kann decodieren.

	BYTE*			m_lpSmallTestbild;
	DWORD			m_dwSmallTestbildLen;

	BYTE*			m_lpLargeTestbild;
	DWORD			m_dwLargeTestbildLen;
};

#endif	// _CIPCOutputSaVicUnit_H_

