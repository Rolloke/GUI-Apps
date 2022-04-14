// CapiSMS_T_Mobile.cpp: implementation of the CCapiSMS_T_Mobile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "smsunit.h"
#include "CapiSMS_T_Mobile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCapiSMS_T_Mobile::CCapiSMS_T_Mobile(CWnd* pParent)
: CCapiSMS(pParent)
{
	m_eSMSStatus = NORMAL_WAITING;
	m_iRepeatCounter = 0;
}
//////////////////////////////////////////////////////////////////////
CCapiSMS_T_Mobile::~CCapiSMS_T_Mobile()
{
}
//////////////////////////////////////////////////////////////////////
BOOL CCapiSMS_T_Mobile::SendSMS(const CString& sReceiverID, const CString& sMessage)
{
	BOOL bRet = FALSE;
	// clear all buffer data for new job
	m_byaProviderResponse.RemoveAll();
	m_sProviderError = _T("");
	m_iRepeatCounter = 0;

	// construct SMS message
// For Tests only!
#ifdef _DEBUG
 #if 1
	m_sProviderID			= theApp.m_sPrefix + theApp.m_sD1;
//	CString sReceiverIDTest	= _T("01754807703");
//	InitTAPMessage(sReceiverIDTest, sMessage);
	InitTAPMessage(sReceiverID, sMessage);
 #endif
#else
	InitTAPMessage(sReceiverID, sMessage);
#endif

	// call SMSC
	bRet = m_pCapi->Open(m_sProviderID, (WORD)GetNumChannels());
	if (bRet)
	{
		m_ConnectState = CONNECT_REQUEST;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS_T_Mobile::OnConnectReady()
{
	TRACE(_T("OnConnectReady\n"));
	m_ConnectState = CONNECT_OPEN;

	if (WK_IS_WINDOW(m_pParent))
	{
		m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_CONNECTED);
	}

	if (m_eSMSStatus != NORMAL_WAITING)
	{
		WK_TRACE_WARNING(_T("CCapiSMS_T_Mobile::OnConnectReady Wrong Status %i\n"), m_eSMSStatus);
		m_eSMSStatus = NORMAL_WAITING;
	}

	// Send sync request
	SendInitialSync();
}
/////////////////////////////////////////////////////////////////////////////
long CCapiSMS_T_Mobile::OnOutgoingDone(CapiHandle ch, CapiOutgoingResult CapiResult)
{
	m_eSMSStatus = NORMAL_WAITING;
	return CCapiSMS::OnOutgoingDone(ch, CapiResult);
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS_T_Mobile::OnReceiveSMSData(BYTE *pData, DWORD dwDataLen)
{
	TRACE(_T("ReceiveData Len %u\n"), dwDataLen);
	BYTE byte;
	for (DWORD dw=0 ; dw<dwDataLen ; dw++)
	{
		byte = pData[dw];
		TRACE(_T("   %c %02x\n"), byte, byte);
		m_byaProviderResponse.Add(byte);
	}

	BOOL bFound = FALSE;
	// try to optimize checking regarding status
	switch (m_eSMSStatus)
	{
	case INITIAL_SYNC:
		bFound = CheckForInitialSyncAck();
		break;
	case IDENTIFICATION:
		bFound = CheckForIdentificationAck();
		break;
	case IDENTIFICATION_ACK:
		bFound = CheckForGoOnIndication();
		break;
	case TRANSACTION:
		bFound = CheckForTransactionAck();
		break;
	case END_OF_TRANSMISSION:
		bFound = CheckForEndOfTransmissionAck();
		break;
	}

	// If unnormal data flow
	if (bFound == FALSE)
	{
		WK_TRACE_WARNING(_T("OnReceiveSMSData Error in data flow %s\n"), LPCTSTR(NameOfStatus()));
		if		(CheckForInitialSyncAck());
		else if	(CheckForIdentificationAck());
		else if (CheckForGoOnIndication());
		else if (CheckForGoOnIndication());
		else if (CheckForTransactionAck());
		else if (CheckForEndOfTransmissionAck());
		else if (CheckForEndOfTransmission());
		else	;	// Not enough data? Wait for more.
	}
}
//////////////////////////////////////////////////////////////////////
CString CCapiSMS_T_Mobile::NameOfStatus()
{
	switch (m_eSMSStatus)
	{
	case NORMAL_WAITING:			return _T("NORMAL_WAITING");			break;
	case INITIAL_SYNC:				return _T("INITIAL_SYNC");				break;
	case INITIAL_SYNC_ACK:			return _T("INITIAL_SYNC_ACK");			break;
	case IDENTIFICATION:			return _T("IDENTIFICATION");			break;
	case IDENTIFICATION_ACK:		return _T("IDENTIFICATION_ACK");		break;
	case GO_ON_INDICATION:			return _T("GO_ON_INDICATION");			break;
	case TRANSACTION:				return _T("TRANSACTION");				break;
	case TRANSACTION_ACK:			return _T("TRANSACTION_ACK");			break;
	case TRANSACTION_NAK:			return _T("TRANSACTION_NAK");			break;
	case TRANSACTION_ERROR:			return _T("TRANSACTION_ERROR");			break;
	case END_OF_TRANSMISSION:		return _T("END_OF_TRANSMISSION");		break;
	case END_OF_TRANSMISSION_ACK:	return _T("END_OF_TRANSMISSION_ACK");	break;
	default:						return _T("UNKNOWN STATUS");			break;
	}
}
//////////////////////////////////////////////////////////////////////
void CCapiSMS_T_Mobile::InitTAPMessage(const CString& sReceiverID, const CString& sMessage)
{
	m_byaTAPMessage.RemoveAll();

	int i = 0;
	int iMaxLen = 0;

	// construct SMS message
	// max length allowed 256 bytes
	// leading stx
	m_byaTAPMessage.Add(ASCII_STX);	// stx

	// AdC address code recipient, string of num char max 16, mandatory
	CWK_String	sRID(sReceiverID);
	LPCSTR szReceiverID = sRID;
	iMaxLen = strlen(szReceiverID);
	if (iMaxLen > 16)
	{
		WK_TRACE_ERROR(_T("CapiSMS szReceiverID %s too long %i > 16\n"), LPCTSTR(sReceiverID), iMaxLen);
		iMaxLen = 16;
	}
	for (i=0 ; i<iMaxLen ; i++)
	{
		m_byaTAPMessage.Add(szReceiverID[i]);
	}
	m_byaTAPMessage.Add(ASCII_CR);	// CR

	// Msg Message itself, String of alphanumeric char
	CByteArray byaMsg;
	EncodeMessage(sMessage, byaMsg, 256-m_byaTAPMessage.GetSize()-6);
	m_byaTAPMessage.Append(byaMsg);

	m_byaTAPMessage.Add(ASCII_CR);	// CR
	m_byaTAPMessage.Add(ASCII_ETX);	// Block terminator, here always ETX, means only one transaction
	
	// 3 printable chars Checksum
	//Sum over all bytes inclusive STX and ETX
	WORD wCRC = 0;
	for (i=0 ; i<m_byaTAPMessage.GetSize() ; i++)
	{
		wCRC = (WORD)(wCRC + m_byaTAPMessage.GetAt(i));
	}
	// part CRC in 4 bit groups
	// take the 3 lower groups and add 0x30, which will result in the printable CRC chars
	BYTE byte = (BYTE)(wCRC >> 8);
	byte = (BYTE)((byte & 0x0f) + 0x30);
	m_byaTAPMessage.Add(byte);
	byte = (BYTE)(wCRC >> 4);
	byte = (BYTE)((byte & 0x0f) + 0x30);
	m_byaTAPMessage.Add(byte);
	byte = (BYTE)wCRC;
	byte = (BYTE)((byte & 0x0f) + 0x30);
	m_byaTAPMessage.Add(byte);

	m_byaTAPMessage.Add(ASCII_CR);	// CR

	CString sTrace;
	for (i=0 ; i<m_byaTAPMessage.GetSize() ; i++)
	{
		sTrace += GetAsciiSymbol(m_byaTAPMessage.GetAt(i));
	}
	TRACE(_T("SMS: %s\n"), (LPCTSTR)sTrace);
}
//////////////////////////////////////////////////////////////////////
BOOL CCapiSMS_T_Mobile::EncodeMessage(const CString& sMessage, CByteArray& byaMessage, int iMaxBytes)
{
	BOOL bRet = TRUE;
	byaMessage.RemoveAll();

	CWK_String sMsg(sMessage);
	LPCSTR szMessage = sMsg;
	int iMsgLen = strlen(szMessage);
	if (iMsgLen > iMaxBytes)
	{
		WK_TRACE_WARNING(_T("D1 message too long, truncated\n"));
		iMsgLen = iMaxBytes;
	}

	for (int i=0 ; i<iMsgLen ; i++)
	{
		byaMessage.Add((BYTE)szMessage[i]);
/*
		sTemp.Format("%02X", byChar);
		if (byChar <= 0x7F)
		{
			switch (byChar)
			{
				case 0x5F:	sTemp.Format("%02X", 0x11);	break;
			}
		}
		else // (byChar > 0x7F)
		{
			switch (byChar)
			{
				case 0xA3:	sTemp.Format("%02X", 0x01);	break;
				case 0xA5:	sTemp.Format("%02X", 0x03);	break;
				case 0xE8:	sTemp.Format("%02X", 0x04);	break;
				case 0xE9:	sTemp.Format("%02X", 0x05);	break;
				case 0xF9:	sTemp.Format("%02X", 0x06);	break;
				case 0xEC:	sTemp.Format("%02X", 0x07);	break;
				case 0xF2:	sTemp.Format("%02X", 0x08);	break;
				case 0xC7:	sTemp.Format("%02X", 0x09);	break;
				case 0xD8:	sTemp.Format("%02X", 0x0B);	break;
				case 0xF8:	sTemp.Format("%02X", 0x0C);	break;
				case 0xC5:	sTemp.Format("%02X", 0x0E);	break;
				case 0xE5:	sTemp.Format("%02X", 0x0F);	break;
				case 0x81:	sTemp.Format("%02X", 0x10);	break;
				case 0x82:	sTemp.Format("%02X", 0x12);	break;
				case 0x83:	sTemp.Format("%02X", 0x13);	break;
				case 0x84:	sTemp.Format("%02X", 0x14);	break;
				case 0x85:	sTemp.Format("%02X", 0x15);	break;
				case 0x86:	sTemp.Format("%02X", 0x16);	break;
				case 0x87:	sTemp.Format("%02X", 0x17);	break;
				case 0x88:	sTemp.Format("%02X", 0x18);	break;
				case 0x89:	sTemp.Format("%02X", 0x19);	break;
				case 0x8A:	sTemp.Format("%02X", 0x1A);	break;
				case 0xC6:	sTemp.Format("%02X", 0x1C);	break;
				case 0xE6:	sTemp.Format("%02X", 0x1D);	break;
				case 0xC9:	sTemp.Format("%02X", 0x1F);	break;
				case 0xA4:	sTemp.Format("%02X", 0x02);	break;
				case 0xA1:	sTemp.Format("%02X", 0x00);	break;
				case 0xD1:	sTemp.Format("%02X", 0x5F);	break;
				case 0xA7:	sTemp.Format("%02X", 0x5E);	break;
				case 0xBF:	sTemp.Format("%02X", 0x60);	break;
				case 0xF1:	sTemp.Format("%02X", 0x1E);	break;
				case 0xE0:	sTemp.Format("%02X", 0x7F);	break;
				case 'Ä':	sTemp.Format("%02X", 0x5B);	break;
				case 'Ö':	sTemp.Format("%02X", 0x5C);	break;
				case 'Ü':	sTemp.Format("%02X", 0x5D);	break;
				case 'ä':	sTemp.Format("%02X", 0x7B);	break;
				case 'ö':	sTemp.Format("%02X", 0x7C);	break;
				case 'ü':	sTemp.Format("%02X", 0x7D);	break;
				case 'ß':	sTemp.Format("%02X", 0x7E);	break;
				default:	sTemp.Format("%02X", 0x20); break;	// replace with SPACE
			}
		}
*/
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCapiSMS_T_Mobile::CheckForInitialSyncAck()
{
	BOOL bReturn = FALSE;
	// Response expected
	// 49 44 3d
	// ID=
	for (int i=0 ; i<m_byaProviderResponse.GetSize() ; i++)
	{
		if (m_byaProviderResponse.GetSize() >= (i+3))
		{
			if (m_byaProviderResponse.GetAt(i) == 0x49) // I
			{
				if (m_byaProviderResponse.GetAt(i+1) == 0x44) // D
				{
					if (m_byaProviderResponse.GetAt(i+2) == 0x3d) // =
					{
						if (m_eSMSStatus != INITIAL_SYNC)
						{
							WK_TRACE_WARNING(_T("CheckForInitialSyncAck Wrong Status %s\n"),
																		LPCTSTR(NameOfStatus()));
						}
						if (m_byaProviderResponse.GetSize() > (i+3))
						{
							WK_TRACE_WARNING(_T("CheckForInitialSyncAck Data too long %i\n"),
														m_byaProviderResponse.GetSize());
						}
						m_byaProviderResponse.RemoveAll();
						m_eSMSStatus = INITIAL_SYNC_ACK;
						SendIdentification();
						bReturn = TRUE;
						break;
					}
				}
			}
		}
		else
		{
			// not enough data, wait for more
			break;
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCapiSMS_T_Mobile::CheckForIdentificationAck()
{
	BOOL bReturn = FALSE;
	// Response expected
	// <version info> 0d 06 0d
	// <version info><CR><ACK><CR>
	// version info may vary or empty, actual is:
	// 32 2e 39 2e 30 2e 32
	// 2.9.0.2
	// we will ignore version info
	for (int i=0 ; i<m_byaProviderResponse.GetSize() ; i++)
	{
		if (m_byaProviderResponse.GetSize() >= (i+3))
		{
			if (m_byaProviderResponse.GetAt(i) == ASCII_CR)
			{
				if (m_byaProviderResponse.GetAt(i+1) == ASCII_ACK)
				{
					if (m_byaProviderResponse.GetAt(i+2) == ASCII_CR)
					{
						if (m_eSMSStatus != IDENTIFICATION)
						{
							WK_TRACE_WARNING(_T("CheckForIdentificationAck Wrong Status %s\n"),
																			LPCTSTR(NameOfStatus()));
						}
						m_eSMSStatus = IDENTIFICATION_ACK;
						if (m_byaProviderResponse.GetSize() > (i+3))
						{
							// may be GoOnIndication already
							// keep extra data
							CByteArray byaTemp;
							for (int j=i+3 ; j<m_byaProviderResponse.GetSize() ; j++)
							{
								byaTemp.Add(m_byaProviderResponse.GetAt(j));
							}
							m_byaProviderResponse.RemoveAll();
							m_byaProviderResponse.Append(byaTemp);
							CheckForGoOnIndication();
						}
						else
						{
							m_byaProviderResponse.RemoveAll();
						}
						bReturn = TRUE;
						break;
					}
				}
			}
		}
		else
		{
			// not enough data, wait for more
			break;
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCapiSMS_T_Mobile::CheckForGoOnIndication()
{
	BOOL bReturn = FALSE;
	// Response expected
	// 1b 5b 70 0d
	// <ESC>[p<CR>
	for (int i=0 ; i<m_byaProviderResponse.GetSize() ; i++)
	{
		if (m_byaProviderResponse.GetSize() >= (i+4))
		{
			if (m_byaProviderResponse.GetAt(i) == ASCII_ESC)
			{
				if (m_byaProviderResponse.GetAt(i+1) == 0x5b) // [
				{
					if (m_byaProviderResponse.GetAt(i+2) == 0x70) // p
					{
						if (m_byaProviderResponse.GetAt(i+3) == ASCII_CR)
						{
							if (m_eSMSStatus != IDENTIFICATION_ACK)
							{
								WK_TRACE_WARNING(_T("CheckForGoOnIndication Wrong Status %s\n"),
																			LPCTSTR(NameOfStatus()));
							}
							if (m_byaProviderResponse.GetSize() > (i+4))
							{
								WK_TRACE_WARNING(_T("CheckForGoOnIndication Data too long %i\n"),
															m_byaProviderResponse.GetSize());
							}
							m_byaProviderResponse.RemoveAll();
							m_eSMSStatus = GO_ON_INDICATION;
							SendTransaction();
							bReturn = TRUE;
							break;
						}
					}
				}
			}
		}
		else
		{
			// not enough data, wait for more
			break;
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCapiSMS_T_Mobile::CheckForTransactionAck()
{
	BOOL bReturn = FALSE;
	// May be ACK, NAK or error
	// [<msg seq>]<CR><ACK><CR>	wenn die Transaktion positiv bestätigt wird,
	// [<msg seq>]<CR><NAK><CR>	wenn die Prüfsumme fehlerhaft ist (Übertragungsfehler)
	// [<msg seq>]<CR><RS><CR>	wenn die Transaktion inhaltlich fehlerhaft ist

	CString sMsg;
	BOOL bAck = FALSE;
	for (int i=0 ; i<m_byaProviderResponse.GetSize() ; i++)
	{
		if (m_byaProviderResponse.GetSize() >= (i+3))
		{
			if (m_byaProviderResponse.GetAt(i) == ASCII_CR)
			{
				if (m_byaProviderResponse.GetAt(i+2) == ASCII_CR)
				{
					// Is there a system message in front?
					for (int j=0 ; j<i ; j++)
					{
						sMsg += m_byaProviderResponse.GetAt(j);
					}
					DecodeProviderMsg(sMsg);

					if (m_eSMSStatus != TRANSACTION)
					{
						WK_TRACE_WARNING(_T("CheckForTransactionAck Wrong Status %s\n"),
																		LPCTSTR(NameOfStatus()));
					}

					// check for ACK, NAK, Error
					if (m_byaProviderResponse.GetAt(i+1) == ASCII_ACK)	// ok
					{
						bAck = TRUE;
						m_eSMSStatus = TRANSACTION_ACK;
						SendEndOfTransmission();
					}
					else if (m_byaProviderResponse.GetAt(i+1) == ASCII_NAK)	// CRC error
					{
						m_eSMSStatus = TRANSACTION_NAK;
						if (m_iRepeatCounter < 3)
						{
							SendTransaction();
						}
						else
						{
							SendEndOfTransmission();
						}
					}
					else if (m_byaProviderResponse.GetAt(i+1) == ASCII_RS)	// syntax error
					{
						SendEndOfTransmission();
					}
					else
					{
						// OOPS what is that?
						WK_TRACE_ERROR(_T("CheckForTransactionAck Wrong Answer 0x%02x\n"),
													m_byaProviderResponse.GetAt(i+1));
						m_eSMSStatus = TRANSACTION_ERROR;
						SendEndOfTransmission();
					}
					bReturn = TRUE;
					break;
				}
			}
		}
		else
		{
			// not enough data, wait for more
			break;
		}
	}
	if (bAck)
	{
		if (   (m_eSMSStatus == TRANSACTION_ACK)
			|| (m_eSMSStatus == END_OF_TRANSMISSION)
			)
		{ // ok, accepted
			if (WK_IS_WINDOW(m_pParent))
			{
				m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_RESPONSE_OK);
			}
		}
		else
		{ // error
			if (WK_IS_WINDOW(m_pParent))
			{
				m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_RESPONSE_ERROR);
			}
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCapiSMS_T_Mobile::CheckForEndOfTransmission()
{
	BOOL bReturn = FALSE;
	// Response expected
	// 04 0D
	// <EOT><CR>
	for (int i=0 ; i<m_byaProviderResponse.GetSize() ; i++)
	{
		if (m_byaProviderResponse.GetSize() >= (i+2))
		{
			if (m_byaProviderResponse.GetAt(i) == ASCII_EOT)
			{
				if (m_byaProviderResponse.GetAt(i+1) == ASCII_CR)
				{
					// End of Transmission
					// SMSC ends the transmission active, we are allowed to hang up
					WK_TRACE_WARNING(_T("EndOfTransmission at Status %s\n"), LPCTSTR(NameOfStatus()));
					if (m_byaProviderResponse.GetSize() > (i+2))
					{
						WK_TRACE_WARNING(_T("CheckForEndOfTransmission Data too long %i\n"),
													m_byaProviderResponse.GetSize());
					}
					m_byaProviderResponse.RemoveAll();
					m_eSMSStatus = END_OF_TRANSMISSION;
					CloseConnection();
					bReturn = TRUE;
					break;
				}
			}
		}
		else
		{
			// not enough data, wait for more
			break;
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCapiSMS_T_Mobile::CheckForEndOfTransmissionAck()
{
	BOOL bReturn = FALSE;
	// Response expected
	// 0D 1B 04 0D
	// <CR><ESC><EOI><CR>
	for (int i=0 ; i<m_byaProviderResponse.GetSize() ; i++)
	{
		if (m_byaProviderResponse.GetSize() >= (i + 4))
		{
			if (m_byaProviderResponse.GetAt(i) == ASCII_CR)
			{
				if (m_byaProviderResponse.GetAt(i+1) == ASCII_ESC)
				{
					if (m_byaProviderResponse.GetAt(i+2) == ASCII_EOT)
					{
						if (m_byaProviderResponse.GetAt(i+3) == ASCII_CR)
						{
							// End of Transmission
							// SMSC acknowledges the end of transmission, we are allowed to hang up
							if (m_eSMSStatus != END_OF_TRANSMISSION)
							{
								WK_TRACE_WARNING(_T("CheckForEndOfTransmissionAck Wrong Status %s\n"),
																					LPCTSTR(NameOfStatus()));
							}
							if (m_byaProviderResponse.GetSize() > (i+4))
							{
								WK_TRACE_WARNING(_T("CheckForEndOfTransmissionAck Data too long %i\n"),
																	m_byaProviderResponse.GetSize());
							}
							m_byaProviderResponse.RemoveAll();
							m_eSMSStatus = END_OF_TRANSMISSION_ACK;
							CloseConnection();
							bReturn = TRUE;
							break;
						}
					}
				}
			}
		}
		else
		{
			// not enough data, wait for more
			break;
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS_T_Mobile::SendInitialSync()
{
	CByteArray byaSend;
	byaSend.Add(ASCII_CR);

	CString sTrace;
	for (int i=0 ; i<byaSend.GetSize() ; i++)
	{
		sTrace += GetAsciiSymbol(byaSend.GetAt(i));
	}
	TRACE(_T("Send:%s\n"), (LPCTSTR)sTrace);
	
	if (m_pCapi->SendRawData(byaSend.GetData(), (WORD)byaSend.GetSize()))
	{
		TRACE(_T("SendInitialSync OK\n"));
		if (WK_IS_WINDOW(m_pParent))
		{
			m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_SEND_OK, (LPARAM)1);
		}
		m_eSMSStatus = INITIAL_SYNC;
	}
	else
	{
		TRACE(_T("SendInitialSync FAILED\n"));
		if (WK_IS_WINDOW(m_pParent))
		{
			m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_SEND_FAILED, (LPARAM)1);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS_T_Mobile::SendIdentification()
{
	// Send identification
	// 1b 50 47 31 0d
	// <ESC>PG1<CR>
	CByteArray byaSend;
	byaSend.Add(ASCII_ESC);
	byaSend.Add(0x50);		// P
	byaSend.Add(0x47);		// G
	byaSend.Add(0x31);		// 1
	byaSend.Add(ASCII_CR);

	CString sTrace;
	for (int i=0 ; i<byaSend.GetSize() ; i++)
	{
		sTrace += GetAsciiSymbol(byaSend.GetAt(i));
	}
	TRACE(_T("Send:%s\n"), (LPCTSTR)sTrace);
	
	if (m_pCapi->SendRawData(byaSend.GetData(), (WORD)byaSend.GetSize()))
	{
		TRACE(_T("SendIdentification OK\n"));
		if (WK_IS_WINDOW(m_pParent))
		{
			m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_SEND_OK, (LPARAM)2);
		}
		m_eSMSStatus = IDENTIFICATION;
	}
	else
	{
		TRACE(_T("SendIdentification FAILED\n"));
		if (WK_IS_WINDOW(m_pParent))
		{
			m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_SEND_FAILED, (LPARAM)2);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS_T_Mobile::SendTransaction()
{
	// Send SMS
	CString sTrace;
	for (int i=0 ; i<m_byaTAPMessage.GetSize() ; i++)
	{
		sTrace += GetAsciiSymbol(m_byaTAPMessage.GetAt(i));
	}
	TRACE(_T("Send:%s\n"), (LPCTSTR)sTrace);
	
	if (m_pCapi->SendRawData(m_byaTAPMessage.GetData(), (WORD)m_byaTAPMessage.GetSize()))
	{
		m_iRepeatCounter++;
		TRACE(_T("SendTransaction OK\n"));
		if (WK_IS_WINDOW(m_pParent))
		{
			m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_SEND_OK, (LPARAM)3);
			m_eSMSStatus = TRANSACTION;
		}
	}
	else
	{
		TRACE(_T("SendTransaction FAILED\n"));
		if (WK_IS_WINDOW(m_pParent))
		{
			m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_SEND_FAILED, (LPARAM)3);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS_T_Mobile::SendEndOfTransmission()
{
	// Send end of transaction
	// 04 0d
	// <EOI><CR>
	CByteArray byaSend;
	byaSend.Add(ASCII_EOT);
	byaSend.Add(ASCII_CR);

	CString sTrace;
	for (int i=0 ; i<byaSend.GetSize() ; i++)
	{
		sTrace += GetAsciiSymbol(byaSend.GetAt(i));
	}
	TRACE(_T("Send:%s\n"), (LPCTSTR)sTrace);
	
	if (m_pCapi->SendRawData(byaSend.GetData(), (WORD)byaSend.GetSize()))
	{
		TRACE(_T("SendEndOfTransaction OK\n"));
		if (WK_IS_WINDOW(m_pParent))
		{
			m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_SEND_OK, (LPARAM)4);
		}
		m_eSMSStatus = END_OF_TRANSMISSION;
	}
	else
	{
		TRACE(_T("SendEndOfTransaction FAILED\n"));
		if (WK_IS_WINDOW(m_pParent))
		{
			m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_SEND_FAILED, (LPARAM)4);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CCapiSMS_T_Mobile::DecodeProviderMsg(CString sMsg)
{
	m_sProviderError = sMsg;
}
