/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcServerControl.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CipcServerControl.cpp $
// CHECKIN:		$Date: 20.04.06 12:26 $
// VERSION:		$Revision: 80 $
// LAST CHANGE:	$Modtime: 20.04.06 12:24 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

// 

#include "stdcipc.h"

#include "CipcServerControl.h"
#include "CipcExtraMemory.h"
#include "User.h"
#include "CipcInput.h"	// OOPS Update hack
#include "NotificationMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
CIPCServerControl::CIPCServerControl(LPCTSTR shmName, BOOL asMaster)
: CIPC(shmName, asMaster)
{
#ifdef _UNICODE
	// CIPCServerControl gets the request from the ServerControlClientSide
	// Initial requests are always coded in ANSI see CIPCServerControlClientSide::Init()
	m_wCodePage = (WORD)CWK_String::GetCodePage();
#endif
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
CIPCServerControl::~CIPCServerControl()
{
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCServerControl::DoRequestInputConnection(const CConnectionRecord &c)
{
	CIPCCmdRecord *pCmd;
	pCmd = c.CreateCmdRecord(this,SRV_CONTROL_REQUEST_INPUT_CONNECTION);
	WriteCmdWithExtraMemory(pCmd->m_pExtraMemory,
							pCmd->m_dwCmd,
							pCmd->m_dwParam1,
							pCmd->m_dwParam2,
							pCmd->m_dwParam3,
							pCmd->m_dwParam4);
	WK_DELETE(pCmd);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCServerControl::OnRequestInputConnection(const CConnectionRecord &c)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCServerControl::DoConfirmInputConnection(CSecID assignedID,
												 LPCTSTR shmName,
											     const CString& sVersion,
										         DWORD dwOptions
#ifdef _UNICODE
													, DWORD dwCodePage
#endif
													)
{
	CString sSharedMemoryName(shmName);
	CString slVersion = sVersion;
	if (slVersion.IsEmpty())
	{
		CString sExe;
		GetModuleFileName(AfxGetApp()->m_hInstance,sExe.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
		sExe.ReleaseBuffer();
		slVersion = WK_GetFileVersion(sExe);
	}

	CStringArray sa;
	
	sa.Add(sSharedMemoryName);
	sa.Add(slVersion);

	CIPCExtraMemory *pBubble = new CIPCExtraMemory();
	if (pBubble->Create(this,sa))
	{
		WriteCmdWithExtraMemory(pBubble,
								SRV_CONTROL_CONFIRM_INPUT_CONNECTION,	// cmd
								assignedID.GetID(),							// param1
								dwOptions										// param2
#ifdef _UNICODE
								, MAKELONG(LOWORD(dwCodePage), 0)					// param3
#endif
								);				
	}
	else
	{
		WK_DELETE(pBubble);
		OnCommandSendError(SRV_CONTROL_CONFIRM_INPUT_CONNECTION);
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::OnConfirmInputConnection(CSecID assignedID,
												 const CString& sShmName,
												 const CString& sVersion,
												 DWORD dwOptions
#ifdef _UNICODE
												 , WORD wCodePage
#endif
												 )
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::DoRequestOutputConnection(const CConnectionRecord &c)
{
	CIPCCmdRecord *pCmd;
	pCmd = c.CreateCmdRecord(this,SRV_CONTROL_REQUEST_OUTPUT_CONNECTION);
	WriteCmdWithExtraMemory(pCmd->m_pExtraMemory,
							pCmd->m_dwCmd,
							pCmd->m_dwParam1,
							pCmd->m_dwParam2,
							pCmd->m_dwParam3,
							pCmd->m_dwParam4);
	WK_DELETE(pCmd);

}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::DoConfirmOutputConnection(CSecID assignedID,
												  LPCTSTR shmName,
											      const CString& sVersion,
												  DWORD dwOptions
#ifdef _UNICODE
													, DWORD dwCodePage
#endif
													)
{
	CString sSharedMemoryName(shmName);
	CString slVersion = sVersion;

	if (slVersion.IsEmpty())
	{
		CString sExe;
		GetModuleFileName(AfxGetApp()->m_hInstance,sExe.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
		sExe.ReleaseBuffer();
		slVersion = WK_GetFileVersion(sExe);
	}

	CStringArray sa;
	
	sa.Add(sSharedMemoryName);
	sa.Add(slVersion);

	CIPCExtraMemory *pBubble = new CIPCExtraMemory();
	if (pBubble->Create(this,sa))
	{
		WriteCmdWithExtraMemory(pBubble,
								SRV_CONTROL_CONFIRM_OUTPUT_CONNECTION,
								assignedID.GetID(),								// param1
								dwOptions											// param2
#ifdef _UNICODE
								, MAKELONG(LOWORD(dwCodePage), 0)					// param3
#endif
								);
	}
	else
	{
		OnCommandSendError(SRV_CONTROL_CONFIRM_OUTPUT_CONNECTION);
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::OnRequestOutputConnection(const CConnectionRecord &c)
{
	OverrideError();
}
/*
Function: Answer on DoRequestOutputConnection.
The returned shmName can be used to open an output connection
to the server or link unit.
*/
void CIPCServerControl::OnConfirmOutputConnection(CSecID assignedID,
												  const CString& shmName,
												  const CString& sVersion,
												  DWORD dwOptions
#ifdef _UNICODE
												  , WORD wCodePage
#endif
												  )
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::DoRequestDataBaseConnection(const CConnectionRecord &c)
{
	CIPCCmdRecord *pCmd;
	pCmd = c.CreateCmdRecord(this,SRV_CONTROL_REQUEST_DB_CONNECTION);
	WriteCmdWithExtraMemory(pCmd->m_pExtraMemory,
							pCmd->m_dwCmd,
							pCmd->m_dwParam1,
							pCmd->m_dwParam2,
							pCmd->m_dwParam3,
							pCmd->m_dwParam4);
	WK_DELETE(pCmd);
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::DoConfirmDataBaseConnection(CSecID assignedID,
													LPCTSTR shmName,
													const CString& sVersion,
													DWORD dwOptions
#ifdef _UNICODE
													, DWORD dwCodePage
#endif
													)
{
	CString sSharedMemoryName(shmName);
	CString slVersion = sVersion;
	if (slVersion.IsEmpty())
	{
		CString sExe;
		GetModuleFileName(AfxGetApp()->m_hInstance,sExe.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
		sExe.ReleaseBuffer();
		slVersion = WK_GetFileVersion(sExe);
	}

	CStringArray sa;
	
	sa.Add(sSharedMemoryName);
	sa.Add(slVersion);

	CIPCExtraMemory *pBubble = new CIPCExtraMemory();
	if (pBubble->Create(this,sa))
	{
		WriteCmdWithExtraMemory(pBubble,
								SRV_CONTROL_CONFIRM_DB_CONNECTION,
								assignedID.GetID(),								// param1
								dwOptions											// param2
#ifdef _UNICODE
								, MAKELONG(LOWORD(dwCodePage), 0)					// param3
#endif
								);
	}
	else
	{
		OnCommandSendError(SRV_CONTROL_CONFIRM_DB_CONNECTION);
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::OnRequestDataBaseConnection(const CConnectionRecord &c)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::OnConfirmDataBaseConnection(CSecID assignedID,
													const CString& shmName,
													const CString& sVersion,
													DWORD dwOptions
#ifdef _UNICODE
													, WORD wCodePage
#endif
													)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::DoRequestServerReset(const CConnectionRecord &c)
{
	CIPCCmdRecord *pCmd;
	pCmd = c.CreateCmdRecord(this,SRV_CONTROL_REQUEST_SERVER_RESET);
	WriteCmdWithExtraMemory(pCmd->m_pExtraMemory,
							pCmd->m_dwCmd,
							pCmd->m_dwParam1,
							pCmd->m_dwParam2,
							pCmd->m_dwParam3,
							pCmd->m_dwParam4);
	WK_DELETE(pCmd);
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::OnRequestServerReset(const CConnectionRecord &c)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::DoConfirmServerReset()
{
	WriteCmd(SRV_CONTROL_CONFIRM_SERVER_RESET);
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::OnConfirmServerReset()
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::DoRequestAlarmConnection(const CConnectionRecord &c)
{
	CIPCCmdRecord *pCmd;
	pCmd = c.CreateCmdRecord(this,SRV_CONTROL_REQUEST_ALARM_CONNECTION);
	WriteCmdWithExtraMemory(pCmd->m_pExtraMemory,
							pCmd->m_dwCmd,
							pCmd->m_dwParam1,
							pCmd->m_dwParam2,
							pCmd->m_dwParam3,
							pCmd->m_dwParam4);
	WK_DELETE(pCmd);
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::OnRequestAlarmConnection(const CConnectionRecord &c)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::DoConfirmAlarmConnection(const CString &shmNameInput,
												 const CString &shmNameOutput,
												 const CString &sBitrate,
												 const CString &sVersion,
												 DWORD dwOptions
#ifdef _UNICODE
													, DWORD dwCodePage
#endif
												 )
{
	// NOT YET
	CStringArray names;
	names.Add(shmNameInput);
	names.Add(shmNameOutput);
	names.Add(sBitrate);
	if (!sVersion.IsEmpty())
	{
		names.Add(sVersion);
	}
	else
	{
		CString sExe;
		GetModuleFileName(AfxGetApp()->m_hInstance,sExe.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
		sExe.ReleaseBuffer();
		names.Add(WK_GetFileVersion(sExe));
	}
	CIPCExtraMemory *pMem = new CIPCExtraMemory();
	
	if (pMem->Create(this,names))
	{
		WriteCmdWithExtraMemory(pMem,SRV_CONTROL_CONFIRM_ALARM_CONNECTION,
			dwOptions											// param2
#ifdef _UNICODE
			, 0													// param2
			, MAKELONG(LOWORD(dwCodePage), 0)					// param3
#endif
			);
	}
	else
	{
		WK_DELETE(pMem);
		WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in DoConfirmAlarmConnection\n")); 
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::OnConfirmAlarmConnection(const CString &shmNameInput,
												 const CString &shmNameOutput,
												 const CString &sBitrate,
												 const CString &sVersion,
												 DWORD dwOptions
#ifdef _UNICODE
													, WORD wCodePage
#endif
													)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::DoRequestDCConnection(const CConnectionRecord &c)
{
	CIPCCmdRecord *pCmd;
	pCmd = c.CreateCmdRecord(this,SRV_CONTROL_REQUEST_DC_CONNECTION);
	WriteCmdWithExtraMemory(pCmd->m_pExtraMemory,
							pCmd->m_dwCmd,
							pCmd->m_dwParam1,
							pCmd->m_dwParam2,
							pCmd->m_dwParam3,
							pCmd->m_dwParam4);
	WK_DELETE(pCmd);
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::OnRequestDCConnection(const CConnectionRecord &c)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::DoConfirmDCConnection(const CString &shmNameDC,
											   const CString &sVersion,
											   DWORD dwOptions
#ifdef _UNICODE
													, DWORD dwCodePage
#endif
												)
{
	CStringArray sa;
	
	sa.Add(shmNameDC);
	sa.Add(sVersion);

	CIPCExtraMemory *pBubble = new CIPCExtraMemory();
	if (pBubble->Create(this,sa))
	{
		WriteCmdWithExtraMemory(pBubble,
								SRV_CONTROL_CONFIRM_DC_CONNECTION,
								dwOptions									// param1
#ifdef _UNICODE
								, 0											// param2
								, MAKELONG(LOWORD(dwCodePage), 0)			// param3
#endif
								);
	}
	else
	{
		OnCommandSendError(SRV_CONTROL_CONFIRM_DC_CONNECTION);
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::OnConfirmDCConnection(const CString &shmNameDC,
							 				   const CString &sVersion,
											   DWORD dwOptions
#ifdef _UNICODE
											   , WORD wCodePage
#endif
											   )
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::DoRequestAudioConnection(const CConnectionRecord &c)
{
	CIPCCmdRecord *pCmd;
	pCmd = c.CreateCmdRecord(this,SRV_CONTROL_REQUEST_AUDIO_CONNECTION);
	WriteCmdWithExtraMemory(pCmd->m_pExtraMemory,
							pCmd->m_dwCmd,
							pCmd->m_dwParam1,
							pCmd->m_dwParam2,
							pCmd->m_dwParam3,
							pCmd->m_dwParam4);
	WK_DELETE(pCmd);

}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::DoConfirmAudioConnection(CSecID assignedID,
												  LPCTSTR shmName,
											      const CString& sVersion,
												  DWORD dwOptions
#ifdef _UNICODE
													, DWORD dwCodePage
#endif
													)
{
	CString sSharedMemoryName(shmName);
	CString slVersion = sVersion;

	if (slVersion.IsEmpty())
	{
		CString sExe;
		GetModuleFileName(AfxGetApp()->m_hInstance,sExe.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
		sExe.ReleaseBuffer();
		slVersion = WK_GetFileVersion(sExe);
	}

	CStringArray sa;
	
	sa.Add(sSharedMemoryName);
	sa.Add(slVersion);

	CIPCExtraMemory *pBubble = new CIPCExtraMemory();
	if (pBubble->Create(this,sa))
	{
		WriteCmdWithExtraMemory(pBubble,
								SRV_CONTROL_CONFIRM_AUDIO_CONNECTION,
								assignedID.GetID(),								// param1
								dwOptions											// param2
#ifdef _UNICODE
								, MAKELONG(LOWORD(dwCodePage), 0)					// param3
#endif
								);
	}
	else
	{
		OnCommandSendError(SRV_CONTROL_CONFIRM_AUDIO_CONNECTION);
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCServerControl::OnRequestAudioConnection(const CConnectionRecord &c)
{
	OverrideError();
}
/*
Function: Answer on DoRequestAudioConnection.
The returned shmName can be used to open an output connection
to the server or link unit.
*/
void CIPCServerControl::OnConfirmAudioConnection(CSecID assignedID,
												  const CString& shmName,
												  const CString& sVersion,
												  DWORD dwOptions
#ifdef _UNICODE
												  , WORD wCodePage
#endif
												  )
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
BOOL CIPCServerControl::HandleCmd(DWORD dwCmd,
						DWORD dwParam1, DWORD dwParam2,
						DWORD dwParam3, DWORD dwParam4,
						const CIPCExtraMemory *pExtraMem
						)
{
	BOOL isOkay=TRUE;
	switch (dwCmd)
	{
		case SRV_CONTROL_REQUEST_INPUT_CONNECTION:
			// pData can be NULL, for local server
			// NOT YET check remote
			if (pExtraMem) 
			{
				CConnectionRecord c(*GetActiveCmdRecord());
				OnRequestInputConnection(c);
			} 
			else 
			{
				WK_TRACE(_T("No connection record\n"));
			}
			break;
		case SRV_CONTROL_CONFIRM_INPUT_CONNECTION:
			if (pExtraMem) 
			{
				CStringArray* pSA = pExtraMem->GetStrings();
				CString sShmName,sVersion;
				sShmName = pSA->GetAt(0);
				sVersion = pSA->GetAt(1);
				OnConfirmInputConnection(CSecID(dwParam1),
										 sShmName,
										 sVersion,
										 dwParam2
#ifdef _UNICODE
										, LOWORD(dwParam3)
#endif
				);
				WK_DELETE(pSA);
			} 
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
//////////////////////////////////////////////////////////////////////////
		case SRV_CONTROL_REQUEST_OUTPUT_CONNECTION:
			if (pExtraMem) 
			{
				CConnectionRecord c(*GetActiveCmdRecord());
				OnRequestOutputConnection(c);
			}
			else 
			{
				WK_TRACE(_T("No connection record\n"));
			}
			break;
		case SRV_CONTROL_CONFIRM_OUTPUT_CONNECTION:
			if (pExtraMem) 
			{
				CStringArray* pSA = pExtraMem->GetStrings();
				CString sShmName,sVersion;
				sShmName = pSA->GetAt(0);
				sVersion = pSA->GetAt(1);
				OnConfirmOutputConnection(CSecID(dwParam1),
										  sShmName,
                                          sVersion,
										  dwParam2
#ifdef _UNICODE
										, LOWORD(dwParam3)
#endif
					);
				WK_DELETE(pSA);
			} 
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
//////////////////////////////////////////////////////////////////////////
		case SRV_CONTROL_REQUEST_DB_CONNECTION:
			// pData can be NULL, for local server
			// NOT YET check remote
			if (pExtraMem) 
			{
				CConnectionRecord c(*GetActiveCmdRecord());
				OnRequestDataBaseConnection(c);
			}
			else
			{
				WK_TRACE(_T("No connection record\n"));
			}
			break;
		case SRV_CONTROL_CONFIRM_DB_CONNECTION:
			if (pExtraMem) 
			{
				CStringArray* pSA = pExtraMem->GetStrings();
				CString sShmName,sVersion;
				sShmName = pSA->GetAt(0);
				sVersion = pSA->GetAt(1);
				OnConfirmDataBaseConnection(CSecID(dwParam1),
											sShmName,
											sVersion,
											dwParam2
#ifdef _UNICODE
										  , LOWORD(dwParam3)
#endif
											);
				WK_DELETE(pSA);
			} 
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
//////////////////////////////////////////////////////////////////////////
		case SRV_CONTROL_REQUEST_DC_CONNECTION:
			// pData can be NULL, for local server
			// NOT YET check remote
			if (pExtraMem) 
			{
				CConnectionRecord c(*GetActiveCmdRecord());
				OnRequestDCConnection(c);
			} 
			else 
			{
				WK_TRACE(_T("No connection record\n"));
			}
			break;
		case SRV_CONTROL_CONFIRM_DC_CONNECTION:
			if (pExtraMem) 
			{
				CStringArray* pSA = pExtraMem->GetStrings();
				CString sShmName,sVersion;
				sShmName = pSA->GetAt(0);
				sVersion = pSA->GetAt(1);
				OnConfirmDCConnection(sShmName,
									  sVersion,
									  dwParam1
#ifdef _UNICODE
									  , LOWORD(dwParam3)
#endif
									  );
				WK_DELETE(pSA);
			} 
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
//////////////////////////////////////////////////////////////////////////
		case SRV_CONTROL_REQUEST_SERVER_RESET:
			if (pExtraMem) 
			{
				CConnectionRecord c(*GetActiveCmdRecord());
				OnRequestServerReset(c);
			}
			else 
			{
				WK_TRACE(_T("No connection record\n"));
			}
			break;
		case SRV_CONTROL_CONFIRM_SERVER_RESET:
			OnConfirmServerReset();
			break;
//////////////////////////////////////////////////////////////////////////
		case SRV_CONTROL_REQUEST_ALARM_CONNECTION:
			if (pExtraMem) 
			{
				const CIPCCmdRecord*pActiveCmdRec = GetActiveCmdRecord();
				CConnectionRecord c(*pActiveCmdRec);
				OnRequestAlarmConnection(c);
			}
			break;
		case SRV_CONTROL_CONFIRM_ALARM_CONNECTION:
			{
				if (pExtraMem)
				{
					CStringArray *pNames = pExtraMem->GetStrings();
					CString sInput=pNames->GetAt(0);
					CString sOutput=pNames->GetAt(1);
					CString sBitrate;
					CString sVersion;
					if (pNames->GetSize()>3)
					{
						sBitrate = pNames->GetAt(2);
						sVersion = pNames->GetAt(3);
					}
					OnConfirmAlarmConnection(sInput,
											 sOutput,
											 sBitrate,
											 sVersion,
											 dwParam1
#ifdef _UNICODE
											 , LOWORD(dwParam3)
#endif
											 );
					WK_DELETE(pNames);
				} else {
					// NOT YET
				}
			}
			break;
//////////////////////////////////////////////////////////////////////////
		case SRV_CONTROL_REQUEST_AUDIO_CONNECTION:
			if (pExtraMem) 
			{
				CConnectionRecord c(*GetActiveCmdRecord());
				OnRequestAudioConnection(c);
			}
			else 
			{
				WK_TRACE(_T("No connection record\n"));
			}
			break;
		case SRV_CONTROL_CONFIRM_AUDIO_CONNECTION:
			if (pExtraMem) 
			{
				CStringArray* pSA = pExtraMem->GetStrings();
				CString sShmName,sVersion;
				sShmName = pSA->GetAt(0);
				sVersion = pSA->GetAt(1);
				OnConfirmAudioConnection(CSecID(dwParam1),
										 sShmName,
										 sVersion,
										 dwParam2
#ifdef _UNICODE
										 , LOWORD(dwParam3)
#endif
										 );
				WK_DELETE(pSA);
			} 
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
#include "UpdateHandle.h"
		default:

			// call superclass
			isOkay=CIPC::HandleCmd(dwCmd,
						dwParam1,dwParam2,dwParam3,dwParam4,
						pExtraMem);
			if (isOkay==FALSE) {
				TRACE(_T("CIPCServerControl unhandled cmd\n"));
			}
	} // end of switch
	return isOkay;
}
#define CLASSX CIPCServerControl
#include "UpdateFns.h"
