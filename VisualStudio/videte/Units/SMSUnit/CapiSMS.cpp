// CapiSMS.cpp: implementation of the CCapiSMS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "smsunit.h"
#include "CapiSMS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static TCHAR BASED_CODE szSection[]		= _T("SMSUnit");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCapiSMS::CCapiSMS(CWnd* pParent)
{
	m_pParent			= pParent;
	m_pCapi				= NULL;
	m_sProviderID		= _T("");
	m_ConnectState		= CONNECT_CLOSED;
	m_sProviderError	= _T("");

	BOOL bDllOkay = LoadDll();	// in capi4.h
	if (bDllOkay==FALSE)
	{
		CString sMsg;
		sMsg.LoadString(IDS_FAILED_LOAD_CAPI);
		CWK_RunTimeError runTimeError(REL_CANNOT_RUN, RTE_CONFIGURATION, sMsg);
		runTimeError.Send();
		m_ConnectState = CONNECT_ERROR;
	}
}
//////////////////////////////////////////////////////////////////////
CCapiSMS::~CCapiSMS()
{
	WK_DELETE(m_pCapi);
	UnloadDll();	// in capi4.h
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCapiSMS::InitCapi(HWND hHiddenWnd) 
{
	BOOL bRet = TRUE;
	if (m_pCapi)
	{
		m_pCapi->Close();	// dunno what's the right one
		m_pCapi->Reset();	// I don't trust the destructor
	}
	else
	{
		m_pCapi = new CCapi();

		int iCapiRet = m_pCapi->NewCreate(this,
										  hHiddenWnd, 
										  NET_CAPI_MSG, 
										  (WORD)GetNumChannels(),
										  8,						// DEFAULT_X_BUFFERS,
										  PROT_NOHANDSAKE, 
										  7,						// wMyMaxMovingBlocks=MAX_MOVING_BLOCKS,
										  FALSE,					// bChannelBundling, not for raw data anyway
										  TRUE						// raw data
										 );

		if (iCapiRet != 0)
		{
			CString sMsg;
			sMsg.Format(IDS_CAPI_NOT_CREATED, iCapiRet);
			CWK_RunTimeError runTimeError(REL_CANNOT_RUN, RTE_CONFIGURATION, sMsg);
//			runTimeError.Send();
			m_ConnectState = CONNECT_ERROR;
			bRet = FALSE;
		}
	}
	if (bRet)
	{
		// disable CCapi Password handshake
		m_pCapi->SetPasswort(NULL);
		// reject all incomming calls
		m_pCapi->ListenRequest(LISTEN_REJECTALL);
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS::PollConnection()
{
	if (m_pCapi)
	{
		m_pCapi->Poll();
	}
}
//////////////////////////////////////////////////////////////////////
void CCapiSMS::SetOwnNumber(CString sOwnNumber)
{
	m_sOriginatorAddress = sOwnNumber;
}
//////////////////////////////////////////////////////////////////////
void CCapiSMS::SetProviderID(CString sProviderID)
{
	m_sProviderID = sProviderID;
}
//////////////////////////////////////////////////////////////////////
void CCapiSMS::CloseConnection()
{
	if (m_pCapi)
	{
		if (   (m_ConnectState != CONNECT_REQUEST)
			&& (m_ConnectState != CONNECT_OPEN)
			)
		{
			WK_TRACE(_T("CapiSMS CloseConnection not requested nor opened\n"));
		}
		m_pCapi->Close();
		m_ConnectState = CONNECT_CLOSING;
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CCapiSMS::ReceiveCapiMessage(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("ReceiveCapiMessage wParam %08x lParam %08x\n"), wParam, lParam);
	long lReturn = 0;
	switch (wParam)
	{   
		case C_INCOMMING:
			// Should normally not com at REJECTALL
			// lParam PCALLINFO
			if (lParam)
			{
				LPCALLINFO pCallInfo= (LPCALLINFO)lParam;
				CWK_String sCallingParty(pCallInfo->szCallingParty);
				CWK_String sCalledParty(pCallInfo->szCalledParty);
				WK_TRACE(_T("Incomming call from '%s' to '%s'\n"), sCallingParty, sCalledParty);
				WK_TRACE(_T("Call will be rejected\n"));
			}
			// return 0 to reject call
			lReturn = 0;
			break;
		case C_OUTGOING:
			{
				// lParam LPSTR Nr
				// NOT YET any validation on szRemoteNumber
				LPCTSTR szRemoteNumber = (LPCTSTR)lParam;
				WK_TRACE(_T("Outgoing Call to '%s'\n"), szRemoteNumber);
			}
			break;
		case C_REJECTING: 
			// Should normally not com at REJECTALL
			// lParam PCALLINFO
			if (lParam)
			{
				LPCALLINFO pCallInfo= (LPCALLINFO)lParam;
				CWK_String sCallingParty(pCallInfo->szCallingParty);
				CWK_String sCalledParty(pCallInfo->szCalledParty);
				WK_TRACE(_T("CallRejected from '%s' to '%s'\n"), LPCTSTR(sCallingParty), LPCTSTR(sCalledParty));
			}
			break;
		case C_OUTGOINGOK:		 
			// lParam (LPCSTR) TelNumber
			OnConnectReady();
			break;
		case C_INCOMMINGOK:
		{
			// Should normally not com at REJECTALL
			// lParam (LPCSTR) TelNumber
			CWK_String sTxt((LPCSTR)lParam);
			WK_TRACE(_T("Incomming call connected %s\n"),(LPCTSTR)sTxt);
			CloseConnection();
		} break;
		case C_HANGUP:
			// lParam 0L
			OnOutgoingDone(NULL, OUTGOING_ACCEPTED);
		    break;      
		case C_CHANNEL_HANGUP:
		{	// lParam LPCSTR TelNumber
			CWK_String sTxt((LPCSTR)lParam);
			WK_TRACE(_T("ChannelClosed %s\n"),(LPCTSTR)sTxt);
		} break;
		case C_NODISCONNECT:
			// lParam LPCSTR TelNumber
			CloseFailed((LPCSTR)lParam);
			break;
		case C_INFO:
			// LOWORD -lParam INF-Msg
			OnInfo(LOWORD(lParam));
			break;
		case C_NR_CONNECT: 
			// LOWORD -lParam B-Kanaele
			OnNumBChannelsChanged(NULL, LOWORD(lParam));
			break; 
		case C_DATA:
			// lParam - LPBYTE Data
			TRACE(_T("Received Data normal\n"));
			break;           
		case C_UNKNOWNDATA:
			// lParam - LPBYTE Data
			TRACE(_T("Received Data unknown\n"));
			break;           
		case C_LISTENREQUEST:
			// LOWORD -lParam INF-Msg or 0
			OnListenConfirm(LOWORD(lParam));
			break;
		case C_USER_BUSY:
			// lParam = 0L	// OOPS ever called?
			OnUserBusy();
			break;
		case C_CALL_REJECTED:
			// lParam = 0L	// OOPS ever called?
			OnCallRejected();
			break;
		default:
			WK_TRACE(_T("Old style Capi function called %u, lParam %08x\n"), wParam, lParam);
	}	// end of switch(wParam)
	return lReturn;
}
/////////////////////////////////////////////////////////////////////////////
// CAQbstractCapiClient functions
const CString& CCapiSMS::GetOwnNumber() const
{
	return m_sOriginatorAddress;
}
/////////////////////////////////////////////////////////////////////////////
int CCapiSMS::GetNumChannels() const
{
	// we only need one channel, no more ever!
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS::OnListenConfirm(WORD wListen)
{
	TRACE(_T("OnListenConfirm %04x\n"), wListen);
}
/////////////////////////////////////////////////////////////////////////////
CapiIncomingAnswer CCapiSMS::OnIncomingCall(CapiHandle newCh, 
											const CAbstractCapiCallInfo &callInfo
											)
{
	WK_TRACE(_T("IncommingCall() from '%s' to '%s'\n"),
					LPCTSTR(callInfo.GetCallingParty()),
					LPCTSTR(callInfo.GetCalledParty())
			);

	// Always ignore
	return INCOMING_REJECTED_IGNORE;
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS::OnIncomingOkay(CapiHandle newCh, const CString &sNumber)
{
	TRACE(_T("OnIncomingOkay\n"));
}
/////////////////////////////////////////////////////////////////////////////
long CCapiSMS::OnOutgoingDone(CapiHandle ch, CapiOutgoingResult CapiResult)
{
	TRACE(_T("OnOutgoingDone\n"));
	m_ConnectState = CONNECT_CLOSED;
	if (WK_IS_WINDOW(m_pParent))
	{
		switch (CapiResult)
		{
			case OUTGOING_REJECTED:
				m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_REJECTED);
				break;
			case OUTGOING_USER_BUSY:
				m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_USER_BUSY);
				break;
			case OUTGOING_NOT_AVAILABLE:
				m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_NOT_AVAILABLE);
				break;
			case OUTGOING_ACCEPTED:
			default:
				m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_SMS_DISCONNECTED);
				break;
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS::OnNumBChannelsChanged(CapiHandle ch, WORD wChannels)
{
	TRACE(_T("OnNumBChannelsChanged %i\n"), wChannels);
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS::OnConnectionClosed(CapiHandle ch)
{
	TRACE(_T("OnConnectionClosed\n"));
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS::CloseFailed(const char* szCalledParty)
{
	TRACE(_T("CloseFailed\n"));
	m_ConnectState = CONNECT_CLOSED;
	m_pCapi->Reset();	
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS::OnReceiveData(CapiHandle ch, DEFHDR *pData)
{
	TRACE(_T("OnReceiveData idip HDR\n"));
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS::OnReceiveData(CapiHandle ch, BYTE *pData, DWORD dwDataLen)
{
	TRACE(_T("OnReceiveData normal\n"));
	OnReceiveSMSData(pData, dwDataLen);
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS::OnCallRejected()
{
	// handled via OnInfo
	TRACE(_T("CallRejected\n"));
	OnOutgoingDone(NULL, OUTGOING_REJECTED);
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS::OnUserBusy()
{
	// handled via OnInfo
	TRACE(_T("UserBusy\n"));
	OnOutgoingDone(NULL, OUTGOING_USER_BUSY);
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS::OnUnableToConnect()	// 0x34A2
{
	// handled via OnInfo
	TRACE(_T("Unable to connect\n"));
	OnOutgoingDone(NULL, OUTGOING_NOT_AVAILABLE);
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS::OnInfo(WORD wInfo)
{
	TRACE(_T("Info %04x\n"), wInfo);
}
/////////////////////////////////////////////////////////////////////////////
void CCapiSMS::OnCapiGetMessageError()
{
	// Substantial capi error, 
	// Call has failed anyway
	if (WK_IS_WINDOW(m_pParent))
	{
		m_pParent->PostMessage(CAPI_SMS_MSG, (WPARAM)CAPI_ERROR);
	}
}
// end of CAQbstractCapiClient functions
