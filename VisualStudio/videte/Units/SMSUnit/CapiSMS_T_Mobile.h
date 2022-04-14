// CapiSMS_T_Mobile.h: interface for the CCapiSMS_T_Mobile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPISMS_T_MOBILE_H__FFAD8385_4A14_4D43_931A_3EA105AE82EF__INCLUDED_)
#define AFX_CAPISMS_T_MOBILE_H__FFAD8385_4A14_4D43_931A_3EA105AE82EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CapiSMS.h"

class CCapiSMS_T_Mobile : public CCapiSMS  
{
	enum EnumSMSStatus
	{
		NORMAL_WAITING,
		INITIAL_SYNC,
		INITIAL_SYNC_ACK,
		IDENTIFICATION,
		IDENTIFICATION_ACK,
		GO_ON_INDICATION,
		TRANSACTION,
		TRANSACTION_ACK,
		TRANSACTION_NAK,
		TRANSACTION_ERROR	,
		END_OF_TRANSMISSION,
		END_OF_TRANSMISSION_ACK
//		UNSOLICITED_EOT is same as END_OF_TRANSMISSION_ACK
	};

public:
	CCapiSMS_T_Mobile(CWnd* pParent);
	virtual ~CCapiSMS_T_Mobile();

	// virtual functions of CCapiSMS
public:
	virtual BOOL	SendSMS(const CString& sReceiverID, const CString& sMessage);
protected:
	virtual void	OnConnectReady();
	virtual void	OnReceiveSMSData(BYTE *pData, DWORD dwDataLen);	
	virtual long	OnOutgoingDone(CapiHandle ch, CapiOutgoingResult result);

private:
	CString	NameOfStatus();
	void	InitTAPMessage(const CString& sReceiverID, const CString& sMessage);
	BOOL	EncodeMessage(const CString& sMessage, CByteArray& byaMessage, int iMaxBytes);

	BOOL	CheckForInitialSyncAck();
	BOOL	CheckForIdentificationAck();
	BOOL	CheckForGoOnIndication();
	BOOL	CheckForTransactionAck();
	BOOL	CheckForEndOfTransmission();
	BOOL	CheckForEndOfTransmissionAck();
	
	void	SendInitialSync();
	void	SendIdentification();
	void	SendTransaction();
	void	SendEndOfTransmission();

	void	DecodeProviderMsg(CString sMsg);

	EnumSMSStatus	m_eSMSStatus;
	CByteArray		m_byaTAPMessage;

	CByteArray	m_byaProviderResponse;
	int			m_iRepeatCounter;
};

#endif // !defined(AFX_CAPISMS_T_MOBILE_H__FFAD8385_4A14_4D43_931A_3EA105AE82EF__INCLUDED_)
