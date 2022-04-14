// CapiSMSVodafone.cpp: implementation of the CCapiSMS_Vodafone class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "smsunit.h"
#include "CapiSMS_Vodafone.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCapiSMS_Vodafone::CCapiSMS_Vodafone(CWnd* pParent)
: CCapiSMS(pParent)
{
	m_byTransactionReference	= 0;
	m_sAuthenticationCode		= _T("");
	m_sNotificationRequested	= _T("");
	m_sNotificationAddress		= _T("");
	m_sNotificationType			= _T("");
	m_sNotificationPID			= _T("");

	m_byaEMIMessage.RemoveAll();

	m_bResponseStarted	= FALSE;
	m_sProviderResponse = _T("");
}

CCapiSMS_Vodafone::~CCapiSMS_Vodafone()
{

}
//////////////////////////////////////////////////////////////////////
BOOL CCapiSMS_Vodafone::SendSMS(const CString& sReceiverID, const CString& sMessage)
{
	BOOL bRet = FALSE;
	m_sProviderResponse = _T("");
	m_sProviderError = _T("");

	// construct SMS message
	InitEMIMessage(sReceiverID, sMessage);

	// call SMSC
	bRet = m_pCapi->Open(m_sProviderID, (WORD)GetNumChannels());
	if (bRet)
	{
		m_ConnectState = CONNECT_REQUEST;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS_Vodafone::OnConnectReady()
{
	TRACE(_T("OnConnectReady\n"));
	m_ConnectState = CONNECT_OPEN;

	if (WK_IS_WINDOW(m_pParent))
	{
		m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_CONNECTED);
	}

	if (m_pCapi->SendRawData(m_byaEMIMessage.GetData(), (WORD)m_byaEMIMessage.GetSize()))
	{
		TRACE(_T("OnConnectReady Data send\n"));
		if (WK_IS_WINDOW(m_pParent))
		{
			m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_SEND_OK);
		}
	}
	else
	{
		TRACE(_T("OnConnectReady Data NOT send\n"));
		if (WK_IS_WINDOW(m_pParent))
		{
			m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_SEND_FAILED);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS_Vodafone::OnReceiveSMSData(BYTE *pData, DWORD dwDataLen)
{
	TRACE(_T("ReceiveData Len %u\n"), dwDataLen);
	BYTE byte;
	for (DWORD dw=0 ; dw<dwDataLen ; dw++)
	{
		byte = pData[dw];
		TRACE(_T("   %c %02x\n"), byte, byte);
		if (byte == 0x02)	// STX starts the message
		{
			if (m_sProviderResponse.GetLength())
			{
				// something went wrong
				WK_TRACE_ERROR(_T("CapiSMS Response new STX without ETX\n"));
				m_sProviderResponse = _T("");
			}
			m_bResponseStarted = TRUE;
		}
		else if (byte == 0x03)	// ETX stops the message
		{
			DecodeProviderResponse();
		}
		else if (m_bResponseStarted)
		{
			m_sProviderResponse += byte;
		}
		else
		{
			// something went wrong, data behind etx without stx, ignore
			WK_TRACE_ERROR(_T("CapiSMS Response data without STX\n"));
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CCapiSMS_Vodafone::InitEMIMessage(const CString& sReceiverID, const CString& sMessage)
{
	m_byaEMIMessage.RemoveAll();

	char szTemp[256];
	int i = 0;
	int iMaxLen = 0;

	// construct SMS message

	// leading stx
	m_byaEMIMessage.Add(0x02);	// stx

	// header
	// 2 num. char. Transaction reference number
	i = _snprintf(szTemp, 2, "%02u", m_byTransactionReference);
	m_byaEMIMessage.Add(szTemp[0]);
	m_byaEMIMessage.Add(szTemp[1]);
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	// 5 num. char. Total number of IA5 characters between stx and etx
	for (i=0 ; i<5 ; i++)
	{
		m_byaEMIMessage.Add('0');
	}
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	// Transaction indicator, 'O' operation, 'R' reaction
	m_byaEMIMessage.Add('O');
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	// Operation type 51 for Submit short message operation 
	m_byaEMIMessage.Add('5');
	m_byaEMIMessage.Add('1');
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator

	// AdC address code recipient, string of num char max 16, mandatory
//	CWK_String sR(sReceiverID);
//	LPCSTR szReceiverID = sR;
//	iMaxLen = strlen(szReceiverID);
	iMaxLen = sReceiverID.GetLength();
	if (iMaxLen > 16)
	{
		WK_TRACE_ERROR(_T("CapiSMS szReceiverID %s too long %i > 16\n"), LPCTSTR(sReceiverID), iMaxLen);
		iMaxLen = 16;
	}
	for (i=0 ; i<iMaxLen ; i++)
	{
//		m_byaEMIMessage.Add(szReceiverID[i]);
		m_byaEMIMessage.Add((BYTE)sReceiverID[i]);
	}
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator

	// OAdC originator address code, string of num char max 16, mandatory
//	CWK_String sOA(m_sOriginatorAddress);
//	LPCSTR szOriginatorAddress = sOA;
//	iMaxLen = strlen(szOriginatorAddress);
	iMaxLen = m_sOriginatorAddress.GetLength();
	if (iMaxLen > 16)
	{
		WK_TRACE_ERROR(_T("CapiSMS szOriginatorAddress %s too long %i > 16\n"), LPCTSTR(m_sOriginatorAddress), iMaxLen);
		iMaxLen = 16;
	}
	for (i=0 ; i<iMaxLen ; i++)
	{
		m_byaEMIMessage.Add((BYTE)m_sOriginatorAddress[i]);
//		m_byaEMIMessage.Add(szOriginatorAddress[i]);
	}
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator

	// AC Authentication code originator, string of num char min 4, max 16, optional
//	CWK_String sAC(m_sAuthenticationCode);
//	LPCSTR szAuthenticationCode = sAC;
//	iMaxLen = strlen(szAuthenticationCode);
	iMaxLen = m_sAuthenticationCode.GetLength();
	if ((iMaxLen > 0) && (iMaxLen < 4))
	{
		WK_TRACE_ERROR(_T("CapiSMS szAuthenticationCode %s too short %i < 4\n"), LPCTSTR(m_sAuthenticationCode), iMaxLen);
		iMaxLen = 0; // skip to avoid error
	}
	if (iMaxLen > 16)
	{
		WK_TRACE_ERROR(_T("CapiSMS szAuthenticationCode %s too long %i > 16\n"), LPCTSTR(m_sAuthenticationCode), iMaxLen);
		iMaxLen = 16;
	}
	for (i=0 ; i<iMaxLen ; i++)
	{
		m_byaEMIMessage.Add((BYTE)m_sAuthenticationCode[i]);
//		m_byaEMIMessage.Add(szAuthenticationCode[i]);
	}
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator

	// NRq Notification request, 1 num char, optional
//	CWK_String sNR(m_sNotificationRequested);
//	LPCSTR szNotificationRequested = sNR;
//	iMaxLen = strlen(szNotificationRequested);
	iMaxLen = m_sNotificationRequested.GetLength();
	if (iMaxLen > 1)
	{
		WK_TRACE_ERROR(_T("CapiSMS szNotificationRequested %s too long %i > 1\n"), LPCTSTR(m_sNotificationRequested), iMaxLen);
		iMaxLen = 1;
	}
	for (i=0 ; i<iMaxLen ; i++)
	{
		m_byaEMIMessage.Add((BYTE)m_sNotificationRequested[i]);
//		m_byaEMIMessage.Add(szNotificationRequested[i]);
	}
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// NAdC Notification address code, string of num char max 16, optional
//	CWK_String sNA(m_sNotificationAddress);
//	LPCSTR szNotificationAddress = sNA;
//	iMaxLen = strlen(szNotificationAddress);
	iMaxLen = m_sNotificationAddress.GetLength();
	if (iMaxLen > 16)
	{
		WK_TRACE_ERROR(_T("CapiSMS szNotificationAddress %s too long %i > 16\n"), LPCTSTR(m_sNotificationAddress), iMaxLen);
		iMaxLen = 16;
	}
	for (i=0 ; i<iMaxLen ; i++)
	{
		m_byaEMIMessage.Add((BYTE)m_sNotificationAddress[i]);
//		m_byaEMIMessage.Add(szNotificationAddress[i]);
	}
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// NT Notification type,1 num char, optional
//	CWK_String sNT(m_sNotificationType);
//	LPCSTR szNotificationType = sNT;
//	iMaxLen = strlen(szNotificationType);
	iMaxLen = m_sNotificationType.GetLength();
	if (iMaxLen > 1)
	{
		WK_TRACE_ERROR(_T("CapiSMS szNotificationType %s too long %i > 1\n"), LPCTSTR(m_sNotificationType), iMaxLen);
		iMaxLen = 1;
	}
	for (i=0 ; i<iMaxLen ; i++)
	{
		m_byaEMIMessage.Add((BYTE)m_sNotificationType[i]);
//		m_byaEMIMessage.Add(szNotificationType[i]);
	}
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// NPID Notification PID value, 4 num char (0100 mobil, 0539 PC Appl. over TCP/IP), optional
//	CWK_String sNPID(m_sNotificationPID);
//	LPCSTR szNotificationPID = sNPID;
//	iMaxLen = strlen(szNotificationPID);
	iMaxLen = m_sNotificationPID.GetLength();
	if ((iMaxLen > 0) && (iMaxLen < 4))
	{
		WK_TRACE_ERROR(_T("CapiSMS szNotificationType %s too short %i < 4\n"), LPCTSTR(m_sNotificationPID), iMaxLen);
		iMaxLen = 0; // skip to avoid error
	}
	if (iMaxLen > 4)
	{
		WK_TRACE_ERROR(_T("CapiSMS szNotificationType %s too long %i > 4\n"), LPCTSTR(m_sNotificationPID), iMaxLen);
		iMaxLen = 4;
	}
	for (i=0 ; i<iMaxLen ; i++)
	{
		m_byaEMIMessage.Add((BYTE)m_sNotificationPID[i]);
//		m_byaEMIMessage.Add(szNotificationPID[i]);
	}
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// n.a., x string of char, empty
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// n.a., x string of char, empty
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// n.a., x string of char, empty
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// DD Deferred Delivery Requested, 1 num char (1, or empty), optional
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// DDT Deferred Delivery time in DDMMYYhhmm, 10 num char, optional
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// VP Validity period in DDMMYYhhmm, 10 num char, optional (Maximum is set to 2 days at the moment
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// RPID Relay PID value, 4 num char, optional (refer to Annex E i) and [1])
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// SCTS Service Centre Time stamp in DDMMYYhhmmss, 12 num char, optional
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// DSt Delivery Status, 1 num char, optional
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// Rsn Reason code, 3 num char, optional
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// DSCTS Delivery Service Centre Time Stamp in DDMMYYhhmmss, 12 num char, optional
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// MT Message type, 1 num char (2 Num, 3 Alphanumeric, 4 Transparent), mandatory
	m_byaEMIMessage.Add('3');	// '/' as seperator
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// NB No. of Bits in Transparent data (TD) message, max 4 num char, mandatory
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// Msg Message itself, String of alphanumeric char max (NMsg+AMsg 640, TMsg 140), optional
	CByteArray byaMessage;
	EncodeMessageToIA5(sMessage, byaMessage);
	m_byaEMIMessage.Append(byaMessage);
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// MMS More messages to sent, 1 num char, restricted to Large Accounts only, so empty
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// n.a., 1 num char, empty
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// DCS Data Coding Scheme, 1 num char, only used in OT-52
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// MCI Message Class Type, 1 num char, only if MT=4 and no XSer "GSM DCS information is used Refer to [3]
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// n.a., 1 num char, empty
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// n.a., x String of char, empty
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// n.a., x String of char, empty
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// OTOA Originator Type Of, 4 string of char, restricted to Large Accounts only, so empty
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// n.a., x String of char, empty
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// XSer, max 400 String of char, restricted for Large Account in OT-52 only, so empty
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator

	// RES4, x String of char, empty
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// RES5, x String of char, empty
	m_byaEMIMessage.Add(0x2f);	// '/' as seperator
	
	// set the length of the emi message
	// length without stx but with 2 char CRC -> +1
	i = _snprintf(szTemp, 5, "%05u", m_byaEMIMessage.GetSize()+1);
	for (i=0 ; i<5 ; i++)
	{
		m_byaEMIMessage.SetAt(i+4, szTemp[i]);
	}

	// checksum, 2 alphanumeric char, addition of all bytes before without stx (and without following etx)
	BYTE byCRC = 0;
	for (i=1 ; i<m_byaEMIMessage.GetSize() ; i++)
	{
		byCRC = (BYTE)(byCRC + m_byaEMIMessage.GetAt(i));
	}
	i = _snprintf(szTemp, 2, "%02X", byCRC);
	m_byaEMIMessage.Add(szTemp[0]);
	m_byaEMIMessage.Add(szTemp[1]);
	
	m_byaEMIMessage.Add(0x03);	// etx

	CString sTrace;
	for (i=0 ; i<m_byaEMIMessage.GetSize() ; i++)
	{
		sTrace += GetAsciiSymbol(m_byaEMIMessage.GetAt(i));
	}
	WK_TRACE(_T("SMS: %s\n"), (LPCTSTR)sTrace);

// 

}
//////////////////////////////////////////////////////////////////////
BOOL CCapiSMS_Vodafone::EncodeMessageToIA5(const CString& sMessage, CByteArray& byaMessage)
{
	BOOL bRet = TRUE;
	byaMessage.RemoveAll();

//	CWK_String sMsg(sMessage);
//	LPCSTR szMessage = sMsg;
//	int iLen = strlen(szMessage);

	CString sMsg = sMessage;;
	sMsg.Replace(_T("ä"), _T("ae"));
	sMsg.Replace(_T("ö"), _T("oe"));
	sMsg.Replace(_T("ü"), _T("ue"));
	sMsg.Replace(_T("Ä"), _T("AE"));
	sMsg.Replace(_T("Ö"), _T("OE"));
	sMsg.Replace(_T("Ü"), _T("UE"));
	sMsg.Replace(_T("ß"), _T("ss"));
/*
	sMsg.Replace(_T("ä"), _T("a"));
	sMsg.Replace(_T("ö"), _T("o"));
	sMsg.Replace(_T("ü"), _T("u"));
	sMsg.Replace(_T("Ä"), _T("A"));
	sMsg.Replace(_T("Ö"), _T("O"));
	sMsg.Replace(_T("Ü"), _T("U"));
	sMsg.Replace(_T("ß"), _T("s"));
*/
	int iLen = sMsg.GetLength();

	char szTemp[3] ={0};
	char byChar;
	wchar_t wChar;

	for (int i=0 ; i<iLen ; i++)
	{
//		byChar = szMessage[i];
		wChar = sMsg.GetAt(i);
		if (HIBYTE(wChar))
		{
			TRACE(_T("Unicode%d: %04x\n"), i, wChar);
		}
		byChar = (char)wChar;
		_snprintf(szTemp, 2, "%02X", byChar);
		if (byChar <= 0x7F)
		{
			switch (byChar)
			{
				case 0x5F:	_snprintf(szTemp, 2, "%02X", 0x11);	break;
			}
		}
		else // (byChar > 0x7F)
		{
			switch (byChar)
			{
				case 0xA3:	_snprintf(szTemp, 2, "%02X", 0x01);	break;
				case 0xA5:	_snprintf(szTemp, 2, "%02X", 0x03);	break;
				case 0xE8:	_snprintf(szTemp, 2, "%02X", 0x04);	break;
				case 0xE9:	_snprintf(szTemp, 2, "%02X", 0x05);	break;
				case 0xF9:	_snprintf(szTemp, 2, "%02X", 0x06);	break;
				case 0xEC:	_snprintf(szTemp, 2, "%02X", 0x07);	break;
				case 0xF2:	_snprintf(szTemp, 2, "%02X", 0x08);	break;
				case 0xC7:	_snprintf(szTemp, 2, "%02X", 0x09);	break;
				case 0xD8:	_snprintf(szTemp, 2, "%02X", 0x0B);	break;
				case 0xF8:	_snprintf(szTemp, 2, "%02X", 0x0C);	break;
				case 0xC5:	_snprintf(szTemp, 2, "%02X", 0x0E);	break;
				case 0xE5:	_snprintf(szTemp, 2, "%02X", 0x0F);	break;
				case 0x81:	_snprintf(szTemp, 2, "%02X", 0x10);	break;
				case 0x82:	_snprintf(szTemp, 2, "%02X", 0x12);	break;
				case 0x83:	_snprintf(szTemp, 2, "%02X", 0x13);	break;
				case 0x84:	_snprintf(szTemp, 2, "%02X", 0x14);	break;
				case 0x85:	_snprintf(szTemp, 2, "%02X", 0x15);	break;
				case 0x86:	_snprintf(szTemp, 2, "%02X", 0x16);	break;
				case 0x87:	_snprintf(szTemp, 2, "%02X", 0x17);	break;
				case 0x88:	_snprintf(szTemp, 2, "%02X", 0x18);	break;
				case 0x89:	_snprintf(szTemp, 2, "%02X", 0x19);	break;
				case 0x8A:	_snprintf(szTemp, 2, "%02X", 0x1A);	break;
				case 0xC6:	_snprintf(szTemp, 2, "%02X", 0x1C);	break;
				case 0xE6:	_snprintf(szTemp, 2, "%02X", 0x1D);	break;
				case 0xC9:	_snprintf(szTemp, 2, "%02X", 0x1F);	break;
				case 0xA4:	_snprintf(szTemp, 2, "%02X", 0x02);	break;
				case 0xA1:	_snprintf(szTemp, 2, "%02X", 0x00);	break;
				case 0xD1:	_snprintf(szTemp, 2, "%02X", 0x5F);	break;
				case 0xA7:	_snprintf(szTemp, 2, "%02X", 0x5E);	break;
				case 0xBF:	_snprintf(szTemp, 2, "%02X", 0x60);	break;
				case 0xF1:	_snprintf(szTemp, 2, "%02X", 0x1E);	break;
				case 0xE0:	_snprintf(szTemp, 2, "%02X", 0x7F);	break;
				case 'Ä':	_snprintf(szTemp, 2, "%02X", 0x5B);	break;
				case 'Ö':	_snprintf(szTemp, 2, "%02X", 0x5C);	break;
				case 'Ü':	_snprintf(szTemp, 2, "%02X", 0x5D);	break;
				case 'ä':	_snprintf(szTemp, 2, "%02X", 0x7B);	break;
				case 'ö':	_snprintf(szTemp, 2, "%02X", 0x7C);	break;
				case 'ü':	_snprintf(szTemp, 2, "%02X", 0x7D);	break;
				case 'ß':	_snprintf(szTemp, 2, "%02X", 0x7E);	break;
				default:	_snprintf(szTemp, 2, "%02X", 0x20); break;	// replace with SPACE
			}
		}
		byaMessage.Add(szTemp[0]);
		byaMessage.Add(szTemp[1]);
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS_Vodafone::DecodeProviderResponse()
{
	// Provider Response without STX and ETX
	TRACE("DecodeProviderResponse\n");

	CStringArray sParts;
	SplitString(m_sProviderResponse, sParts, '/');
	// Parts are
	// 0 Transaction reference number
	// 1 Length
	// 2 Operation 'R'
	// 3 Operation type
	// 4 Acknowledge 'A' ok, 'N' error
	// 5 if ok:    Modified Validity Period or empty
	//	 if error: Error code
	// 6 System message (e.g. Time stampError description
	// 7 checksum

	if (sParts.GetSize() != 8)
	{
		WK_TRACE_ERROR(_T("ProviderResponse fields mismatch\n"));
	}

	// NOT YET checksum check

	BOOL bAck = FALSE;
	int iErrorCode = 0;
	CString sSystemMessage;
	if (sParts.GetSize() > 4)
	{
		bAck = (sParts.GetAt(4) == "A");
		if (   (bAck == FALSE)
			&& (sParts.GetSize() > 5)
			)
		{
			iErrorCode = _ttoi(sParts.GetAt(5));
			if (sParts.GetSize() > 6)
			{
				sSystemMessage = sParts.GetAt(6);
			}
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("ProviderResponse not enough fields for Acknowledge\n"));
		sSystemMessage = _T("ProviderResponse invalid");
	}

	if (bAck)
	{ // ok, accepted
		m_sProviderError = _T("");
		if (WK_IS_WINDOW(m_pParent))
		{
			m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_RESPONSE_OK);
		}
	}
	else
	{ // error
		DecodeProviderErrorCode(iErrorCode, sSystemMessage);
		if (WK_IS_WINDOW(m_pParent))
		{
			m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_RESPONSE_ERROR);
		}
	}

	m_sProviderResponse = _T("");
	m_bResponseStarted = FALSE;

	m_byTransactionReference++;
	// only 2 num. chars in protocol
	if (m_byTransactionReference >99)
	{
		m_byTransactionReference = 0;
	}

	CloseConnection();
}
//////////////////////////////////////////////////////////////////////
void CCapiSMS_Vodafone::DecodeProviderErrorCode(int iErrorCode, CString sErrorDescription)
{
	CString sError;
	switch (iErrorCode)
	{
		// Application internal code, not from SMSC
		case 0:		sError =_T("Internal Protocol error");							break;
		case 1:		sError =_T("Checksum error");									break;
		case 2:		sError =_T("Syntax error");										break;
		case 3:		sError =_T("Operation notsupported by system");					break;
		case 4:		sError =_T("Operation not allowed (at this point in time)");	break;
		case 5:		sError =_T("Call barring active");								break;
		case 6:		sError =_T("AdC invali");										break;
		case 7:		sError =_T("Authentication failure");							break;
		case 8:		sError =_T("Legitimisation code for all calls, failure");		break;
		case 9:		sError =_T("GA not valid");										break;
		case 10:	sError =_T("Repetition not allowed");							break;
		case 11:	sError =_T("Legitimisation code for repetition, failure");		break;
		case 12:	sError =_T("Priority call not allowed");						break;
		case 13:	sError =_T("Legitimisation code for priority call, failure");	break;
		case 14:	sError =_T("Urgent message not allowed");						break;
		case 15:	sError =_T("Legitimisation code for urgent message, failure");	break;
		case 16:	sError =_T("Reverse charging not allowed");						break;
		case 17:	sError =_T("Legitimisation code for reverse charging, failure");break;
		case 18:	sError =_T("Deferred delivery not allowed");					break;
		case 19:	sError =_T("New AC not valid");									break;
		case 20:	sError =_T("New legitimisation code not valid");				break;
		case 21:	sError =_T("Standard text not valid");							break;
		case 22:	sError =_T("Time period not valid");							break;
		case 23:	sError =_T("Message type not supported by system");				break;
		case 24:	sError =_T("Message too long");									break;
		case 25:	sError =_T("Requested standard text not valid");				break;
		case 26:	sError =_T("Message type not valid for the pager type");		break;
		case 27:	sError =_T("Message not found in SMSC");						break;
		case 28:	sError.Format(_T("Error %i code not defined"), iErrorCode);		break;
		case 29:	sError.Format(_T("Error %i code not defined"), iErrorCode);		break;
		case 30:	sError =_T("Subscriber hang-up");								break;
		case 31:	sError =_T("Fax group not supported");							break;
		case 32:	sError =_T("Fax message type not supported");					break;
		case 33:	sError =_T("Address already in list (60 series)");				break;
		case 34:	sError =_T("Address not in list (60 series)");					break;
		case 35:	sError =_T("List full (60 series)");							break;
		default:	sError.Format(_T("Error %i code not defined"), iErrorCode);		break;
	}
	if (sErrorDescription.Find(sError) != -1)
	{
		m_sProviderError = sErrorDescription;
	}
	else if (sError.Find(sErrorDescription) != -1)
	{
		m_sProviderError = sError;
	}
	else
	{
		m_sProviderError = sError + _T(" ; ") + sErrorDescription;
	}
}
