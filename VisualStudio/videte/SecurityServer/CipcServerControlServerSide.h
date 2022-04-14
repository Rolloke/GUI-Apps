/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcServerControlServerSide.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/CipcServerControlServerSide.h $
// CHECKIN:		$Date: 12.06.06 20:56 $
// VERSION:		$Revision: 43 $
// LAST CHANGE:	$Modtime: 12.06.06 20:44 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _CIPCServerControlServerSide_H_
#define _CIPCServerControlServerSide_H_

#include "CipcServerControl.h"
#include "CipcInputServerClient.h"
#include "CipcOutputServerClient.h"
#include "IpcAudioServerClient.h"

class CClientLink {
public:
	CClientLink(CIPCOutputServerClient *pClient, DWORD dwUserData);
	CClientLink(CIPCInputServerClient *pClient);
	// data:
	DWORD	m_dwUserData;	// from client side
	DWORD	m_dwID;			// used as userData --> Unit
	CIPCOutputServerClient *m_pOutputClient;
	CIPCInputServerClient *m_pInputClient;
private:
	static DWORD m_idCounter;
};
typedef CClientLink *CClientLinkPtr;

WK_PTR_ARRAY_CS(CClientLinkArrayPlain,CClientLinkPtr,CClientLinkArraySafe)

class CClientLinkArray : public CClientLinkArraySafe
{
public:
	CClientLinkArray();
	void DropAndSwap(int ix);
};

class CIPCServerControlServerSide : public CIPCServerControl
{
	// construction / destruction
public:
	CIPCServerControlServerSide();
	virtual ~CIPCServerControlServerSide();

	// attributes
public:
	inline CSecID GetOverlayClient() const;
	inline CSecID GetOverlayClient();
	inline CSecID GetDecompressClient();
	inline BOOL CanDoOverlay(CSecID idClient) const;
	BOOL IsTimeout();

    // operations
public:
	void Shutdown();
	void DisconnectAll();
	void DisconnectTimedoutClients(CSecID idTimer);
	BOOL CheckPermission(CConnectionRecord &connectIn,CIPCFetchResult &fetchResult);

	CIPCInputServerClient*  AddInputClient(const CConnectionRecord &c);
	CIPCOutputServerClient* AddOutputClient(const CConnectionRecord &c,
											BOOL bAlarm, 
											const CString& sRemoteHost);
	CIPCAudioServerClient* AddAudioClient(const CConnectionRecord &c);
	//
	void RemoveMe(CIPCInputServerClient *pClient);
	void RemoveMe(CIPCOutputServerClient *pClient);
	void RemoveMe(CIPCAudioServerClient *pClient);

	// overlay managment:
	void SetActiveClient(CIPCOutputServerClient *pClient, BOOL bActive);
	void SetOverlayClient(CSecID idClient, WORD wDecodeId);
	void SetDecompressClient(CIPCOutputServerClient *pClient);

	void IndicateAlarmToClients(WORD wGroupID,
								DWORD inputMask, 	// 1 high, 0 low
								DWORD changeMask);	// 1 changed, 0 unchanged
	void IndicateAlarmToClients(CSecID id, BOOL bAlarm, DWORD dwData1, DWORD dwData2);

	// overrides
public:
	//
	virtual BOOL Run(DWORD dwTimeout);
	//
	virtual void OnRequestServerReset(const CConnectionRecord &c);
	virtual void OnRequestInputConnection(const CConnectionRecord &c);
	virtual void OnRequestOutputConnection(const CConnectionRecord &c);
	virtual void OnRequestAudioConnection(const CConnectionRecord &c);

	//
	// clients:
	CIPCInputClientsArray m_inputClients;
	CIPCOutputClientsArray m_outputClients;
	CIPCAudioClientsArray m_audioClients;

	// data:
	CClientLinkArray m_usageH263Decoding;
	CClientLinkArray m_usageJpegDecoding;
	//
	CClientLinkArray m_usageCommData;



private:
	CIPCInputClientsArray m_inputClientsDropped;
	CIPCOutputClientsArray m_outputClientsDropped;
	CIPCAudioClientsArray m_audioClientsDropped;
	//
	CSecID m_idActiveClient;
	CSecID m_idOverlayClient;
	CSecID m_idDecompressClient;
	//
	CCriticalSection m_csDisplayClient;
	//
	WORD m_wLastOverlayDecoderID;	// from SetOverlayClient
	// counter for connection IDs
	static WORD	m_wNumInputConnections;
	static WORD m_wNumOutputConnections;
	static WORD m_wNumAudioConnections;

private:
	int		m_iCounter;
	DWORD	m_dwRuntick;
	CCriticalSection m_csRuntick;
};
///////////////////////////////////////////////////////////////////////////////////////////////
inline CSecID CIPCServerControlServerSide::GetOverlayClient() 
{
	return m_idOverlayClient;	
}
///////////////////////////////////////////////////////////////////////////////////////////////
inline CSecID CIPCServerControlServerSide::GetDecompressClient()
{
	return m_idDecompressClient;
}
///////////////////////////////////////////////////////////////////////////////////////////////
inline BOOL CIPCServerControlServerSide::CanDoOverlay(CSecID id) const
{
	return (   m_idActiveClient==SECID_NO_ID 
			|| m_idActiveClient == id
			|| (   m_idOverlayClient!=SECID_NO_ID
				&& m_idOverlayClient == id)
			);
}
inline CSecID CIPCServerControlServerSide::GetOverlayClient() const
{
	return m_idOverlayClient;
}

#endif
