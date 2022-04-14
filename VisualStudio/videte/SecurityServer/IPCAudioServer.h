// IPCAudioServer.h: interface for the CIPCAudioServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCAUDIOSERVER_H__25321CE1_F9F9_4097_B86D_48139BFC04D3__INCLUDED_)
#define AFX_IPCAUDIOSERVER_H__25321CE1_F9F9_4097_B86D_48139BFC04D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProcessSchedulerAudio;
class CIPCAudioServerClient;

class CIPCAudioServer : public CIPCMedia, public CIOGroup 
{
	// construction/destruction
public:
	CIPCAudioServer(LPCTSTR pName, WORD wID, LPCTSTR pSMName);
	virtual ~CIPCAudioServer();

	// attributes
public:
	inline  CProcessSchedulerAudio*	GetProcessScheduler() const;
	inline  DWORD GetHardwareStateFlags() const;
	// operations

	void DoRequestMediaDecoding(CIPCAudioServerClient*pBCC, const CIPCMediaSampleRecord& media, CSecID mediaID, DWORD dwFlags);
	void DoRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags);
public:
	BOOL   Load(CWK_Profile& wkp, int iSize);
	CSecID GetDecoderID() const;

	// cipc communication
public:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnRequestDisconnect();
	virtual void OnConfirmSetGroupID(WORD wGroupID);

	virtual void OnConfirmHardware(CSecID secID, int errorCode, DWORD dwFlags, DWORD dwIDRequest, const CIPCExtraMemory *pEM);
	virtual void OnConfirmReset(WORD wGroupID);
	virtual void OnConfirmValues(CSecID, DWORD, DWORD, DWORD dwValue  , const CIPCExtraMemory*pData);

	// the data
public:
	virtual void OnIndicateMediaData(const CIPCMediaSampleRecord& media, CSecID mediaID, DWORD dwUserData);

private:
	CProcessSchedulerAudio* m_pProcessScheduler;
	CSecID m_DecoderID;
	CIPCAudioServerClient *m_pBroadcastingClient;
	DWORD  m_dwHardwareStateFlags;
public:
	static BOOL gm_bTraceConfirmValues;
	static BOOL gm_bTraceIndicateMediaData;
};
inline CProcessSchedulerAudio* CIPCAudioServer::GetProcessScheduler() const
{
	return m_pProcessScheduler;
}
inline DWORD CIPCAudioServer::GetHardwareStateFlags() const
{
	return m_dwHardwareStateFlags;
}
typedef CIPCAudioServer* CIPCAudioServerPtr;
WK_PTR_ARRAY_CS(CIPCAudioServerArrayPlain,CIPCAudioServerPtr,CIPCAudioServerArraySafe)
/////////////////////////////////////////////////////////////////////////////
class CAudioList : public CIPCAudioServerArraySafe 
{
	// construction/destruction
public:
			CAudioList();
	virtual ~CAudioList();

	// attributes
public:
	CIPCAudioServer*		GetGroupByID(CSecID id) const;
//	CMedia*					GetAudioByID(CSecID id);		nicht mehr benutzt
	CString					GetAudioNameByID(CSecID id);
	inline CIPCAudioServer*	GetGroupAt(int i) const;
	CIPCAudioServer*		GetDecoder() const;
	// operations
public:
	void Load(CWK_Profile& wkp);
	void StartThreads();

private:
	CIPCAudioServer* m_pDecoder;
};
/////////////////////////////////////////////////////////////////////////////
inline CIPCAudioServer*	CAudioList::GetGroupAt(int i) const 
{ 
	if (i<GetSize()) 
	{
		return GetAt(i);
	} 
	else 
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_IPCAUDIOSERVER_H__25321CE1_F9F9_4097_B86D_48139BFC04D3__INCLUDED_)
