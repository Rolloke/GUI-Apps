/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ISDNConnectionMisc.cpp $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/ISDNConnectionMisc.cpp $
// CHECKIN:		$Date: 1/03/06 12:10p $
// VERSION:		$Revision: 23 $
// LAST CHANGE:	$Modtime: 1/02/06 4:16p $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "resource.h"
#include "IsdnConnection.h"
#include "user.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// @doc
/////////////////////////////////////////////////////////////////////////////
// NOT YET check thread relations
// OOPS only called for failure of m_pCapi->NewCreate
// Therefor GP because AfxMessageBox while thread waits for return with Timeout!!!
void CISDNConnection::CapiError(int iError) 
{
	CString sMsg;
	sMsg.LoadString(IDS_UNIT_NOT_STARTED);
	if ( iError>=1 && iError<=10 ) {
		sMsg.LoadString(IDS_CAPI_NOT_ENOUGH_MEMORY);
	}
	else if ( iError==INF_TOO_MANY_APPL ) {
		sMsg.LoadString(IDS_TOO_MANY_APPL);
	}
	else if ( iError==INF_BLOCK_SIZE_TOO_SMALL ) {
//		sMsg.LoadString(IDS_BLOCK_SIZE_TOO_SMALL);
	}
	else if ( iError==INF_BUF_EXCEEDS_64K ) {
//		sMsg.LoadString(IDS_BUF_EXCEEDS_64K);
	}
	else if ( iError==INF_MSG_BUF_TOO_SMALL ) {
//		sMsg.LoadString(IDS_MSG_BUF_TOO_SMALL);
	}
	else if ( iError==INF_MAX_NR_CONN_NOSUPPORT ) {
//		sMsg.LoadString(IDS_MAX_NR_CONN_NOSUPPORT);
	}
	else if ( iError==INF_INTERNAL_BUSY_1 ) {
//		sMsg.LoadString(IDS_INTERNAL_BUSY_1);
	}
	else if ( iError==INF_NO_MEMORY_1 ) {
//		sMsg.LoadString(IDS_NO_MEMORY_1);
	}
	else if ( iError==INF_CAPI_NOT_INSTALLED_1 ) {
		sMsg.LoadString(IDS_CAPI_NOT_INSTALLED_1);
	}
	else if ( iError==INF_EXT_EQU_NOSUPPORT_1 ) {
//		sMsg.LoadString(IDS_EXT_EQU_NOSUPPORT_1);
	}
	else if ( iError==INF_EXT_EQU_ONLY_SUPPORT_1 ) {
//		sMsg.LoadString(IDS_EXT_EQU_ONLY_SUPPORT_1);
	}
	WK_TRACE(_T("%s\n"),sMsg);
	// Never call a AfxMessageBox, because it is in CapiThread
	// while main thread is waiting for return of CapiThread!
	// -> MsgBox will never appear!!!
	// Because MainWnd is not visible yet, a COemGuiApi::MessageBox
	// does not appear either and can not be destroyed -> Memory leak
//	AfxMessageBox(sMsg, MB_OK|MB_ICONSTOP);
//	COemGuiApi::MessageBox(sMsg, 60, MB_OK|MB_ICONSTOP);
}

/////////////////////////////////////////////////////////////////////////////
/*@MFUNC
@PARM word | wInfo | CAPI message
*/
void CISDNConnection::InfoMsg(WORD wInfo)
{
	// spezial info, message ignore
	if ( wInfo == INF_20_MESSAGE_NOT_SUPPORTED ) {
		WK_TRACE(_T("InfoMsg %04x Message not supported\n"), wInfo);
	} else {
		WK_TRACE(_T("InfoMsg %04x\n"), wInfo);
		WhatsHappened(wInfo);
	}
}

/*@MFUNC
@PARM word | wInfo | CAPI message

returns a CString repesentation of wInfo
*/
CString CISDNConnection::NameOfCapiInfo(WORD wInfo)
{
	CString sMsg,sTmp;

	sMsg.Format(_T("CapiInfo %x"),wInfo);	// default

	if ( 0x0001<=wInfo && wInfo<=0x0fff )
	{
		// only informational, corresponding message was processed
	} 
	else if ( 0x1000<=wInfo && wInfo<=0x10ff ) 
	{
		// error information concerning CAPI_REGISTER, normaly not suspected
		sTmp.LoadString(IDS_CE_10XX);
		sMsg.Format(_T("%s [%x]"),(LPCTSTR)sTmp,wInfo);
	}
	else if ( 0x1100<=wInfo && wInfo<=0x11ff )
	{
		// error information concerning message exchange functions
		sTmp.LoadString(IDS_CE_11XX);
		sMsg.Format(_T("%s [%x]"),(LPCTSTR)sTmp,wInfo);
	}
	else if ( 0x1200<=wInfo && wInfo<=0x1fff )
	{
		// group not defined
		WK_TRACE(_T("WhatsHappened %04x OOPS UNKNOWN\n"), wInfo);
		// NOT YET sMsg
	}
	else if ( 0x2000<=wInfo && wInfo<=0x20ff )
	{
		// error information concerning resource / coding problems
		sTmp.LoadString(IDS_CE_10XX);
		sMsg.Format(_T("%s [%x]"),(LPCTSTR)sTmp,wInfo);
	} 
	else if ( 0x2100<=wInfo && wInfo<=0x2fff ) 
	{
		// group not defined
		WK_TRACE(_T("WhatsHappened %04x OOPS UNKNOWN\n"), wInfo);
		// NOT YET sMsg
	}
	else if ( 0x3000<=wInfo && wInfo<=0x30ff ) 
	{
		// error information concerning requested services
		if (wInfo == INF_B1_PROTOCOL_NOT_SUPPORTED) 
		{
			sMsg.LoadString(IDS_CE_B1_PROTOCOL_NOT_SUPPORTED);
		}
		else if (wInfo == INF_B2_PROTOCOL_NOT_SUPPORTED) 
		{
			sMsg.LoadString(IDS_CE_B2_PROTOCOL_NOT_SUPPORTED);
		}
		else if (wInfo == INF_B3_PROTOCOL_NOT_SUPPORTED)
		{
			sMsg.LoadString(IDS_CE_B3_PROTOCOL_NOT_SUPPORTED);
		}
		else if (wInfo == INF_B1_PARAMS_NOT_SUPPORTED)
		{
			sMsg.LoadString(IDS_CE_B1_PARAMS_NOT_SUPPORTED);
		}
		else if (wInfo == INF_B2_PARAMS_NOT_SUPPORTED)
		{
			sMsg.LoadString(IDS_CE_B2_PARAMS_NOT_SUPPORTED);
		}
		else if (wInfo == INF_B3_PARAMS_NOT_SUPPORTED) 
		{
			sMsg.LoadString(IDS_CE_B3_PARAMS_NOT_SUPPORTED);
		} 
		else if (wInfo == INF_B_PROTOCOL_COMBINATION_NOT_SUPPORTED)
		{
			sMsg.LoadString(IDS_CE_B_PROTOCOL_COMBINATION_NOT_SUPPORTED);
		}
		else
		{
			sTmp.LoadString(IDS_CE_30XX);
			sMsg.Format(_T("%s [%x]"),(LPCTSTR)sTmp,wInfo);
		}
	}
	else if ( 0x3100<=wInfo && wInfo<=0x32ff )
	{
		// group not defined
		WK_TRACE(_T("WhatsHappened %04x OOPS UNKNOWN\n"), wInfo);
	}
	else if ( 0x3300<=wInfo && wInfo<=0x33ff )
	{
		// error information regarding the clearing of a physical connection
		if (wInfo == INF_PROTOCOL_ERROR_LAYER_1)
		{
			sMsg.LoadString(IDS_CE_PROTOCOL_ERROR_LAYER_1);
		}
		else if (wInfo == INF_PROTOCOL_ERROR_LAYER_2)
		{
			sMsg.LoadString(IDS_CE_PROTOCOL_ERROR_LAYER_2);
		}
		else if (wInfo == INF_PROTOCOL_ERROR_LAYER_3) 
		{
			sMsg.LoadString(IDS_CE_PROTOCOL_ERROR_LAYER_3);
		}
		else if (wInfo == INF_ANOTHER_APPLICATION_GOT_THE_CALL)
		{
			sMsg.LoadString(IDS_CE_ANOTHER_APPLICATION_GOT_THE_CALL);
		}
		else
		{
			sTmp.LoadString(IDS_CE_33XX);
			sMsg.Format(_T("%s [%x]"),(LPCTSTR)sTmp,wInfo);
		}
	}
	else if ( 0x3400<=wInfo && wInfo<=0x34ff )
	{
		// error information regarding the clearing of a physical connection
		if ( wInfo == INF_20_NORMAL_CALL_CLEARING )
		{
			// OOPS dropped m_bActiveRemoving pipes here
			sMsg.LoadString(IDS_CE_NORMAL_CALL_CLEARING);
		}
		else if (wInfo == INF_20_USER_BUSY)
		{
			sMsg.LoadString(IDS_CE_USER_BUSY);
		}
		else if (wInfo == INF_20_NO_USER_RESPONDING)
		{
			sMsg.LoadString(IDS_CE_NO_USER_RESPONDING);
		}
		else if (wInfo == INF_20_NO_ANSWER_FROM_USER)
		{
			sMsg.LoadString(IDS_CE_NO_ANSWER_FROM_USER);
		}
		else if (wInfo == INF_20_CALL_REJECTED)
		{
			sMsg.LoadString(IDS_CE_CALL_REJECTED);
		} 
		else if (wInfo == INF_20_NUMBER_CHANGED)
		{
			sMsg.LoadString(IDS_CE_NUMBER_CHANGED);
		}
		else if (wInfo == INF_20_NON_SELECTED_USER_CLEARING)
		{
			sMsg.LoadString(IDS_CE_NON_SELECTED_USER_CLEARING);
		}
		else if (wInfo == INF_20_DESTINATION_OUT_OF_ORDER)
		{
			sMsg.LoadString(IDS_CE_DESTINATION_OUT_OF_ORDER);
		}
		else if (wInfo == INF_20_INVALID_NUMBER_FORMAT) 
		{
			sMsg.LoadString(IDS_CE_INVALID_NUMBER_FORMAT);
		}
		else if (wInfo == INF_20_FACILITY_REJECTED)
		{
			sMsg.LoadString(IDS_CE_FACILITY_REJECTED);
		}
		else if (wInfo == INF_20_RESPONSE_TO_STATUS_ENQUIRY) 
		{
			sMsg.LoadString(IDS_CE_RESPONSE_TO_STATUS_ENQUIRY);
		} 
		else if (wInfo == INF_20_NORMAL_UNSPECIFIED)
		{
			// OOPS dropped m_bActiveRemoving pipes here
			sMsg.LoadString(IDS_CE_NORMAL_UNSPECIFIED);
		}
		else if (wInfo == INF_20_NO_CHANNEL_AVAILABLE)
		{
			sMsg.LoadString(IDS_CE_NO_CHANNEL_AVAILABLE);
		}
		else if (wInfo == INF_20_NETWORK_OUT_OF_ORDER)
		{
			sMsg.LoadString(IDS_CE_NETWORK_OUT_OF_ORDER);
		}
		else if (wInfo == INF_20_TEMPORARY_FAILURE)
		{
			sMsg.LoadString(IDS_CE_TEMPORARY_FAILURE);
		}
		else 
		{
			sTmp.LoadString(IDS_CE_34XX);
			sMsg.Format(_T("%s [%x]"),(LPCTSTR)sTmp,wInfo);
		}
	} 
	else 
	{
		// group not defined
		WK_TRACE(_T("WhatsHappened %04x OOPS UNKNOWN\n"), wInfo);
	}

	return sMsg;
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::WhatsHappened(WORD wInfo)
{
	BOOL bError = FALSE;
	CString sMsg = NameOfCapiInfo(wInfo);

	// messages sorted by groups
	if ( 0x0001<=wInfo && wInfo<=0x0fff ) 
	{
		// only informational, corresponding message was processed
	}
	else if ( 0x1000<=wInfo && wInfo<=0x10ff ) 
	{	// error information concerning CAPI_REGISTER, normaly not suspected
		bError = TRUE;
	}
	else if ( 0x1100<=wInfo && wInfo<=0x11ff ) 
	{	// error information concerning message exchange functions
		bError = TRUE;
	}
	else if ( 0x1200<=wInfo && wInfo<=0x1fff ) 
	{	// group not defined
		WK_TRACE(_T("WhatsHappened %04x OOPS UNKNOWN\n"), wInfo);
	}
	else if ( 0x2000<=wInfo && wInfo<=0x20ff ) 
	{	// error information concerning resource / coding problems
		bError = TRUE;
	}
	else if ( 0x2100<=wInfo && wInfo<=0x2fff ) 
	{	// group not defined
		WK_TRACE(_T("WhatsHappened %04x OOPS UNKNOWN\n"), wInfo);
	}
	else if ( 0x3000<=wInfo && wInfo<=0x30ff ) 
	{	// error information concerning requested services
		bError = TRUE;
	}
	else if ( 0x3100<=wInfo && wInfo<=0x32ff ) 
	{	// group not defined
		WK_TRACE(_T("WhatsHappened %04x OOPS UNKNOWN\n"), wInfo);
	}
	else if ( 0x3300<=wInfo && wInfo<=0x33ff ) 
	{
		bError = TRUE;
	}
	else if ( 0x3400<=wInfo && wInfo<=0x34ff ) 
	{
		bError = TRUE;
		// error information regarding the clearing of a physical connection
		if ( wInfo == INF_20_NORMAL_CALL_CLEARING ) 
		{
			if (m_bActiveRemovingPipes) 
			{
				bError = FALSE;
			}
		}
		else if (wInfo == INF_20_NORMAL_UNSPECIFIED) 
		{
			if (m_bActiveRemovingPipes)
			{
				bError = FALSE;
			}
		}
		// OOPS HEDU
		if (wInfo==0x3491 || wInfo==0x349f || wInfo==0x3495) 
		{	// busy, might be the SECOND channel only
			// 0x3303 NOT YET
			if (GetNumBChannels()==2)
			{
				WK_TRACE(_T("second channel closed...\n"));
				bError = FALSE;	// move on
				// NOT YET decrement channel count now ?
				// or ist it done by Capi NrBChannelsChanged
			}
		}
	}
	else 
	{
		// group not defined
		WK_TRACE(_T("WhatsHappened %04x OOPS UNKNOWN\n"), wInfo);
	}

	if (bError)
	{ 
		if ( GetConnectState() != CONNECT_CLOSED 
				&& GetConnectState() != CONNECT_CLOSING )
		{
			// OOPS any Capi actions ? what about a PanicClose
			SetRemoteNumber(_T(""));
			SetRemoteHostName(_T(""));
			m_CallState = CALL_NONE;
			WK_TRACE(_T("Closing... info is %x\n"),wInfo);
			m_ConnectState = CONNECT_CLOSING;
		}
	}
}

void CISDNConnection::SetRemoteHostName(const CString &sName)
{
	// NOT YET some STAT_LOG stuff
	if (sName.GetLength())
	{
		m_sRemoteHostName = sName;
		WK_TRACE(_T("RemoteHostName is '%s'\n"),(LPCTSTR)sName);
	}
	else
	{
		m_sRemoteHostName = _T("");	// reset 
	}
}
void CISDNConnection::SetRemoteNumber(const CString &sNumber)
{
	// NOT YET some validation
	// NOT YET some STAT_LOG stuff
	m_sRemoteNumber = sNumber;
}


/////////////////////////////////////////////////////////////////////////////
/*@mfunc
NYD
*/
CString CISDNConnection::FilterDigits( const CString& sString)
{
	// collect digits onlye
	CString sDigits;
	LPCTSTR pszString = sString;
	int nLen = sString.GetLength();
	for (int i=0;i<nLen;i++)
	{
		if (isdigit((char)pszString[i]))
		{
			sDigits += pszString[i];
		}
	}

	return sDigits;
}



