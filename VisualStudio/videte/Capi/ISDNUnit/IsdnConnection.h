/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: IsdnConnection.h $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/IsdnConnection.h $
// CHECKIN:		$Date: 28.02.06 15:14 $
// VERSION:		$Revision: 103 $
// LAST CHANGE:	$Modtime: 28.02.06 14:58 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "CipcfetchResult.h"

#ifndef IsdnConnection_H
#define IsdnConnection_H

#include "SecID.h"
#include "Capi4.h"
#include "hdr.h"
#include "CIPCPipeISDN.h"
#include "AbstractCapi.h"

// @doc

/////////////////////////////////////////////////////////////////////////////
class CISDNUnitApp;
class CIPCExtraMemory;
class CIPCServerControlISDN;
class CCapiQueue;
class CCapiQueueRecord;
class CConnectionRecord;
class CIPCCmdRecord;
class CKnockKnockBox;

class CIPCPipeInputPT;
class CIPCPipeOutputPT;

class CPTBox;

/////////////////////////////////////////////////////////////////////////////

// OOPS wird nicht ernsthaft benutzt
// sollte eine TimeOut fuer capi.Close() sein
// aber das geht wohl nicht
#define CAPI_CLOSE_TIME_OUT		5000

/////////////////////////////////////////////////////////////////////////////

// @enum state of a call
enum CallState
{
	CALL_NONE		= 0,	// @emem no call at all
	CALL_PASSIVE	= 1,	// @emem passive, called from the outside
	CALL_ACTIVE		= 2,	// @emem active, calling out
};

/////////////////////////////////////////////////////////////////////////////
// @enum state of a connection
enum ConnectionState
{
	CONNECT_ERROR	= 0,	// @emem error value
	CONNECT_CLOSED	= 1,	// @emem closed, not connected
	CONNECT_REQUEST	= 2,	// @emem NYD
	CONNECT_OPEN	= 3,	// @emem NYD
	CONNECT_CLOSING	= 4,	// @emem NYD
};

LPCTSTR NameOfEnum(ConnectionState cs);

/////////////////////////////////////////////////////////////////////////////
// @class the ISDNConnectionClass
// @base CAbstractCapiClient
class CISDNConnection : public CAbstractCapiClient
{
public:
	static BOOL ms_bSupportsMSN;
public:
	CISDNConnection(CISDNUnitApp* pApp, CIPCServerControlISDN* pControl, WORD wID);
	~CISDNConnection();
	
	void DeleteKnockKnockBox();

	void CrossThreadResetCapi();
	BOOL InitCapi(HWND hWnd);	// only for CCapiThread!
	// access
	inline	BOOL			IsValid() const;
	inline	ConnectionState	GetConnectState() const;
	inline	const CString &	GetRemoteNumber() const;
	inline	WORD			GetNumBChannels() const;
	inline	const CString &	GetRemoteHostName() const;
	inline	CallState		GetCallState() const;

	void	DoListen(const CString &sNumber);
	//
	void			CapiError(int iError);
	int		PollConnection(); // returns the number of OpenRequests
	BOOL	CrossThreadCloseConnection(BOOL bWaitForResponse);
	BOOL	CloseConnection(BOOL bWaitForResponse=FALSE);
	void	OpenSecondChannel();
	void	CloseOneChannel();
	BOOL	CheckCloseOneChannel();
	void	CallRemoteAddress(const CString& sRemoteAddress, WORD wBChannels = 0); // 0 means default channels
	BOOL	IsConnected(LPCTSTR psRemoteAddress, LPCTSTR pszRemotHostName=NULL);
	void	AddInputPipeClient(const CConnectionRecord &c, DWORD dwRequestCounter);
	void	AddOutputPipeClient(const CConnectionRecord &c, DWORD dwRequestCounter);
	void	AddDataBasePipeClient(const CConnectionRecord &c, DWORD dwRequestCounter);
	void	AddAlarmPipes(const CConnectionRecord &c, DWORD dwRequestCounter);
	void	AddAudioPipeClient(const CConnectionRecord &cIn, DWORD dwRequestCounter);
	BOOL	CipcDataRequest(CSecID idFrom, CSecID idTo,
							CIPCType Type, DWORD dwCmd,
							DWORD dwParam1, DWORD dwParam2,
							DWORD dwParam3, DWORD dwParam4,
							const CIPCExtraMemory* pExtraMem);
	void	RemovePipe(CIPCPipeISDN* pPipe);
	void	RemoveRemotePipe(CSecID localID, 
							 CSecID remoteID,
							 CIPCType ct );
	BOOL	AnyPipesExisting();
	//
	long	ReceiveCapiMessage(WPARAM wParam, LPARAM lParam);
	//
	static	CString NameOfCapiInfo(WORD wInfo);
public:
	// specials for thread communication
	BOOL m_bDoResetCapi;
	BOOL m_bDoCloseCapi;
protected:
	// CAPI-Funktionen
	void	ListenConfirm(WORD wInfo);
	void	WhatsHappened(WORD wInfo);
	void	ConnectOK(LPCTSTR pCalledParty);
	void	ConnectClosed();
	void	ChannelClosed( LPCTSTR pCalledParty );
	void	CloseFailed(LPCTSTR pCalledParty);
	void	NrBChannelsChanged( WORD wBChannel );
	void	InfoMsg(WORD wInfo);


	// Capi hat Daten empfangen -> Empfangsfunktionen 
	void	ReceiveCipcData(const CIPCHDR *pCipcHdr);
	// CIPC-Daten von Capi empfangen
	void	OnCipcPipeRequest(const CConnectionRecord &c);
	void	OnCipcPipeRemove(CSecID id);

	void	MoveCipcDataToBuffer(LPCIPCHDR lpHdr, BYTE* lpData, WORD wMy_Data_Len);

	BOOL	PushCipcCmd(CCapiQueueRecord* pRecord);
public:
	CKnockKnockBox *m_pKnockKnockBox;	// OOPS
	static CString FilterDigits( const CString& strRufnummer);
	BOOL	AcceptCallForCalledPartyNumber( const CAbstractCapiCallInfo &callInfo);
	//
	void	RemoveMe(CIPCPipeISDN *pPipe);
	void	RequestPipe(CIPC *pCipc, const CConnectionRecord &c, DWORD dwCmd);

	inline void Lock();
	inline void Unlock();
	//
	inline BOOL IsPTConnection() const;
	inline BOOL IsISTConnection() const;
	//
	inline CCapiQueue *GetSendQueue() const;
	void	OnCipcPipeConfirm(CIPCType ct, 
								CSecID idLocal, CSecID idAnswer,
								CSecID idLocal2, CSecID idAnswer2,
								CIPCFetchResult &remoteFetchResult
								);
	inline CPTBox *GetPTBox() const;
	//
	void PanicClose(LPCTSTR szMsg);
protected:
	void	CheckForPendingPipes();
	void	CheckPipeArrayForPendingPipes(CIPCPipesArrayCS& pipes);
private:
	void SendPipeRequest(const CConnectionRecord &cIn,
						 CIPCType requestType, 
						 CIPCPipeISDN *pNewPipe,				// is input for ALARM connection
						 CIPCPipeISDN *pNewOutputPipe=NULL	// for ALARM connections
						 );
	void SetRemoteHostName(const CString &sName);
	void SetRemoteNumber(const CString &sNumber);
	void DumpPipeState();
	//
	void ShowKnockKnockBox(const CAbstractCapiCallInfo &callInfo, BOOL bOtherUnit);
	void UpdateKnockKnockBox(const CAbstractCapiCallInfo &callInfo);
	//
	BOOL CheckConnection();
	void DeleteAllPipesAndResetQueues();
	void CancelActiveRequests();
	void HandlePipeRequest(const CIPCCmdRecord &cmd);
	//
	void UpdateMSNSupport(const CString &sToText);
	BOOL ScanHostList(const CString &sCallingNumber);

	void RemoveOldPipes();
//	void CheckTimeoutForConnectRequest();
	void CheckForExistingPipes();	
	void CheckPTConnection();
	
private:
	CCriticalSection	m_cs;
	CIPCServerControlISDN	*m_pControl;				// CIPC Control to Clients
	CCapi					*m_pCapi;

	CIPCPipesArrayCS		m_InputPipesClient;
	CIPCPipesArrayCS		m_InputPipesServer;
	CIPCPipesArrayCS		m_OutputPipesClient;
	CIPCPipesArrayCS		m_OutputPipesServer;
	CIPCPipesArrayCS		m_DataBasePipesClient;
	CIPCPipesArrayCS		m_DataBasePipesServer;
	CIPCPipesArrayCS		m_AudioPipesClient;
	CIPCPipesArrayCS		m_AudioPipesServer;

	CIPCPipesArrayCS		m_removedPipes;

	CallState				m_CallState;
	ConnectionState			m_ConnectState;

	WORD					m_wNrBChannels;
	WORD					m_wNrBChannelsWanted;
	CString					m_sOwnNumber;
	CString					m_sRemoteNumber ;	
	CString					m_sRemoteHostName;	// from CConnectionRecord
	BOOL					m_bNoNumbersFromCapi;
private:
	CCapiQueue*				m_pCipcSendQ;
	CCapiQueue*				m_pCipcReceiveQ;
	BOOL					m_bIsValid;

	CIPCType				m_typeConnectRequest;

	BOOL					m_bThereWerePipesBefore;
	BOOL					m_bActiveRemovingPipes;
	
	DWORD					m_dwReceivedBlockCounter;

	CIPCCmdRecords	m_pipeRequests;
//
	CIPCPipeISDN *m_pCachedPipe;	// last use pipe or NULL
	//
	WORD	m_wCapiInfo;
	//
	BOOL m_bIsPTConnection;
	CPTBox *m_pPTBox;
	BOOL		m_bHandlingCapiGetMessageError;
	BOOL		m_bCompressed;
public:
	// NEW 100397
	// CAbstractCapiClient functions
	virtual const CString &GetOwnNumber() const;
	virtual int GetNumChannels() const;
	inline BOOL IsActive() const;

	virtual void OnListenConfirm(WORD wListen);
	//
	virtual CapiIncomingAnswer OnIncomingCall(
						CapiHandle newCh, 
						const CAbstractCapiCallInfo &callInfo
							);
	virtual void OnIncomingOkay(CapiHandle newCh, const CString &sNumber);
	//
	virtual long OnOutgoingDone(CapiHandle ch, CapiOutgoingResult result);
	//
	virtual void OnNumBChannelsChanged(CapiHandle ch, WORD wChannels);
	virtual void OnConnectionClosed(CapiHandle ch);
	//
	virtual void OnReceiveData(CapiHandle ch, DEFHDR *pData);
	virtual void OnReceiveData(CapiHandle ch, BYTE *pData, DWORD dwDataLen);	

	// NEW 17.8.97
	virtual void OnCallRejected();
	virtual void OnUserBusy();
	virtual void OnUnableToConnect();	// 0x34A2
	virtual void OnInfo(WORD wInfo);
	virtual void OnCapiGetMessageError();
	// end of CAbstractCapiClient fns

	static void HexDump(const BYTE *pData, DWORD dwDataLen, LPCTSTR szPre);
	//
	void HandlePTAlarmCall();
};

/////////////////////////////////////////////////////////////////////////////
typedef CISDNConnection* CISDNConnectionPtr;

/////////////////////////////////////////////////////////////////////////////
inline BOOL CISDNConnection::IsValid() const
{
	return m_bIsValid;
}
/////////////////////////////////////////////////////////////////////////////
inline ConnectionState CISDNConnection::GetConnectState() const
{
	return m_ConnectState;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString &CISDNConnection::GetRemoteNumber() const
{
	return m_sRemoteNumber ;
}
inline const CString &CISDNConnection::GetRemoteHostName() const
{
	return m_sRemoteHostName;
}

/////////////////////////////////////////////////////////////////////////////
inline WORD CISDNConnection::GetNumBChannels() const
{
	return m_wNrBChannels;
}

inline void CISDNConnection::Lock()
{
	m_cs.Lock();
}
inline void CISDNConnection::Unlock()
{
	m_cs.Unlock();
}

inline BOOL CISDNConnection::IsActive() const
{
	if (m_ConnectState == CONNECT_OPEN
		|| m_ConnectState == CONNECT_REQUEST) {
		return TRUE;
	}

	if (m_pCapi->CapiActive()) {
		return TRUE;
	} else {
		return FALSE;
	}
}

inline BOOL CISDNConnection::IsPTConnection() const
{
	return m_bIsPTConnection;
}

inline BOOL CISDNConnection::IsISTConnection() const
{
	if (m_bIsPTConnection) {
		return FALSE;
	} else {
		return TRUE;
	}
}

inline CCapiQueue *CISDNConnection::GetSendQueue() const
{
	return m_pCipcSendQ;
}
inline CPTBox *CISDNConnection::GetPTBox() const
{
	return m_pPTBox;
}

inline 	CallState CISDNConnection::GetCallState() const
{
	return m_CallState;
}

#endif	// IsdnConnection_H
