/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: CIPCOutputCoCoUnit.h $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/CIPCOutputCoCoUnit.h $
// CHECKIN:		$Date: 27.03.02 9:46 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 27.03.02 9:45 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCOutputCoCoUnit_H_
#define _CIPCOutputCoCoUnit_H_

#include "CipcOutput.h"
#include "TimedMessage.h"

#define OUTPUT_TYPE_COCO_CAMERA		1
#define OUTPUT_TYPE_COCO_RELAY		2
#define OUTPUT_TYPE_MEGRA_CAMERA	3

class CCoCo;
class CMegra;
class CVideoJob;

class CIPCOutputCoCoUnit : public CIPCOutput
{
public:
	CIPCOutputCoCoUnit(CCoCoUnitDlg* pMainWnd, CCoCo *pCoCo, CMegra *pMegra,
					   BYTE byCoCoID, int iType, const char *shmName);
	~CIPCOutputCoCoUnit();
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
	virtual void OnRequestSync(DWORD dwTickSend,DWORD dwType, DWORD dwUser);

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
	virtual void OnRequestSetOutputWindow(WORD wGroupID, const CRect &Rect,
								OverlayDisplayMode odm);


	virtual	void OnRequestEncodedVideo(CSecID camID, Resolution  res,
								Compression comp, WORD	wNumPictures,
								DWORD dwBitrateScale, DWORD dwUserData=0);

	virtual void OnRequestH263Decoding(WORD wGroupID, const CRect &rect,
									   const CIPCPictureRecord &Pict,
									   DWORD dwUserData);

	virtual void OnRequestStopH263Decoding(WORD wGroupID);
	
	virtual void OnRequestLocalVideo(CSecID camID, Resolution res, const CRect &rect, BOOL bActive);


	virtual void OnRequestDumpOutputWindow(WORD wGroupID, WORD wUserID,
				const CRect &outputRect, int iFormat);

	// Indications der 16Bit-Serverapplikation
	void IndicationDataReceived(WORD wExtCard, WORD wSource, LPCSTR lpData, DWORD dwLen, DWORD dwUserData, BOOL bIntra);
	void IndicationEncoderStarted();
	void IndicationEncoderStoped();
	void IndicationDecoderStarted();
	void IndicationDecoderStoped();
	void SelectCamera(CSecID camID);
	BOOL SendDataToDecoder(const CIPCPictureRecord *pPict);	
	BOOL EncoderStart(CVideoJob *pJob);
	BOOL EncoderStop(CVideoJob *pJob);
	BOOL DecoderStart(CVideoJob *pJob, int nJobCount = 0);
	BOOL DecoderStop(CVideoJob *pJob);
	BOOL LocalVideo(CVideoJob *pJob);

private:
	CCoCo*			m_pCoCo;
	CMegra*			m_pMegra;
	CCoCoUnitDlg*	m_pMainWnd;
	BYTE			m_byCoCoID;
	WORD			m_wGroupID;
	int				m_iType;
	DWORD			m_dwHardwareState;
	// see below in picture data Resolution	m_res;
	// see below in picture data Compression m_comp;
	BOOL			m_bOK;
	// picture data
	Resolution		m_res;			// Gewählte Kameraauflösung
	Compression		m_comp;			// Gewählter Kompressionsgrad.
	DWORD			m_dwBitrateScale;
	CSecID			m_camID;
	DWORD			m_dwEncBitrate;
	DWORD			m_dwDecBitrate;
	WORD			m_wMPI;
	WORD			m_wFormat;
	// HEDU added timed message
	CTimedMessage	m_tmInvalidEncoderResolutionInIndication;

	BOOL			m_bAllowHardDecode;
};

#endif	// _CIPCOutputCoCoUnit_H_