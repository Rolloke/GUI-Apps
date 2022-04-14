// CapiSMS.h: interface for the CCapiSMS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPISMS_H__D7663682_6CE2_4CED_B889_6537B829EB22__INCLUDED_)
#define AFX_CAPISMS_H__D7663682_6CE2_4CED_B889_6537B829EB22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Capi4.h"
#include "AbstractCapi.h"

#define NET_CAPI_MSG	WK_NET_BASE +0x0002

#define CAPI_SMS_MSG		WM_USER + 0x0002
// sub type of CAPI_SMS_MSG
#define CAPI_ERROR					0
#define CAPI_SMS_CONNECTED			1
#define CAPI_SMS_DISCONNECTED		2
#define CAPI_SMS_REJECTED			3
#define CAPI_SMS_USER_BUSY			4
#define CAPI_SMS_NOT_AVAILABLE	5
#define CAPI_SMS_SEND_OK			6
#define CAPI_SMS_SEND_FAILED		7
#define CAPI_SMS_RESPONSE_OK		8
#define CAPI_SMS_RESPONSE_ERROR	9

enum CapiSmsType
{
	CST_VODAFONE,
	CST_T_D1
};

class CCapiSMS  : public CAbstractCapiClient
{
/////////////////////////////////////////////////////////////////////////////
protected:
	enum ConnectionState
	{
		CONNECT_ERROR	= 0,	// @emem error value
		CONNECT_CLOSED	= 1,	// @emem closed, not connected
		CONNECT_REQUEST	= 2,	// @emem NYD
		CONNECT_OPEN	= 3,	// @emem NYD
		CONNECT_CLOSING	= 4,	// @emem NYD
	};

public:
	CCapiSMS(CWnd* pParent);
	virtual ~CCapiSMS();

public:
	inline CString	GetProviderID() const;
	inline CString	GetProviderError() const;

	// virtual functions of Capi4Lib
public:
	virtual const CString& GetOwnNumber() const;
protected:
	virtual int GetNumChannels() const;

	virtual void OnListenConfirm(WORD wListen);		// not yet used
	virtual CapiIncomingAnswer OnIncomingCall(CapiHandle newCh, const CAbstractCapiCallInfo &callInfo);

	virtual void OnIncomingOkay(CapiHandle newCh, const CString &sNumber);	// not yet used
	virtual long OnOutgoingDone(CapiHandle ch, CapiOutgoingResult result);
	virtual void OnNumBChannelsChanged(CapiHandle ch, WORD wChannels);		// not yet used
	virtual void OnConnectionClosed(CapiHandle ch);							// not yet used

	virtual void OnReceiveData(CapiHandle ch, DEFHDR *pData);
	virtual void OnReceiveData(CapiHandle ch, BYTE *pData, DWORD dwDataLen);	

	virtual void OnCallRejected();
	virtual void OnUserBusy();
	virtual void OnUnableToConnect();	// 0x34A2
	virtual void OnInfo(WORD wInfo);
	virtual void OnCapiGetMessageError();

	// virtual functions of CCapiSMS
public:
	virtual BOOL	SendSMS(const CString& sReceiverID, const CString& sMessage) PURE_VIRTUAL(0)
protected:
	virtual void	OnConnectReady() PURE_VIRTUAL(;)
	virtual void	OnReceiveSMSData(BYTE *pData, DWORD dwDataLen) PURE_VIRTUAL(;)	

public:
	BOOL	InitCapi(HWND hHiddenWnd);
	void	PollConnection();
	void	SetOwnNumber(CString sOwnNumber);
	void	SetProviderID(CString sProviderID);
	void	CloseConnection();
	LRESULT ReceiveCapiMessage(WPARAM wParam, LPARAM lParam);
private:
	void	CloseFailed(const char* szCalledParty);

protected:
	CWnd*			m_pParent;
	CCapi*			m_pCapi;
	CString			m_sProviderID;
	CString			m_sOriginatorAddress;
	ConnectionState	m_ConnectState;
	CString			m_sProviderError;
};
//////////////////////////////////////////////////////////////////////
inline CString CCapiSMS::GetProviderID() const
{
	return m_sProviderID;
}
//////////////////////////////////////////////////////////////////////
inline CString CCapiSMS::GetProviderError() const
{
	return m_sProviderError;
}

#endif // !defined(AFX_CAPISMS_H__D7663682_6CE2_4CED_B889_6537B829EB22__INCLUDED_)
