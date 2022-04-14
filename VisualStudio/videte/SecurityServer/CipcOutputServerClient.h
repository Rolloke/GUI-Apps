/* GlobalReplace: m_iClientPicsPerSecond --> m_iCoCoClientPicsPerSecond */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcOutputServerClient.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/CipcOutputServerClient.h $
// CHECKIN:		$Date: 24.08.06 9:21 $
// VERSION:		$Revision: 95 $
// LAST CHANGE:	$Modtime: 23.08.06 20:33 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _CIPCOutputServerClient_H
#define _CIPCOutputServerClient_H

#include "CIPCOutput.h"
#include "CIPCServerControl.h"	// for CConnectionRecord

#include "Process.h"
#include "encoderthread.h"
#include "GOPSendThread.h"

class CIPCServerControlServerSide;
class CTranscoderThread;

class CIPCOutputServerClient : public CIPCOutput
{
public:
	CIPCOutputServerClient(const CConnectionRecord &c,
							CIPCServerControlServerSide *pControl,
							LPCTSTR shmName, 
							WORD wNr,
							BOOL bAlarm,
							const CString& sRemoteName);
	virtual ~CIPCOutputServerClient();
	//
	static BOOL m_bAllowAnyOverlayClient;
	static BOOL m_bTraceClientJpegRequests;

	static int m_iCoCoClientPicsPerSecond;
	// access:
	inline DWORD	GetOptions() const;
	inline void		SetOptions(DWORD dwOptions);
	inline void		SetConnectionTime(DWORD dwTime);
	inline void		SetBitrate(DWORD dwBitrate);
	inline CSecID	GetID() const;
	inline DWORD	GetPriority() const;
	inline CSecID   GetPermissionID() const;
	inline CString	GetRemoteHost() const;
	inline CString	GetSerial() const;
	inline DWORD	GetBitrate() const;
	inline const CConnectionRecord &GetConnection() const;
	inline BOOL		IsLocal() const;
		   BOOL		UseH263();
	inline BOOL		CanMpeg4() const;
	inline BOOL		CanGOP() const;
	inline BOOL		IsLowBandwidth() const;
		   BOOL		IsStreaming();
		   BOOL		IsMultiCamera();
	
	inline BOOL		IsTimedOut() const;
	inline BOOL		IsConnectionTimedOut();
	inline BOOL		IsAlarm() const;

	//
	virtual BOOL	OnTimeoutWrite(DWORD dwCmd);		// no CmdDone response, OOPS called before WriteCmd
	virtual BOOL	OnTimeoutCmdReceive();	// no new cmd received
	//
	virtual void OnRequestConnection();
	virtual void OnRequestDisconnect();
	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual void OnRequestInfoOutputs(WORD wGroupID);
	virtual void OnRequestSetRelay(CSecID relayID, BOOL bClosed);
	// JPEG
	virtual void OnRequestNewJpegEncoding(CSecID	camID, 
										Resolution	res, 
										Compression comp,
										int iNumPictures,
										DWORD		dwUserID
										);
	virtual	void OnRequestStartVideo(CSecID id,			// camera nr.
									Resolution res,		// resolution
									Compression comp,		// image size or bitrate
									CompressionType ct,	// JPEG, YUV, MPEG4 ...
									int iFPS,
									int iIFrameInterval, 
									DWORD dwUserData		// unique server id
									);
	virtual void OnRequestStopVideo(CSecID camID,DWORD dwUserData);
	virtual void OnRequestJpegDecoding(WORD wGroupID, 
										Resolution res, 
										const CIPCExtraMemory &data,
										DWORD dwUserData
										);
	// H263
	virtual void OnRequestSetDisplayWindow(WORD wGroupID, const CRect &rect);
	virtual void OnRequestSetOutputWindow (WORD wGroupID, 
											const CRect &rect,
											OverlayDisplayMode odm);
	//
	virtual void OnIndicateClientActive(BOOL bClientIsActive, WORD wGroupID);
	//
	virtual void OnRequestStartH263Encoding(CSecID camID, 
											Resolution res, 
											Compression comp,
											DWORD dwBitrate,
											DWORD dwRecordTime
											);
	virtual void OnRequestStopH263Encoding(CSecID camID);
	virtual void OnRequestH263Decoding(WORD wGroupID, 
										const CRect &rect,
										const CIPCPictureRecord &pict,
										DWORD dwUserData
										);
	virtual void OnRequestStopH263Decoding(WORD wGroupID, const CRect &rect);
	// values:
	virtual void OnRequestGetValue(CSecID id, const CString &sKey,DWORD dwServerData);
	virtual void OnRequestSetValue(CSecID id,const CString &sKey,const CString &sValue,DWORD dwServerData);
	//
	virtual void OnRequestSync(DWORD dwTickSend, DWORD dwType,DWORD dwUserID);
	//
	virtual void OnRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID);
	virtual void OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask);
	//
	void RemoveProcess(CProcess *pProcess);	// called from the according client process

	// operations
public:
	void SendPicture(const CIPCPictureRecord& pict,CompressionType ct,Compression comp,DWORD dwMDX, DWORD dwMDY);
	BOOL SelectCamera(CSecID camID);
	inline CEncoderThread* GetEncoderThread();
	void SetFetchResult(CIPCFetchResult& fr);
	void CheckNagScreen();
	//
	DWORD m_remotePID;
private:
	void DoClientEncoding(CSecID camID, 
							Resolution	res, 
							Compression comp,
							CompressionType ct,
							int iNumPictures,
							DWORD dwTimeSlice,
							DWORD dwBitrateScale,
							BOOL bTranscode);
	void CreateTranscoder(int iNumPictures,DWORD dwCalculatedBitrate);
	void DestroyTranscoder();
	void InitH263Mpeg4();
	CString CheckNotifiableErrors();
	CString CheckMaintenance();
	CString GetMaintenanceText();


	CConnectionRecord				m_ConnectionRecord;
	BOOL m_bIsLocal;
	BOOL m_bH263;
	BOOL m_bMpeg4;
	BOOL m_bCanGOP;
	CIPCServerControlServerSide*	m_pControl;

	DWORD	m_dwPriority;
	CSecID	m_permissionID;
	DWORD	m_dwOptions;
	DWORD	m_dwConnectionTime;
	volatile BOOL	m_bTimeoutDisconnected;
	CSecID	m_id;
	CSecID	m_idDecoderBW;
	CSecID	m_idDecoderColor;
	CString m_sSerial;
	CString m_sVersion;
	CString m_sRemoteMSN;
	CString m_sRemoteIP;
	DWORD   m_dwRemoteBitrate;
	BOOL	m_bAlarm;
	//
	CProcesses  m_Processes;
	DWORD		m_dwStarttimeForNotConnectedTimeout;
	DWORD		m_dwNotConnectedTimeout;
	BOOL		m_bInfoConfirmed;
	DWORD	    m_dwTickInfoConfirmed;

	CCriticalSection   m_csTranscoder;
	CTranscoderThread* m_pTranscoder;
	DWORD			   m_dwSelectCameraTick;
	CEncoderThread*	   m_pEncoder;
	CGOPSendThread	   m_GOPSender;
};

typedef CIPCOutputServerClient * CIPCOutputServerClientPtr;

WK_PTR_ARRAY_CS(CIPCOutputClientsArrayPlain,CIPCOutputServerClientPtr,CIPCOutputClientsArraySafe)

class CIPCOutputClientsArray : public CIPCOutputClientsArraySafe
{
public:
	CIPCOutputClientsArray();
	CIPCOutputServerClient *GetClientByID(CSecID id) const;
	CIPCOutputServerClient *GetClientBySubID(WORD wSubID) const;
	BOOL IsConnected(const CString& sSerial,const CString& sSourceHost);
	int	 GetNrOfLicensed();
};

/*@MD nomen est omen*/
inline DWORD CIPCOutputServerClient::GetOptions() const
{
	return m_dwOptions;
}
inline void	CIPCOutputServerClient::SetOptions(DWORD dwOptions)
{
	m_dwOptions = dwOptions;
	m_bCanGOP = (m_dwOptions & SCO_CAN_GOP)>0;
}
inline void	CIPCOutputServerClient::SetBitrate(DWORD dwBitrate)
{
	m_dwRemoteBitrate = dwBitrate;
}
/*@MD nomen est omen*/
inline CSecID CIPCOutputServerClient::GetID() const
{
	return m_id;
}

/*@MD nomen est omen*/
inline DWORD CIPCOutputServerClient::GetPriority() const
{
	return m_dwPriority;
}

inline CString CIPCOutputServerClient::GetRemoteHost() const
{
	return m_ConnectionRecord.GetSourceHost();
}

inline DWORD CIPCOutputServerClient::GetBitrate() const
{
	return m_dwRemoteBitrate;
}
inline const CConnectionRecord& CIPCOutputServerClient::GetConnection() const
{
	return m_ConnectionRecord;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CIPCOutputServerClient::IsTimedOut() const
{
	if (   GetIPCState() == CIPC_STATE_READ_FOUND
		|| GetIPCState() == CIPC_STATE_WRITE_CREATED
		|| GetIPCState() == CIPC_STATE_CONNECTED)
	{
		if (!m_bInfoConfirmed)
		{
			return (GetTimeSpan(m_dwStarttimeForNotConnectedTimeout) >= m_dwNotConnectedTimeout);
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCOutputServerClient::IsConnectionTimedOut()
{
	if (   GetIPCState() == CIPC_STATE_READ_FOUND
		|| GetIPCState() == CIPC_STATE_WRITE_CREATED
		|| GetIPCState() == CIPC_STATE_CONNECTED)
	{
		if (   m_bInfoConfirmed
			&& m_dwTickInfoConfirmed != 0
			&& m_dwConnectionTime>0)
		{
			m_bTimeoutDisconnected = (GetTimeSpan(m_dwTickInfoConfirmed) >= m_dwConnectionTime*1000);
		}
	}
	return m_bTimeoutDisconnected;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCOutputServerClient::IsLocal() const
{
	return m_bIsLocal;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCOutputServerClient::IsAlarm() const
{
	return m_bAlarm;
}
/////////////////////////////////////////////////////////////////////////////
inline void	CIPCOutputServerClient::SetConnectionTime(DWORD dwTime)
{
	m_dwConnectionTime = dwTime;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CIPCOutputServerClient::GetSerial() const
{
	return m_sSerial;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCOutputServerClient::CanMpeg4() const
{
	return m_bMpeg4;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCOutputServerClient::CanGOP() const
{
	return m_bCanGOP;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCOutputServerClient::IsLowBandwidth() const
{
	return (m_dwRemoteBitrate <= 128*1024);
}
/////////////////////////////////////////////////////////////////////////////
inline CEncoderThread* CIPCOutputServerClient::GetEncoderThread()
{
	return m_pEncoder;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CIPCOutputServerClient::GetPermissionID() const
{
	return m_permissionID;
}


#endif
