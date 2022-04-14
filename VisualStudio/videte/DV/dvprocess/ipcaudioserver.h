// IPCAudioServer.h: interface for the CIPCAudioServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCAUDIOSERVER_H__25321CE1_F9F9_4097_B86D_48139BFC04D3__INCLUDED_)
#define AFX_IPCAUDIOSERVER_H__25321CE1_F9F9_4097_B86D_48139BFC04D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "media.h"

class CIPCAudioDVClient;

class CIPCAudioServer : public CIPCMedia, public CMediaArrayCS
{
	// construction/destruction
public:
	CIPCAudioServer(WORD wID, LPCTSTR pSMName, BOOL bIn, BOOL bOut);
	virtual ~CIPCAudioServer();

	// attributes
public:
	inline WORD	GetID() const;
	inline int	GetHardwareState() const;
	inline BOOL	ResetConfirmed() const;
	inline DWORD GetRecordingFlags() const;

	// operations
public:
	void   Load(CWK_Profile& wkp);
	CSecID GetDecoderID() const;
	CSecID GetEncoderID() const;
	CMedia* GetMedia(CSecID id);

	void   SetRecording(DWORD dwFlags, BOOL bActive);
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

public:
	void DoRequestMediaDecoding(CIPCAudioDVClient*pBCC, const CIPCMediaSampleRecord& media, CSecID mediaID, DWORD dwFlags);
	void DoRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags);
	void DeleteAllMedia();
	void CheckForStopAudioEncoding();


private:
	WORD	m_wGroupID;
	int		m_iHardware;	// Hardwareflag, 0: ok, !=0 Fehler im Modul
	DWORD	m_dwState;		// Der aktuelle Status der max 32 Gruppenmitglieder.
	CSecID  m_DecoderID;
	CSecID  m_EncoderID;
	DWORD	m_dwRecordingFlags;
	BOOL	m_bResetConfirmed;
	
	CIPCAudioDVClient *m_pBroadcastingClient;

public:
	static BOOL gm_bTraceConfirmValues;
	static BOOL gm_bTraceIndicateMediaData;
};
//////////////////////////////////////////////////////////////////////
inline WORD CIPCAudioServer::GetID() const
{
	return m_wGroupID;
}
//////////////////////////////////////////////////////////////////////
inline int CIPCAudioServer::GetHardwareState() const
{
	return m_iHardware;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCAudioServer::ResetConfirmed() const
{
	return m_bResetConfirmed;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CIPCAudioServer::GetRecordingFlags() const
{
	return m_dwRecordingFlags;
}

//////////////////////////////////////////////////////////////////////
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
	void StartCIPCThreads();
	void WaitForConnect();
	void WaitForReset();

private:
	CIPCAudioServer* m_pDecoder;
};
/////////////////////////////////////////////////////////////////////////////
inline CIPCAudioServer*	CAudioList::GetGroupAt(int i) const 
{ 
	if (i<GetSize()) 
	{
		return GetAtFast(i);
	} 
	else 
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_IPCAUDIOSERVER_H__25321CE1_F9F9_4097_B86D_48139BFC04D3__INCLUDED_)
