// IPCMediaAudioUnit.h: interface for the CIPCMediaAudioUnit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCMEDIAAUDIOUNIT_H__72262C36_3D40_4CD7_8537_A30AF43FD7D4__INCLUDED_)
#define AFX_IPCMEDIAAUDIOUNIT_H__72262C36_3D40_4CD7_8537_A30AF43FD7D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAudioUnitDlg;

class CIPCMediaAudioUnit : public CIPCMedia  
{
	// @access construction / destruction
public:
	CIPCMediaAudioUnit(LPCTSTR, BOOL, CAudioUnitDlg*);
	virtual ~CIPCMediaAudioUnit();
	// Client-Server Information
public:
	// Parameters
	CSecID GetSecID(WORD) const;
	CSecID GetReplyID()    { return m_ReplyID;}
	CSecID GetEncodingID() { return m_EncodingID;}

	BOOL   IsConnected();
	// Media Reset:
	virtual void OnRequestReset(WORD wGroupID);
	// Media Hardware:
	virtual void OnRequestHardware(CSecID secID, DWORD dwIDRequest);
	// Media Encoding
	virtual void OnRequestStartMediaEncoding(CSecID mediaID, DWORD dwFlags, DWORD dwUserData);
	virtual void OnRequestStopMediaEncoding(CSecID mediaID, DWORD dwFlags);
	virtual void OnIndicateMediaData(const CIPCMediaSampleRecord& media, CSecID mediaID, DWORD dwUserData);
	void DoIndicateMediaData(const CIPCMediaSampleRecord& media, CSecID mediaID);
	// Media Decoding
	virtual void OnRequestMediaDecoding(const CIPCMediaSampleRecord&, CSecID mediaID, DWORD dwFlags);
	virtual void OnRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags);
	// Media Values
 	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestSetValue(CSecID , const CString &, const CString &, DWORD);
	virtual void OnRequestGetValue(CSecID id, const CString &sKey,DWORD dwServerData);
	virtual void OnRequestValues(CSecID, DWORD, DWORD, DWORD, const CIPCExtraMemory*);
	virtual void OnConfirmValues(CSecID, DWORD, DWORD, DWORD dwValue  , const CIPCExtraMemory*pData);
	// Connection
	virtual void OnReadChannelFound();
	virtual void OnRequestDisconnect();
	
	bool DoReset() { return m_bReset;}
	void ConfirmReset();
	
private:
	// no implementation, to avoid implicit generation!
	CIPCMediaAudioUnit(const CIPCMediaAudioUnit& src);		
	// no implementation, to avoid implicit generation!
	void operator=(const CIPCMediaAudioUnit& src);	

	CAudioUnitDlg *m_pDlg;
	WORD           m_wGroupID;
	DWORD          m_dwUserData;
	CSecID         m_ReplyID;
	CSecID         m_EncodingID;
	bool           m_bReset;
};

#endif // !defined(AFX_IPCMEDIAAUDIOUNIT_H__72262C36_3D40_4CD7_8537_A30AF43FD7D4__INCLUDED_)
