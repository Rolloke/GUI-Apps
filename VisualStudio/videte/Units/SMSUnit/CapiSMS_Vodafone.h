// CapiSMSVodafone.h: interface for the CCapiSMS_Vodafone class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPISMSVODAFONE_H__7CC98C1A_A4A1_45D9_924E_DF2C434CE7DC__INCLUDED_)
#define AFX_CAPISMSVODAFONE_H__7CC98C1A_A4A1_45D9_924E_DF2C434CE7DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CapiSMS.h"

class CCapiSMS_Vodafone : public CCapiSMS  
{
public:
	CCapiSMS_Vodafone(CWnd* pParent);
	virtual ~CCapiSMS_Vodafone();

	// virtual functions of CCapiSMS
public:
	virtual BOOL	SendSMS(const CString& sReceiverID, const CString& sMessage);
protected:
	virtual void	OnConnectReady();
	virtual void	OnReceiveSMSData(BYTE *pData, DWORD dwDataLen);	

	void	InitEMIMessage(const CString& sReceiverID, const CString& sMessage);
	BOOL	EncodeMessageToIA5(const CString& sMessage, CByteArray& byaMessage);
	void	DecodeProviderResponse();
	void	DecodeProviderErrorCode(int iErrorCode, CString sErrorDescription);

	BYTE		m_byTransactionReference;
	CString		m_sAuthenticationCode;
	CString		m_sNotificationRequested;
	CString		m_sNotificationAddress;
	CString		m_sNotificationType;
	CString		m_sNotificationPID;

	CByteArray	m_byaEMIMessage;

	BOOL		m_bResponseStarted;
	CString		m_sProviderResponse;
};

#endif // !defined(AFX_CAPISMSVODAFONE_H__7CC98C1A_A4A1_45D9_924E_DF2C434CE7DC__INCLUDED_)
