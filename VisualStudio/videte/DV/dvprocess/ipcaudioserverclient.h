// IPCAudioServerClient.h: interface for the CIPCAudioDVClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCAUDIOSERVERCLIENT_H__1CA6371D_A9E6_4A89_8EE6_DA8ADAB7B941__INCLUDED_)
#define AFX_IPCAUDIOSERVERCLIENT_H__1CA6371D_A9E6_4A89_8EE6_DA8ADAB7B941__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCAudioServer;
class CAudioClient;

class CIPCAudioDVClient : public CIPCMedia  
{
public:
	CIPCAudioDVClient(CAudioClient* pClient, LPCTSTR shmName);
	virtual ~CIPCAudioDVClient();

public:
	BOOL IsTimedOut();
	BOOL IsAudioInActive();
	inline DWORD	GetOptions() const;


public:
	// info
	virtual void OnRequestInfoMedia(WORD wGroupID);

public:
	// the real data
	virtual void OnRequestStartMediaEncoding(CSecID mediaID, DWORD dwFlags, DWORD dwUserData);
	virtual void OnRequestStopMediaEncoding(CSecID mediaID, DWORD dwFlags);

	virtual void OnRequestMediaDecoding(const CIPCMediaSampleRecord&, CSecID mediaID, DWORD dwFlags);
	virtual void OnRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags);

	virtual void OnRequestHardware(CSecID secID, DWORD dwIDRequest);

	virtual void OnRequestValues(CSecID secID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData);

	virtual void OnRequestDisconnect();
	virtual void OnRequestGetValue(CSecID id,
								   const CString &sKey,
								   DWORD dwServerData);
	virtual void OnRequestSetValue(CSecID id,
								   const CString &sKey,
								   const CString &sValue,
								   DWORD dwServerData);

protected:

private:
	CAudioClient* m_pClient;
	DWORD	m_dwOptions;
	DWORD	m_dwTime;
	BOOL	m_bAudioInActive;

public:
	static BOOL gm_bTraceDoClientEncoding;
	static BOOL gm_bTraceOnRequestStartMediaEncoding;
	static BOOL gm_bTraceOnRequestStopMediaEncoding;
	static BOOL gm_bTraceRequestMediaDecoding;
	static BOOL gm_bTraceOnRequestStopMediaDecoding;
	static BOOL gm_bTraceOnRequestValues;
};
///////////////////////////////////////////////////////////////////////
inline DWORD CIPCAudioDVClient::GetOptions() const
{
	return m_dwOptions;
}

#endif // !defined(AFX_IPCAUDIOSERVERCLIENT_H__1CA6371D_A9E6_4A89_8EE6_DA8ADAB7B941__INCLUDED_)
