/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: CipcOutputUSBcamUnit.h $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCamUnit/CipcOutputUSBcamUnit.h $
// CHECKIN:		$Date: 17.11.03 14:01 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 28.04.03 12:41 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCOutputUSBcamUnit_H_
#define _CIPCOutputUSBcamUnit_H_

#include "CipcOutput.h"
#include "TimedMessage.h"
#include "cmotiondetection.h"

class CVideoJob;

class CIPCOutputUSBcamUnit : public CIPCOutput
{
public:
	CIPCOutputUSBcamUnit(CUSBCamUnitDlg* pMainWnd, 
					   LPCTSTR shmName, const CString &sAppIniName);
	~CIPCOutputUSBcamUnit();
	BOOL IsValid();

	//
	void OnRequestDisconnect();
   // RequestXXX
	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual void OnRequestReset(WORD wGroupID);	
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
	CUSBCamUnitDlg*	m_pMainWnd;
	WORD					m_wGroupID;
	DWORD					m_dwHardwareState;
	CString				m_sAppIniName;

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

	BOOL			m_bAllowHardDecode;	// TRUE -> USBCam kann decodieren.

	BYTE*			m_lpSmallTestbild;
	DWORD			m_dwSmallTestbildLen;

	BYTE*			m_lpLargeTestbild;
	DWORD			m_dwLargeTestbildLen;
};

#endif	// _CIPCOutputUSBcamUnit_H_

