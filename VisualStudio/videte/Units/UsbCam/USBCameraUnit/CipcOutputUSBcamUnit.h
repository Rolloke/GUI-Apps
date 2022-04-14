/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: CipcOutputUSBcamUnit.h $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCameraUnit/CipcOutputUSBcamUnit.h $
// CHECKIN:		$Date: 31.10.05 14:12 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 12.09.05 15:08 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCOutputUSBcamUnit_H_
#define _CIPCOutputUSBcamUnit_H_

#include "CipcOutput.h"
#include "TimedMessage.h"
//#include "cmotiondetection.h"

class CVideoJob;

class CIPCOutputUSBcamUnit : public CIPCOutput
{
public:
	CIPCOutputUSBcamUnit(CUSBCameraUnitDlg* pMainWnd, 
					   LPCTSTR shmName, BOOL bAsMaster=FALSE);
	~CIPCOutputUSBcamUnit();
	// Attributes
	BOOL IsConnected();
	// Operations
	BOOL DoReset() { return m_bReset;}
	void ConfirmReset();

   // RequestXXX
	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestCurrentState(WORD wGroupID);
	virtual void OnRequestHardware(WORD wGroupID);
	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestDisconnect();

	virtual void OnRequestGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						DWORD dwUserData);

	virtual void OnRequestSetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwUserData);


	virtual void OnConfirmJpegEncoding(const CIPCPictureRecord &pict,
								   	   DWORD dwUserData,
									   CSecID idArchive);


// Test to connect to tasha, jacob and savic unit
	virtual void OnConfirmSetGroupID(WORD wGroupID);
	virtual void OnConfirmReset(WORD wGroupID);
	virtual void OnConfirmHardware(WORD wGroupID,	
									int errorCode,	// 0==okay, !=0 some error
									BOOL bCanSWCompress,
									BOOL bCanSWDecompress,
									BOOL bCanColorCompress,
									BOOL bCanColorDecompress,
									BOOL bCanOverlay
									);
	virtual void OnConfirmCurrentState(WORD wGroupID, DWORD stateMask);

	virtual void OnIndicateVideo(const CIPCPictureRecord& pict,
								 DWORD dwMDX,
								 DWORD dwMDY,
								 DWORD dwUserData,
								 CSecID idArchive);



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

	void SelectCamera(CSecID camIDCurr, CSecID camIDNext, DWORD dwProzessID, CompressionType compType, DWORD dwPics);

	void OnReceivedMotionDIB(WORD wSource, HANDLE hDIB);
	void OnResetButtonPressed();

	// Connection
	virtual void OnReadChannelFound();
private:
	CUSBCameraUnitDlg*	m_pMainWnd;
	WORD					m_wGroupID;

	// see below in picture data Resolution	m_res;
	// see below in picture data Compression m_comp;
	// picture data

	Resolution		m_res;			// Gewählte Kameraauflösung
	Compression		m_comp;			// Gewählter Kompressionsgrad.
	WORD			m_wFormat;
	
	BYTE*			m_lpSmallTestbild;
	DWORD			m_dwSmallTestbildLen;

	BYTE*			m_lpLargeTestbild;
	DWORD			m_dwLargeTestbildLen;
	BOOL            m_bReset;
};

#endif	// _CIPCOutputUSBcamUnit_H_

