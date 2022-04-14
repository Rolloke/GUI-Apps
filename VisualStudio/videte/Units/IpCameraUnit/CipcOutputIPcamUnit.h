/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: CipcOutputIPcamUnit.h $
// ARCHIVE:		$Archive: /Project/Units/IPCameraUnit/CipcOutputIPcamUnit.h $
// CHECKIN:		$Date: 20.06.06 13:34 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 20.06.06 12:34 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CipcOutput.h"
#include "TimedMessage.h"
//#include "cmotiondetection.h"

class CCameraParameterDlg;
class CIpCameraUnitDlg;

class CIPCOutputIPcamUnit : public CIPCOutput
{
public:
	CIPCOutputIPcamUnit(CIpCameraUnitDlg* pMainWnd, 
					   LPCTSTR shmName, BOOL bAsMaster=FALSE);
	~CIPCOutputIPcamUnit();
	// Attributes
	BOOL IsConnected();
	inline WORD GetGroupID() const;
	inline const CStringArray& GetRelais();
	inline const DWORD GetRelaisState();

	// Operations
	inline BOOL DoReset() const;
	void ConfirmReset();
	BOOL CheckGroupID(WORD wGid, LPCTSTR sFunction);
	void InitRelaisStrings();
	BOOL DoRequestRelais(const CString& sRelais, BOOL bRequestValue, BOOL bClosed);

#ifdef  _TEST_SV_DLG
	void SetParamDlg(CCameraParameterDlg*pDlg);
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmSetGroupID(WORD wGroupID);
	virtual void OnConfirmGetValue(CSecID id, const CString &sKey,
						const CString &sValue,
						DWORD dwUserData);
	virtual void OnConfirmSetValue(CSecID id, const CString &sKey,
						const CString &sValue,
						DWORD dwUserData);
#endif

   // RequestXXX
	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestHardware(WORD wGroupID);
	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestDisconnect();
	virtual void OnRequestGetValue(CSecID id, const CString &sKey, DWORD dwUserData);
	virtual void OnRequestSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData);

	virtual void OnRequestCurrentState(WORD wGroupID);
	virtual void OnRequestSetRelay(CSecID relayID, BOOL bClosed);

	// 
	// PART: picture functions
	//
	// OnRequestXXX, virtual
	virtual void OnRequestSetDisplayWindow(WORD wGroupID, const CRect &rect);
	virtual void OnRequestSetOutputWindow(WORD wGroupID, const CRect &rect, OverlayDisplayMode odm);

	virtual	void OnRequestStartVideo(CSecID id,			// camera nr.
									 Resolution res, Compression comp, CompressionType ct,
									 int iFPS, int iIFrameInterval, DWORD dwUserData);
	virtual void OnRequestStopVideo(CSecID camID, DWORD dwUserData);

	virtual void OnRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID);
	virtual void OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);
	virtual void OnConfirmReset(WORD wGroupID);

	void IndicationVideoState(WORD wCamSubID, BOOL bState);

	void OnResetButtonPressed();

	// Connection
private:
	CIpCameraUnitDlg*		m_pDlg;
	WORD					m_wGroupID;
	BOOL					m_bReset;
	DWORD					m_dwRelaisState;
	CStringArray			m_saRelais;
#ifdef  _TEST_SV_DLG
	CCameraParameterDlg		*m_pParamDlg;
#endif


//	BYTE*			m_lpSmallTestbild;
//	DWORD			m_dwSmallTestbildLen;

//	BYTE*			m_lpLargeTestbild;
//	DWORD			m_dwLargeTestbildLen;
};

inline WORD CIPCOutputIPcamUnit::GetGroupID() const
{
	return m_wGroupID;
}
	// Operations
inline BOOL CIPCOutputIPcamUnit::DoReset() const
{
	return m_bReset;
}

inline const CStringArray& CIPCOutputIPcamUnit::GetRelais() 
{
	return m_saRelais; 
}

inline const DWORD CIPCOutputIPcamUnit::GetRelaisState() 
{
	return m_dwRelaisState; 
}
