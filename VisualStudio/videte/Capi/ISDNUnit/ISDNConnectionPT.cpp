/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ISDNConnectionPT.cpp $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/ISDNConnectionPT.cpp $
// CHECKIN:		$Date: 16.12.05 14:10 $
// VERSION:		$Revision: 23 $
// LAST CHANGE:	$Modtime: 16.12.05 10:47 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "IsdnConnection.h"
#include "CapiQueue.h"
#include "CIPCPipeInputPT.h"
#include "CIPCPipeOutputPT.h"
#include "PictureRecord.h"
#include "TimedMessage.h"

#include "pt.h"
#include "PTBox.h"
#include "WK_Names.h"	// for StartExplorer
#include "IPCFetch.h"
#include "CIPCServerControlClientSide.h"
#include "ISDNUnit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// @doc
// @mfunc HexDump using WK_TRACE
void CISDNConnection::HexDump(
			const BYTE *pData,		// @parm the data, which is to be dumped
			DWORD dwDataLen,		// @parm len of data
			LPCTSTR szPre		// @parm string to prepend each line
			)
{
	CString sOneLine;
	CString sOneValue;
	for (DWORD i=0;i<(int)dwDataLen;i+=8) {
		sOneLine.Format(_T("%s%03x: "),szPre,i);

		DWORD j=0;
		for (j=i;j<i+8;j++) {
			if (j<dwDataLen) {
				sOneValue.Format(_T(" %02x"),pData[j]);
			} else {
				sOneValue=_T("   ");
			}
			sOneLine += sOneValue;
		}

		sOneLine += _T("    |");

		for (j=i;j<i+8;j++) {
			if (j<dwDataLen) {
				if (isprint(pData[j])) {
					sOneValue.Format(_T("%c"),pData[j]);
				} else {
					sOneValue.Format(_T("."));
				}
			} else {
				sOneValue=_T(" ");
			}
			sOneLine += sOneValue;
		}

		WK_TRACE(_T("%s|\n"),sOneLine);

	}
}

// @mfunc NYD
void CISDNConnection::OnReceiveData(CapiHandle ch, BYTE *pData, DWORD dwDataLen)
{
	// WK_TRACE(_T("Received %d bytes raw data\n"),dwDataLen);
	if (IsISTConnection()) {
		PanicClose(_T("Can't handle raw data in IST connection"));
		return;	// <<< EXIT >>>
	} else if (IsPTConnection()) {
		m_pPTBox->OnRawData(pData,dwDataLen);
	} else {
		PanicClose(_T("Invalid connection combination in ReceiveRawData"));
	}
}

// @mfunc NYD
void CISDNConnection::HandlePTAlarmCall()
{
	WK_TRACE(_T("HandlePTAlarmCall\n"));
	if (WK_IS_RUNNING(WK_APP_NAME_EXPLORER)==FALSE) 
	{
		if (CIPCServerControlClientSide::StartVision()) 
		{
			Sleep(500);	// OOPS
		} 
		else 
		{
			// failed to start
		}
	}
	//
	// create client connections first
	// the created shared memory names are used in the FetchAlarmConnection

	CString sSuffix;
	if (MyGetApp()->IsSecondInstance()) {
		sSuffix = _T("_2");
	}

	// Add new input pipe to ISDN connection
	WORD wNewPipeOneNr = CIPCPipeISDN::GetNextGlobalID();
	CString sShmName;
	CIPCPipeISDN *pNewPipeOne = NULL;
	CIPCPipeISDN *pNewPipeTwo = NULL;

	sShmName.Format(_T("PTAlarmPipeInput%d"),wNewPipeOneNr);
	sShmName += sSuffix;
	m_pPTBox->m_pInputPT = new CIPCPipeInputPT(
						CIPC_INPUT_CLIENT,
							this,
							sShmName,
							CIPC_MASTER,
							0,
							wNewPipeOneNr,
							0,	// dwRequestCounter
							TRUE	// alarm connection
							);
	pNewPipeOne = m_pPTBox->m_pInputPT;

	m_InputPipesClient.SafeAdd(pNewPipeOne);
	m_bThereWerePipesBefore = TRUE;
	pNewPipeOne->GetCIPC()->StartThread(TRUE);
	if (theApp.TracePipes())
	{
		WK_TRACE(_T("%s '%s'\n"), _T(__FUNCTION__), sShmName);
	}
	
	// Add new output pipe to ISDN connection
	WORD wNewPipeTwoNr = CIPCPipeISDN::GetNextGlobalID();
	sShmName.Format(_T("PTAlarmPipeOutput%d"),wNewPipeTwoNr);
	sShmName += sSuffix;
	if (m_pPTBox->m_pOutputPT) {
		PanicClose(_T("Internal error, still have outputPT\n"));
		return;	// <<< EXIT >>>
	} else {
		m_pPTBox->m_pOutputPT = new CIPCPipeOutputPT(
								CIPC_OUTPUT_CLIENT,
								this,
								sShmName,
								CIPC_MASTER,
								0,	// options NOT YET c.GetOptions(),
								wNewPipeTwoNr , 
								0, // dwRequestCounter
								TRUE	// alarm connection
							);
		pNewPipeTwo = m_pPTBox->m_pOutputPT;
		m_OutputPipesClient.SafeAdd(pNewPipeTwo);
		m_bThereWerePipesBefore = TRUE;
		pNewPipeTwo->GetCIPC()->StartThread(TRUE);
		if (theApp.TracePipes())
		{
			WK_TRACE(_T("%s '%s'\n"), _T(__FUNCTION__), sShmName);
		}
	}

	CConnectionRecord connectRecord;
	connectRecord.SetSourceHost(m_pPTBox->GetBoxName());
	// NOT YET all data
	connectRecord.SetDestinationHost(LOCAL_LOOP_BACK);	// OOPS cascade ?

	// WK_TRACE(_T("Alarm CamID is %d\n"),m_pPTBox->m_alarmCamID.GetSubID());
	connectRecord.SetCamID(m_pPTBox->m_alarmCamID);
	connectRecord.SetInputShm(pNewPipeOne->GetCIPC()->GetShmName());
	connectRecord.SetOutputShm(pNewPipeTwo->GetCIPC()->GetShmName());

	Sleep(250);	// OOPS test, there seem to be a timing problem

	CIPCFetchResult fetchResult;
	CIPCFetch fetch;

	fetchResult = fetch.FetchAlarmConnection(connectRecord);

	if (fetchResult.IsOkay()) 
	{
		// confirm is below
		WK_TRACE(_T("CipcPipeRequest() Alarm Request %u successful\n"),
											connectRecord.GetOptions());

		
		Sleep(50);	// OOPS test, there seems to be a timing problem
	} 
	else 
	{
		CIPCFetchResult dummyFetchResult(
			_T(""),
			CIPC_ERROR_UNABLE_TO_CONNECT, 0,	// NOT YET
			_T("Alarm connection failed"),
			0,
			SECID_NO_ID
#ifdef _UNICODE
			, CODEPAGE_UNICODE
#endif
		);
		fetchResult = dummyFetchResult;
		WK_TRACE(_T("ERROR: CISDNConnection::CipcPipeRequest() FetchAlarm failed\n"));				// failed
		RemovePipe(pNewPipeOne);
		pNewPipeOne = NULL;
		RemovePipe(pNewPipeTwo);
		pNewPipeTwo = NULL;
	}
}