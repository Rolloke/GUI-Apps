
#ifndef _CPTBox_H
#define _CPTBox_H

#include "PictureDef.h"

#include "pt.h"

#include "RawDataArray.h"
#include "PTCamera.h"
#include "PTAlarm.h"

// @doc

// @enum state of a <CPTBox>.
enum PTConnectionState {
	PCS_NOT_CONNECTED = 0,	// @emem NYD
	PCS_CALLING=1,			// @emem NYD
	PCS_INITIALIZING=2,		// @emem NYD
	PCS_CONNECTED=3,		// @emem NYD
	PCS_CLOSING=4			// @emem NYD
};

extern LPCTSTR NameOfEnum(PTConnectionState s);


class CISDNConnection;
class CIPCPipeInputPT;
class CIPCPipeOutputPT;

// @class CPTBox
class  CPTBox 
{
public:
	static int m_iFrameCount;
	static BOOL m_bDoHexDumpSend;
	static BOOL m_bDoHexDumpReceive;
	static BOOL m_bDisableFrameGrabbing;
	static BOOL m_bDoTraceFrameSize;
	static BOOL m_bDisableEEPWrite;
	static BOOL m_bLowAsMid;

public:
	CPTBox(CISDNConnection *pConnection);
	~CPTBox();
	//
	inline int GetVersion() const;	// 100 or 200
	inline DWORD GetRelayBits() const;
	//
	void OnRawData(const BYTE *pData,DWORD dwDataLen);
	//
	void SetPTConnectionState(PTConnectionState newState);
	void ClearInternalData();

	void SendPTCmd(BYTE byCmd, DWORD dwDataLen, const BYTE *pData);
	void CheckForPTCmd();
	void OnPTCmd(const BYTE *pFullData, DWORD dwFullLen);
	void OnPTFrame(const BYTE *pFrameData, DWORD dwFrameLen);
	//
	void SendPTConfig();
	void HandlePTBoxInfo(const BYTE *pData,DWORD dwDataLen);
	void HandlePTBoxConfig(const BYTE *pData,DWORD dwDataLen);
	void HandleAlarmState(const BYTE *pData,DWORD dwDataLen);
	void HandlePTError(const BYTE *pData,DWORD dwDataLen);
	void HandlePTRemoteIn(const BYTE *pData,DWORD dwDataLen);
	void HandlePTRemoteOut(const BYTE *pData,DWORD dwDataLen);

	// access:
	inline PTConnectionState GetPTConnectionState() const;
	inline DWORD GetLastStateChange() const;
	inline const CPTCameras & GetCameras() const;
	inline const CPTAlarms & GetAlarms() const;
	
	// actions
	void DoSelectCamera(CSecID camID, BOOL bActive, Resolution res);
	void DoSetRelay(CSecID relayID, BOOL bClosed);
	void DoConfirmAlarm(CSecID inputID);
	void DoSetTime(const CSystemTime& st);

	void DoRequestCameraValue(CSecID camID, const CString &sKey);

	// internal data
	void DoSetCameraName(int ix, const CString &sNewName);
	void DoSetResolution(CSecID camID, Resolution res, BOOL bPermanent);
	void DoSetCameraValue(CSecID camID, const CString &sKey, const CString &sValue);
	void DoSetHostName(const CString &sNewName);
	void DoWriteEEP();	
	
	//
	inline const CString & GetBoxName() const;
	inline const CString & GetBoxNumber() const;
	//
	inline int GetNumOutstandingCmds() const;

	// on pt cmds
	void	OnAcknowledgeCamera(const TCMDSetCamera& ackcamera);


public:
	// for direct CPTBox access
	CIPCPipeInputPT *m_pInputPT;
	CIPCPipeOutputPT *m_pOutputPT;
	//
	CSecID	m_alarmCamID;

private:
	//
	void DoGrabFrame(BOOL bForceIntra);
	void DecodePictureTime(const BYTE *pFrameData, DWORD dwFrameDataLen);
	//
	CISDNConnection *m_pConnection;
	PTConnectionState m_PTState;
	DWORD	m_dwLastStateChange;
	CRawDataArray m_rawData;	// received data
	BYTE m_byPictureType;	// from 0xD3 AckPicture
	//
	BYTE m_byPicTimeSecond;
	BYTE m_byPicTimeMinute;
	BYTE m_byPicTimeHour;
	BYTE m_byPicTimeDay;
	BYTE m_byPicTimeMonth;
	WORD m_wPicTimeYear;
	//
	CSecID m_requestedCamID;	// SECID_NO_ID to stop grabbing
	Resolution m_requestedResolution;
	BOOL m_bRequestForceIntra;
	//
	DWORD m_dwRelayBits;
	//
	int m_iVersion;
	//
	CPTCameras m_cameras;
	CPTAlarms m_alarms;
	//
	CString m_sVersion;
	CString m_sName;
	CString m_sNumber;
	//
	int m_NumOutstandingCmds;
};

inline PTConnectionState CPTBox::GetPTConnectionState() const
{
	return m_PTState;
}

inline int CPTBox::GetVersion() const
{
	return m_iVersion;
}
inline DWORD CPTBox::GetRelayBits() const
{
	return m_dwRelayBits;
}

inline const CPTCameras & CPTBox::GetCameras() const
{
	return m_cameras;
}
inline const CPTAlarms & CPTBox::GetAlarms() const
{
	return m_alarms;
}

inline const CString & CPTBox::GetBoxName() const
{
	return m_sName;
}
inline const CString & CPTBox::GetBoxNumber() const
{
	return m_sNumber;
}
inline DWORD CPTBox::GetLastStateChange() const
{
	return m_dwLastStateChange;
}

inline int CPTBox::GetNumOutstandingCmds() const
{
	return m_NumOutstandingCmds;
}


#endif