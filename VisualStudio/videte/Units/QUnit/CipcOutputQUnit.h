/////////////////////////////////////////////////////////////////////////////
// PROJECT:		QUnit
// FILE:		$Workfile: CipcOutputQUnit.h $
// ARCHIVE:		$Archive: /Project/Units/QUnit/CipcOutputQUnit.h $
// CHECKIN:		$Date: 1.12.05 16:09 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 1.12.05 14:52 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCOutputQUnit_H_
#define _CIPCOutputQUnit_H_

#include "CipcOutput.h"
#include "TimedMessage.h"

#define OUTPUT_TYPE_CAMERA	1
#define OUTPUT_TYPE_RELAY		2

class CUDP;	   
class CVideoJob;

class CIPCOutputQUnit : public CIPCOutput
{
public:
	CIPCOutputQUnit(CQUnitDlg* pMainWnd, CUDP* pUDP,
					   int iType, LPCTSTR shmName, const CString &sAppIniName);
	~CIPCOutputQUnit();
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

	void IndicationDataReceived(const QIMAGE* pQImage);
	void OnIndicationVideoStateChanged(int nCamera, BOOL bState);

	int  GetEncoderState();

	void OnResetButtonPressed();

private:
	CUDP*			m_pUDP;
	CQUnitDlg*		m_pMainWnd;
	WORD			m_wGroupID;
	int				m_iType;
	DWORD			m_dwHardwareState;
	CString			m_sAppIniName;

	// see below in picture data Resolution	m_res;
	// see below in picture data Compression m_comp;
	BOOL			m_bOK;
	// picture data

	DWORD			m_dwDefaultBPF[COMPRESSION_26+4];

	BYTE*			m_lpSmallTestbild;
	DWORD			m_dwSmallTestbildLen;

	BYTE*			m_lpLargeTestbild;
	DWORD			m_dwLargeTestbildLen;

	VideoStream		m_VideoStream[MAX_CAMERAS];
};

#endif	// _CIPCOutputQUnit_H_

