/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcInput.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CipcInput.cpp $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 121 $
// LAST CHANGE:	$Modtime: 19.01.06 20:30 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdcipc.h"

#include "CipcInput.h"
#include "CipcExtraMemory.h"
#include "CipcInputRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 

/*
 CIPCInput Overview |
<i>In this context input is used for alarms.<ni>
CIPCInput is derived from <c CIPC>. 
Units, which have alarms (alarm hardware), derive their own
specialized subclasses from CIPCInput. 

The maximum number of inputs in one CIPCInput instance (input group
on the server side) is 32. This limitation is induced by the usage
of DWORD for bitmasks to represent the overall state.
If there is need for more than 32 inputs in one unit,
it is possible to instantiate more than one instance of the same class.

CIPCInput provides virtual functions for both sides of the 
communication. If a units will perform any input operation , it should
overwrite the OnRequestXXX and OnResponseXXX functions.
*/
///////////////////////////////////////////////////////////////////////////
/* 
Function: constructor 
param: const char * | shmName | The systemwide unique shared memory name
for the CIPC object. The other process must use the same name.
param: BOOL | asMaster | The process which creates it's object first is master, so
asMaster must be TRUE, the second process must use FALSE. The server process
uses most times the asMaster flag as TRUE.
*/
CIPCInput::CIPCInput(LPCTSTR shmName, BOOL asMaster)
	: CIPC(shmName, asMaster)
{
}
///////////////////////////////////////////////////////////////////////////
/*Function: NOT YET DOCUMENTED*/
CIPCInput::~CIPCInput()
{
	StopThread();
}
/*********************************************************************************************
 Class      : CIPCInput
 Function   : OnRequestHardware
 Description: The unit should check it's hardware state, and return 0 for okay !=0 as an
              error code in the call DoConfirmHardware.

 Parameters:   
  wGroupID: (E): current group ID  (WORD)

 Result type: None (void)
 created: March, 03 2003
 <TITLE OnRequestHardware>
*********************************************************************************************/
void CIPCInput::OnRequestHardware(WORD wGroupID)
{
	OverrideError();
}
/*********************************************************************************************
 Class      : CIPCInput
 Function   : OnConfirmHardware
 Description: Called if a confirmation of DoConfirmHardware is received. The unit has
              answered its hardware state in iErrorCode:
               0, hardware okay
					!=0, unit specific error code

 Parameters:   
  wGroupID  : (E): current group ID  (WORD)
  iErrorCode: (E): unit specific error code  (int)

 Result type:  (void)
 created: March, 03 2003
 <TITLE OnConfirmHardware>
*********************************************************************************************/
void CIPCInput::OnConfirmHardware(WORD wGroupID, int iErrorCode)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Resets a unit, you should perform all status value resettings.
Has to be answered with <mf CIPCInput.DoConfirmReset>.
*/
void CIPCInput::OnRequestReset(WORD wGroupID)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*Function:
Called if a confirmation of <mf CIPCInput.DoRequestReset> is
received. The unit has performed a reset.
*/
void CIPCInput::OnConfirmReset(WORD wGroupID)
{
	OverrideError();
}
/*********************************************************************************************
 Class      : CIPCInput
 Function   : OnRequestSetEdge
 Description: Sets the edge of all inputs. Indicate alarm changes on: 
               1 = raising edge
               0 = falling edge
              Has to be answered with DoConfirmEdge.
 relative functions: OnRequestSetFree

 Parameters:   
  wGroupID: (E): current group ID  (WORD)
  edgeMask: (E): Bit mask for rising or falling edge  (DWORD)

 Result type:  (void)
 created: March, 03 2003
 <TITLE OnRequestSetEdge>
*********************************************************************************************/
void CIPCInput::OnRequestSetEdge(WORD wGroupID, DWORD edgeMask)
{
	OverrideError();
}

/*********************************************************************************************
 Class      : CIPCInput
 Function   : OnRequestSetFree
 Description: Called to enable/disable inputs. An input is:
               enabled, if the bit in openMask is set
               disabled, if the bit in openMask is clear
              A disabled input should not send alarm indications with DoIndicateAlarmState!
              relative functions: OnRequestSetEdge, DoIndicateAlarmState

 Parameters:   
  wGroupID: (E): current group ID  (WORD)
  openMask: (E): State mask to enable or disable an input  (DWORD)

 Result type:  (void)
 created: March, 03 2003
 <TITLE OnRequestSetFree>
*********************************************************************************************/
void CIPCInput::OnRequestSetFree(WORD wGroupID, DWORD openMask)
{
	OverrideError();
}
/*********************************************************************************************
 Class      : CIPCInput
 Function   : OnRequestAlarmState
 Description: Called if a RequestAlarmState cmd is received. As answer the bitmask of the
              current alarm states has to be send with DoConfirmAlarmState.
				  Don't forget to mask the state with the current free-mask.
 Parameters:   
  wGroupID: (E): Current group ID  (WORD)

 Result type: None (void)
 created: March, 03 2003
 <TITLE OnRequestAlarmState>
*********************************************************************************************/
void CIPCInput::OnRequestAlarmState(WORD wGroupID)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnResponseAlarmState(WORD wGroupID)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
//
/*
Function:
Called if a confirmation of <mf CIPCInput.DoRequestSetEdge> is
received. The unit has set the according edges. 1 positive, 0 negative
*/
void CIPCInput::OnConfirmEdge(WORD wGroupID, DWORD edgeBitmask)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Called if a confirmation of <mf CIPCInput.DoRequestSetFree> is
received. The unit has enabled/disabled inputs.
*/
void CIPCInput::OnConfirmFree(WORD wGroupID, DWORD openBitmask)	// 1 open, 0 closed
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
// 1 high, 0 low; 1 changed, 0 unchanged
/*
Function:
Called if a confirmation of <mf CIPCInput.DoRequestAlarmState> is
received. NOT YET documented.
*/
void CIPCInput::OnConfirmAlarmState(WORD wGroupID, DWORD inputMask)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Called if a unit indicates changes of alarm state[s] via
<mf CIPCInput.DoIndicateAlarmState>.
NOT YET args.
Has to be answered with <mf CIPCInput.DoResponseAlarmState>.
*/
void CIPCInput::OnIndicateAlarmState(WORD wGroupID, 
									 DWORD inputMask, DWORD changeMask,
									 LPCTSTR szInfoString
									 )	// 1 high, 0 low; 1 changed, 0 unchanged
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnIndicateAlarm(CSecID id,BOOL bAlarm,int iNumRecords,
								const CIPCField fields[])
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnRequestInfoInputs(WORD wGroupID)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Called if an answer to <mf CIPCInput.DoRequestInfoInputs> is received.
param: const CIPCInputRecord records[] | records | hold data about the inputs, see xref CIPCInputRecord for details.
*/
void CIPCInput::OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, 
									int numRecords, const CIPCInputRecord records[])
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Sends a Reset cmd, which causes a unit to reset the underlying hardware.
<mf CIPCInput.OnConfirmReset>
*/
void CIPCInput::DoRequestReset(WORD wGroupID)
{
	WriteCmd(INP_REQ_RESET,wGroupID);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Sends a SetEdge cmd, which defines when the the unit should
indicate alarm changes <mf CIPCInput.DoIndicateAlarmState>.
<nl>
The bitmask dwEdgeMask defines:
<b> negative edge <nb>, if a bit is clear
<b> positive edge <nb>, if a bit is set
 <mf CIPCInput.OnConfirmEdge>
*/
void CIPCInput::DoRequestSetEdge(WORD wGroupID, DWORD edgeMask)	
{
	WriteCmd(INP_REQ_SETEDGE,wGroupID,edgeMask);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Sends a SetFree cmd, which enables/disables inputs. The bitmask
dwOpenMask} defines:
<b> enabled <nb>, if the bit in openMask is set
<b> disabled<nb>, if the bit in openMask is clear

 <mf CIPCInput.OnConfirmFree>
*/
void CIPCInput::DoRequestSetFree(WORD wGroupID, DWORD openMask)
{
	WriteCmd(INP_REQ_SETFREE,wGroupID,openMask);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoRequestAlarmState(WORD wGroupID)
{
	WriteCmd(INP_REQ_AL_STATE,wGroupID);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Sends a RequestHardware cmd, which asks a unit for the state of
its hardware.
 <mf CIPCInput.OnConfirmHardware>
*/
void CIPCInput::DoRequestHardware(WORD wGroupID)
{
	WriteCmd(INP_REQ_HARDWARE,wGroupID);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Sends a RequestInfoInputs cmd, which asks the server for details
about an input group.If SEC_ID_NOID is given the server sends
info about all input groups.
param: WORD | wGroupID | id of an input group, or SECID_NO_ID for all
 <mf CIPCInput.OnConfirmInfoInputs>
*/
void CIPCInput::DoRequestInfoInputs(WORD wGroupID)
{
	WriteCmd(INP_REQ_INFO,wGroupID);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Answer to <mf CIPCInput.DoRequestInfoInputs>, used by the server.
*/
void CIPCInput::DoConfirmInfoInputs(WORD wGroupID, int iNumGroups, 
										int iNumRecords, 
										const CIPCInputRecord records[])
{
	if (iNumRecords && records) 
	{
		// calc size for memory bubble. Each record has
		// wLEN:dwID:bFLAGS:TEXT
		DWORD dwMemoryLen=0;
		int i;
#ifdef _UNICODE
		WORD *pwLen = (WORD*) _alloca(sizeof(WORD)*iNumRecords);
		BOOL bUnicode = m_wCodePage == CODEPAGE_UNICODE;
#endif
		for (i=0;i<iNumRecords;i++) 
		{
			// text + flags + id + wRecLen
#ifdef _UNICODE
			CWK_String str = records[i].GetName();
			pwLen[i] = (WORD)str.CopyToMemory(bUnicode, NULL, -1, m_wCodePage);
			WORD wRecLen = (WORD)(pwLen[i] + 1 + 4 +2);
#else
			WORD wRecLen = (WORD)(records[i].GetName().GetLength() + 1 + 4 +2);
#endif
			dwMemoryLen += wRecLen;
		}
		CIPCExtraMemory *pMem = new CIPCExtraMemory();
		if (pMem->Create(this, dwMemoryLen))
		{
			BYTE *pPtr = (BYTE *)pMem->GetAddressForWrite();
			// ugly byte filling to avoid struct alignment flags
			for (i=0;i<iNumRecords;i++) 
			{
#ifdef _UNICODE
				CWK_String str = records[i].GetName();
				
				WORD_TO_MEMORY(pPtr, (WORD)(pwLen[i] + 1 + 4 +2));
				// id 4 bytes
				DWORD_TO_MEMORY(pPtr, records[i].GetID().GetID());
				// flags 1 bytes
				*pPtr++ = (BYTE) records[i].GetFlags();
				pPtr += str.CopyToMemory(bUnicode, pPtr, -1, m_wCodePage);
#else
				WORD wRecLen = (WORD)(records[i].GetName().GetLength() + 1 + 4 +2);
				*pPtr++ = HIBYTE(wRecLen);
				*pPtr++ = LOBYTE(wRecLen);
				// id 4 bytes
				DWORD id = records[i].GetID().GetID();
				*pPtr++ = HIBYTE(HIWORD(id));
				*pPtr++ = LOBYTE(HIWORD(id));
				*pPtr++ = HIBYTE(LOWORD(id));
				*pPtr++ = LOBYTE(LOWORD(id));
				// flags 1 bytes
				*pPtr++ = (BYTE) records[i].GetFlags();
				int len = records[i].GetName().GetLength();
				for (int j=0;j<len;j++) 
				{
					*pPtr++ = records[i].GetName()[j];
				}
#endif
			}
			WriteCmdWithExtraMemory( pMem, INP_CONF_INFO, 
										wGroupID, iNumGroups, iNumRecords,0);
		}
		else
		{
			WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in DoConfirmInfoInputs\n"));
		}
		WK_DELETE(pMem);
	} 
	else 
	{
		// no records or NULL record
		WriteCmd( INP_CONF_INFO, wGroupID, iNumGroups, iNumRecords,0);
	}
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
NOT YET documented
*/
void CIPCInput::DoResponseAlarmState(WORD wGroupID)
{
	WriteCmd(INP_RESP_AL_STATE,wGroupID);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Used to answer in <mf CIPCInput.OnRequestReset>.
*/
void CIPCInput::DoConfirmReset(WORD wGroupID)
{
	WriteCmd(INP_CONF_RESET,wGroupID);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Used to answer in <mf CIPCInput.OnRequestAlarmState>.
*/
void CIPCInput::DoConfirmAlarmState(WORD wGroupID, DWORD inputMask)
{
	WriteCmd(INP_CONF_AL_STATE,wGroupID, inputMask);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Used to answer in <mf CIPCInput.OnRequestSetEdge>.
*/
void CIPCInput::DoConfirmEdge(WORD wGroupID, DWORD edgeBitmask)
{
	WriteCmd(INP_CONF_EDGE,wGroupID, edgeBitmask);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Used to answer in <mf CIPCInput.OnRequestSetFree>.
*/
void CIPCInput::DoConfirmFree(WORD wGroupID, DWORD openBitmask)
{
	WriteCmd(INP_CONF_FREE,wGroupID, openBitmask);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
Used to answer in <mf CIPCInput.OnRequestHardware>.
*/
void CIPCInput::DoConfirmHardware(WORD wGroupID, int iErrorcode)
{
	WriteCmd(INP_CONF_HARDWARE,wGroupID, (DWORD) iErrorcode);
}
///////////////////////////////////////////////////////////////////////////
/*Function:
Used to indicate an alarm.
param: WORD | wGroupID | the group identifier
param: DWORD | inputMask| bit set if alarm is active
param: DWORD | changeMask | bit set, if alarm has changed
param: LPCTSTR | szInfoString | additional info wrapped into a string. 
NULL if the is no info.
*/
void CIPCInput::DoIndicateAlarmState(WORD wGroupID, DWORD inputMask, 
									 DWORD changeMask,
									 LPCTSTR szInfoString)
{
	if (szInfoString && _tcslen(szInfoString)) 
	{
		WriteCmdWithData(_tcslen(szInfoString),
						 szInfoString,
						 INP_IND_AL_STATE, 
						 wGroupID, 
						 inputMask, 
						 changeMask);
	} 
	else 
	{
		WriteCmd(INP_IND_AL_STATE, wGroupID, inputMask, changeMask); 
	}
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoIndicateAlarm(CSecID id, BOOL bAlarm, const CIPCFields& fields)
{
	if (fields.GetSize()>=0) 
	{
		CIPCExtraMemory* pBubble = fields.BubbleFromFields(this);
		if (pBubble)
		{
			WriteCmdWithExtraMemory(pBubble,INP_INDICATE_ALARM_FIELDS,id.GetID(),fields.GetSize(),bAlarm);
		}
		else
		{
			OnCommandSendError(INP_INDICATE_ALARM_FIELDS);
		}

	} 
	else 
	{
		WriteCmd(INP_INDICATE_ALARM_FIELDS, id.GetID(),0,bAlarm); 
	}
}

static inline WORD GroupIDFromParam(DWORD dwParam)
{
	return (WORD)dwParam;
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
BOOL CIPCInput::HandleCmd(DWORD dwCmd,
						DWORD dwParam1, DWORD dwParam2,
						DWORD dwParam3, DWORD dwParam4,
						const CIPCExtraMemory *pExtraMem
						)
{
	BOOL isOkay=TRUE;
	
	switch (dwCmd) 	
	{
		case INP_REQ_INFO:
			OnRequestInfoInputs((WORD)dwParam1);
			break;
		case INP_CONF_INFO:
			{
				WORD wGroupID = (WORD)dwParam1;
				int iNumGroups = dwParam2;
				int iNumRecords = dwParam3;
				if (pExtraMem && iNumRecords) 
				{
					CIPCInputRecord *records = new CIPCInputRecord[iNumRecords];
					const BYTE *pPtr = (const BYTE *) pExtraMem->GetAddress();
					for (int i=0;i<iNumRecords;i++) 
					{
						// UNUSED CIPCInputRecord & rec = records[i];
						// reverse the sending
						WORD wRecLen= (WORD)(((WORD)pPtr[0]<<8) | (WORD)pPtr[1]);
						pPtr += 2;
						CSecID tmpID( 
							  (DWORD)pPtr[0]<<24 
							| (DWORD)pPtr[1]<<16
							| (DWORD)pPtr[2]<<8
							| (DWORD)pPtr[3]
							);
						pPtr += 4;
						// flag
						BYTE tmpFlags = *pPtr++;
						// rest is text
						if (wRecLen>=7)
						{
#ifdef _UNICODE
							CWK_String str;
							str.InitFromMemory(m_wCodePage==CODEPAGE_UNICODE, pPtr, wRecLen-7, m_wCodePage);
							pPtr += (wRecLen-7);
							records[i].Set(str,tmpID,tmpFlags);
#else
							BYTE *pTmpBuffer = new BYTE[wRecLen-7+1];
							BYTE *pTmp = pTmpBuffer;	//  loop pointer
							for (int j=7;j<wRecLen;j++) 
							{
								*pTmp++ = *pPtr++;
							}
							*pTmp = 0;	// terminate string
							records[i].Set(pTmpBuffer,tmpID,tmpFlags);
							WK_DELETE_ARRAY(pTmpBuffer);
#endif
						} 
						else 
						{
							WK_TRACE_ERROR(_T("CIPCInput wrong len %d <= 7\n"),wRecLen);
						}
					}
					OnConfirmInfoInputs(wGroupID,iNumGroups, iNumRecords,records);
					WK_DELETE_ARRAY(records);
				} 
				else 
				{
					// no bubble or num==0
					CIPCInputRecord dummyRecords[1]; 
					OnConfirmInfoInputs(wGroupID,iNumGroups,iNumRecords,dummyRecords);
				}
			}
			break;
		case INP_REQ_RESET:			
			OnRequestReset(GroupIDFromParam(dwParam1)); 
			break;
		case INP_REQ_SETEDGE :		
			OnRequestSetEdge(GroupIDFromParam(dwParam1), dwParam2); 
			break;
		case INP_REQ_SETFREE:		
			OnRequestSetFree(GroupIDFromParam(dwParam1), dwParam2); 
			break;
		// OLD case INP_REQ_CUR_STATE: OnRequestCurrentState(dwParam1); break;
		case INP_REQ_AL_STATE :		
			OnRequestAlarmState(GroupIDFromParam(dwParam1)); 
			break;
		case INP_REQ_HARDWARE:		
			OnRequestHardware(GroupIDFromParam(dwParam1)); 
			break;

		case INP_RESP_AL_STATE:		
			OnResponseAlarmState(GroupIDFromParam(dwParam1)); 
			break;
		//
		case INP_CONF_HARDWARE:
			OnConfirmHardware(GroupIDFromParam(dwParam1),dwParam2);
			break;
		case INP_CONF_EDGE:
			OnConfirmEdge(GroupIDFromParam(dwParam1),dwParam2);
			break;
		case INP_CONF_FREE:
			OnConfirmFree(GroupIDFromParam(dwParam1),dwParam2);
			break;
		case INP_CONF_RESET:
			OnConfirmReset(GroupIDFromParam(dwParam1));
			break;			
		case INP_CONF_AL_STATE:
			OnConfirmAlarmState(GroupIDFromParam(dwParam1),dwParam2);
			break;
		case INP_IND_AL_STATE:
			{
				CWK_String strInfo;
				if (pExtraMem) 
				{
					int nlen = pExtraMem->GetLength();
#ifdef _UNICODE
					strInfo.InitFromMemory(m_wCodePage==CODEPAGE_UNICODE, pExtraMem->GetAddress(), nlen, m_wCodePage);
					TRACE(_T("INP_IND_AL_STATE:%s\n"), strInfo);
#else
					strInfo.InitFromMemory(pExtraMem->GetAddress(), nlen);
#endif
//					WK_TRACE(_T("INP_IND_AL_STATE (%s)\n"), pInfo);
				}
				OnIndicateAlarmState(GroupIDFromParam(dwParam1), dwParam2, dwParam3, strInfo);
			}
			break;
		case INP_INDICATE_ALARM_FIELDS:
			{
				CIPCField* pFields = NULL;
				int iNumRecords = dwParam2;
				if (pExtraMem)
				{
					pFields = CIPCFields::FieldsFromBubble(iNumRecords,pExtraMem);
				}
				OnIndicateAlarm(CSecID(dwParam1), dwParam3, iNumRecords,pFields);
			}
			break;
		case INP_INDICATE_ALARM_NR:
			{
				OnIndicateAlarmNr(CSecID(dwParam1),dwParam2,dwParam3,dwParam4);
			}
			break;
			///////////////////
		case INP_REQUEST_GET_FILE:
			if (pExtraMem) 
			{
				int iDestination = (int)dwParam1;
				CString s=pExtraMem->GetString();
				OnRequestGetFile(iDestination,s);
			} 
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
		case INP_CONFIRM_GET_FILE:
			if (pExtraMem) 
			{
				int iDestination  = (int)dwParam1;
				BYTE *pByte       = (BYTE*)pExtraMem->GetAddress();
				DWORD dwStringLen = *((DWORD *)pByte);
				CWK_String strInfo;
				pByte += sizeof(DWORD);
				CWK_String s;//=pTmp;
#ifdef _UNICODE
				s.InitFromMemory(m_wCodePage==CODEPAGE_UNICODE, pByte, dwStringLen, m_wCodePage);
				TRACE(_T("INP_CONFIRM_GET_FILE:%s\n"), s);
#else
				s.InitFromMemory(pByte, dwStringLen);
#endif
//				WK_TRACE(_T("INP_CONFIRM_GET_FILE (%s)\n"), s);
//				CopyMemory(pTmp,((const char *)pExtraMem->GetAddress())+sizeof(DWORD),dwStringLen);
//				pTmp[dwStringLen]=0;
				const void *pData=NULL;
				DWORD dwDataLen=pExtraMem->GetLength()-sizeof(DWORD)-dwStringLen;
				if (dwDataLen)
				{
					pData = ((BYTE*)pExtraMem->GetAddress())+sizeof(DWORD)+dwStringLen;
				}
				OnConfirmGetFile(iDestination,
					s,
					pData,
					dwDataLen,
					dwParam2);
//				WK_DELETE_ARRAY(pTmp);
			} 
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
		case INP_INDICATE_COMM_DATA:
			if (pExtraMem) 
			{
				CSecID id(dwParam1);
				DWORD dwBlockNr=dwParam2;
				OnIndicateCommData(id,*pExtraMem,dwBlockNr);
			}
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
		case INP_WRITE_COMM_DATA:
			if (pExtraMem) 
			{
				CSecID id(dwParam1);
				DWORD dwBlockNr=dwParam2;
				OnWriteCommData(id,*pExtraMem,dwBlockNr);
			}
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
		case INP_CONFIRM_WRITE_COMM_DATA:
			{
				CSecID id(dwParam1);
				DWORD dwBlockNr=dwParam2;
				OnConfirmWriteCommData(id,dwBlockNr);
			}
			break;
		case INP_REQUEST_CAMERA_CONTROL:
			{
				CSecID commID(dwParam1);
				CSecID camID(dwParam2);
				// NOT YET use switch
				CameraControlCmd cmd = (CameraControlCmd)dwParam3;
				DWORD dwValue = dwParam4;
				OnRequestCameraControl(commID,camID,cmd,dwValue);
			}
			break;
		case INP_IND_ALARM_FIELD_STATE:
			{
				int iNumFields = dwParam4;
				CIPCField *pData = NULL;
				if (pExtraMem) 
				{
					pData = CIPCFields::FieldsFromBubble(iNumFields,pExtraMem);
				}
				OnIndicateAlarmFieldsState(GroupIDFromParam(dwParam1),
										   dwParam2,
										   dwParam3,
										   iNumFields,
										   pData);
				WK_DELETE_ARRAY(pData);
			}
			break;
		case INP_IND_ALARM_FIELD_UPDATE:
			{
				CSecID inputID(dwParam1);
				int iNumFields = dwParam2;
				CIPCField *pData = CIPCFields::FieldsFromBubble(iNumFields,pExtraMem);
				OnIndicateAlarmFieldsUpdate(inputID,iNumFields,pData);
				WK_DELETE_ARRAY(pData);
			}
			break;

		case INP_REQUEST_GET_SYSTEM_TIME:
			OnRequestGetSystemTime();
			break;
		case INP_CONFIRM_GET_SYSTEM_TIME:
			{
				CSystemTime st;

				st.wYear = LOWORD(dwParam1);
				st.wMonth = HIWORD(dwParam1);

				st.wDayOfWeek = LOWORD(dwParam2);
				st.wDay = HIWORD(dwParam2);

				st.wHour = LOWORD(dwParam3);
				st.wMinute = HIWORD(dwParam3);

				st.wSecond = LOWORD(dwParam4);
				st.wMilliseconds = HIWORD(dwParam4);

				OnConfirmGetSystemTime(st);
			}
			break;
		case INP_REQUEST_SET_SYSTEM_TIME:
			{
				CSystemTime st;

				st.wYear = LOWORD(dwParam1);
				st.wMonth = HIWORD(dwParam1);

				st.wDayOfWeek = LOWORD(dwParam2);
				st.wDay = HIWORD(dwParam2);

				st.wHour = LOWORD(dwParam3);
				st.wMinute = HIWORD(dwParam3);

				st.wSecond = LOWORD(dwParam4);
				st.wMilliseconds = HIWORD(dwParam4);

				OnRequestSetSystemTime(st);
			}
			break;
		case INP_CONFIRM_SET_SYSTEM_TIME:
			OnConfirmSetSystemTime();
			break;
		case INP_CONFIRM_ALARM:
			OnConfirmAlarm(CSecID(dwParam1));
			break;
		case INP_REQUEST_GET_ALARM_OFF_SPANS:
			OnRequestGetAlarmOffSpans(CSecID(dwParam1));
			break;
		case INP_CONFIRM_GET_ALARM_OFF_SPANS:
			{
				int iNumRecords = (int)dwParam2;
				CSystemTimeSpan* pData = CSystemTimeSpans::SystemTimeSpansFromBubble(iNumRecords,pExtraMem);
				OnConfirmGetAlarmOffSpans(CSecID(dwParam1),iNumRecords,pData);
				WK_DELETE_ARRAY(pData);
			}
			break;
		case INP_REQUEST_SET_ALARM_OFF_SPANS:
			{
				int iNumRecords = (int)dwParam2;
				CSystemTimeSpan* pData = CSystemTimeSpans::SystemTimeSpansFromBubble(iNumRecords,pExtraMem);
				OnRequestSetAlarmOffSpans(CSecID(dwParam1),iNumRecords,pData);
				WK_DELETE_ARRAY(pData);
			}
			break;
		case INP_CONFIRM_SET_ALARM_OFF_SPANS:
			OnConfirmSetAlarmOffSpans(CSecID(dwParam1));
			break;
		case INP_REQUEST_GET_ZIME_ZONE_INFORMATION:
			OnRequestGetTimeZoneInformation();
			break;
		case INP_CONFIRM_GET_ZIME_ZONE_INFORMATION:
			if (pExtraMem)
			{
				CTimeZoneInformation tzi;
				tzi.FromBubble(pExtraMem);
				OnConfirmGetTimeZoneInformation(tzi);
			}
			else
			{
				OnMissingExtraMemory(INP_CONFIRM_GET_ZIME_ZONE_INFORMATION);
			}
			break;
		case INP_REQUEST_SET_ZIME_ZONE_INFORMATION:
			if (pExtraMem)
			{
				CTimeZoneInformation tzi;
				tzi.FromBubble(pExtraMem);
				OnRequestSetTimeZoneInformation(tzi);
			}
			else
			{
				OnMissingExtraMemory(INP_REQUEST_SET_ZIME_ZONE_INFORMATION);
			}
			break;
		case INP_CONFIRM_SET_ZIME_ZONE_INFORMATION:
			OnConfirmSetTimeZoneInformation();
			break;
		case INP_REQUEST_GET_ZIME_ZONE_INFORMATIONS:
			OnRequestGetTimeZoneInformations();
			break;
		case INP_CONFIRM_GET_ZIME_ZONE_INFORMATIONS:
			{
				CTimeZoneInformations tzis;
				tzis.FromBubble(dwParam1,pExtraMem);
				OnConfirmGetTimeZoneInformations(tzis);
			}
			break;

#include "UpdateHandle.h"
		default:
			// call superclass
			isOkay=CIPC::HandleCmd(dwCmd,dwParam1,dwParam2,dwParam3,dwParam4,pExtraMem);
			break;
	} // end of switch
	return isOkay;
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoRequestGetFile(int iDestination,const CString &sFileName)
{
#ifdef _UNICODE
	WriteCmdWithString(sFileName, INP_REQUEST_GET_FILE, iDestination);
#else
	CString sTmp(sFileName);
	WriteCmdWithData(sTmp.GetLength(), LPCTSTR(sTmp), INP_REQUEST_GET_FILE, iDestination);
#endif
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnRequestGetFile(int iDestination,const CString &sFileName)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoConfirmGetFile(int iDestination,
								 const CString &sFileName,
								 const void *pData,
								 DWORD dwDataLen, DWORD dwCodePage/*=0*/)
{
#ifdef _UNICODE
	CWK_String sFN(sFileName);
	CWK_String sData;
	BOOL bUnicode = m_wCodePage == CODEPAGE_UNICODE;
	DWORD dwLen = sFN.CopyToMemory(bUnicode, NULL, -1, m_wCodePage);
	if (dwDataLen & STRING_DATA_POINTER)
	{
		dwDataLen &= ~STRING_DATA_POINTER;
		sData.InitFromMemory(TRUE, pData, dwDataLen*sizeof(wchar_t));// Achtung Daten sind hier definitiv immer Unicode !!
		dwDataLen = sData.CopyToMemory(bUnicode, NULL, -1, m_wCodePage);
		pData = NULL;
	}
	BYTE *pAll  = new BYTE[dwLen+sizeof(DWORD)+dwDataLen];
	BYTE *pByte = pAll;
	DWORD *pDw = (DWORD *)pByte;
	pDw[0] = dwLen;
	pByte += sizeof(DWORD);
	pByte += sFN.CopyToMemory(bUnicode, pByte, -1, m_wCodePage);
	if (!sData.IsEmpty())
	{
		pByte += sData.CopyToMemory(bUnicode, pByte, -1, m_wCodePage);
	}
	else if (pByte && dwDataLen) 
	{
		CopyMemory(pByte, pData, dwDataLen);
	}
	
	DWORD dwAllLen = sizeof(DWORD) + dwLen + dwDataLen;
	WriteCmdWithData(dwAllLen, pAll, INP_CONFIRM_GET_FILE, iDestination, dwCodePage);
	WK_DELETE_ARRAY(pAll);
#else
	BYTE *pAll = new BYTE[sFileName.GetLength()+sizeof(DWORD)
							+dwDataLen];
	DWORD *pDw = (DWORD *)pAll;
	pDw[0]=sFileName.GetLength();
	CopyMemory(pAll+sizeof(DWORD), sFileName, sFileName.GetLength());
	if (pData && dwDataLen) 
	{
		CopyMemory(pAll+(sizeof(DWORD)+sFileName.GetLength()), pData, dwDataLen);
	}
	
	DWORD dwLen = sizeof(DWORD)+sFileName.GetLength()+dwDataLen;
	WriteCmdWithData(dwLen, pAll, INP_CONFIRM_GET_FILE, iDestination);
	WK_DELETE_ARRAY(pAll);
#endif
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnConfirmGetFile(int iDestination,
								 const CString &sFileName,
								 const void *pData,
								 DWORD dwDataLen, DWORD dwCodePage)
{
	OverrideError();
}

#define CLASSX CIPCInput
#include "UpdateFns.h"

//////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoIndicateCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr
									)
{
	CIPCExtraMemory *pTmp = new CIPCExtraMemory(data);
	WriteCmdWithExtraMemory(pTmp,INP_INDICATE_COMM_DATA, 
								id.GetID(), dwBlockNr);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnIndicateCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr
									)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoWriteCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr
									)
{
	CIPCExtraMemory *pTmp = new CIPCExtraMemory(data);
	WriteCmdWithExtraMemory(pTmp,INP_WRITE_COMM_DATA, id.GetID(), dwBlockNr);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnWriteCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr
									)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoConfirmWriteCommData(CSecID id, DWORD dwBlockNr)
{
	WriteCmd(INP_CONFIRM_WRITE_COMM_DATA, id.GetID(), dwBlockNr);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnConfirmWriteCommData(CSecID id, DWORD dwBlockNr)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoRequestCameraControl(CSecID commID, 
									   CSecID camID,
									   CameraControlCmd cmd,
									   DWORD dwValue)
{
	WriteCmd(INP_REQUEST_CAMERA_CONTROL,
				commID.GetID(), camID.GetID(),
				(DWORD)cmd,
				dwValue);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnRequestCameraControl(CSecID commID, 
									   CSecID camID,
									   CameraControlCmd cmd,
									   DWORD dwValue)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoIndicateAlarmFieldsUpdate(CSecID id, 
									   const CIPCFields& fields)
{
	CIPCExtraMemory *pFieldBubble = fields.BubbleFromFields(this);
	WriteCmdWithExtraMemory(pFieldBubble,INP_IND_ALARM_FIELD_UPDATE,
							id.GetID(),
							(DWORD)fields.GetSize());
}

///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnIndicateAlarmFieldsUpdate(CSecID id,
									   int iNumFields,
									   const CIPCField fields[])
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoIndicateAlarmFieldsState(WORD wGroupID,
									  DWORD inputMask,  // 1 high, 0 low
									  DWORD changeMask, // 1 changed, 0 unchanged
									  const CIPCFields& fields
									  )
{
	CIPCExtraMemory *pFieldBubble = fields.BubbleFromFields(this);
	WriteCmdWithExtraMemory(pFieldBubble,INP_IND_ALARM_FIELD_STATE,
							wGroupID, inputMask, changeMask,
							(DWORD)fields.GetSize());
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnIndicateAlarmFieldsState(WORD wGroupID,
									  DWORD inputMask, 	// 1 high, 0 low
									  DWORD changeMask,	// 1 changed, 0 unchanged
									  int iNumFields,
									  const CIPCField fields[]
									  )
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoIndicateAlarmNr(CSecID id,
								  BOOL bAlarm,
								  DWORD dwData1,
								  DWORD dwData2)
{
	WriteCmd(INP_INDICATE_ALARM_NR,id.GetID(),bAlarm,dwData1,dwData2);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnIndicateAlarmNr(CSecID id,
								  BOOL bAlarm,
								  DWORD dwData1,
								  DWORD dwData2)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoRequestGetSystemTime()
{
	WriteCmd(INP_REQUEST_GET_SYSTEM_TIME);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnRequestGetSystemTime()
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoConfirmGetSystemTime(const CSystemTime& st)
{
	DWORD dwParam1,dwParam2,dwParam3,dwParam4;

	dwParam1 = MAKELONG(st.wYear,st.wMonth);
	dwParam2 = MAKELONG(st.wDayOfWeek,st.wDay);
	dwParam3 = MAKELONG(st.wHour,st.wMinute);
	dwParam4 = MAKELONG(st.wSecond,st.wMilliseconds);

	WriteCmd(INP_CONFIRM_GET_SYSTEM_TIME,dwParam1,dwParam2,dwParam3,dwParam4);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnConfirmGetSystemTime(const CSystemTime& st)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoRequestSetSystemTime(const CSystemTime& st)
{
	DWORD dwParam1,dwParam2,dwParam3,dwParam4;

	dwParam1 = MAKELONG(st.wYear,st.wMonth);
	dwParam2 = MAKELONG(st.wDayOfWeek,st.wDay);
	dwParam3 = MAKELONG(st.wHour,st.wMinute);
	dwParam4 = MAKELONG(st.wSecond,st.wMilliseconds);

	WriteCmd(INP_REQUEST_SET_SYSTEM_TIME,dwParam1,dwParam2,dwParam3,dwParam4);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnRequestSetSystemTime(const CSystemTime& st)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoConfirmSetSystemTime()
{
	WriteCmd(INP_CONFIRM_SET_SYSTEM_TIME);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnConfirmSetSystemTime()
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoConfirmAlarm(CSecID id)
{
	WriteCmd(INP_CONFIRM_ALARM,id.GetID());
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnConfirmAlarm(CSecID id)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoRequestGetAlarmOffSpans(CSecID id)
{
	WriteCmd(INP_REQUEST_GET_ALARM_OFF_SPANS,id.GetID());
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnRequestGetAlarmOffSpans(CSecID id)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoConfirmGetAlarmOffSpans(CSecID id, const CSystemTimeSpans& spans)
{
	CIPCExtraMemory *pBubble = spans.BubbleFromSystemTimeSpans(this);
	WriteCmdWithExtraMemory(pBubble,
							INP_CONFIRM_GET_ALARM_OFF_SPANS,
							id.GetID(),
							spans.GetSize());
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnConfirmGetAlarmOffSpans(CSecID id, int iNumRecords, const CSystemTimeSpan data[])
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoRequestSetAlarmOffSpans(CSecID id, const CSystemTimeSpans& spans)
{
	CIPCExtraMemory *pBubble = spans.BubbleFromSystemTimeSpans(this);
	WriteCmdWithExtraMemory(pBubble,
							INP_REQUEST_SET_ALARM_OFF_SPANS,
							id.GetID(),
							spans.GetSize());
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnRequestSetAlarmOffSpans(CSecID id, int iNumRecords, const CSystemTimeSpan data[])
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoConfirmSetAlarmOffSpans(CSecID id)
{
	WriteCmd(INP_CONFIRM_SET_ALARM_OFF_SPANS,id.GetID());
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnConfirmSetAlarmOffSpans(CSecID id)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoRequestGetTimeZoneInformation()
{
	WriteCmd(INP_REQUEST_GET_ZIME_ZONE_INFORMATION);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnRequestGetTimeZoneInformation()
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoConfirmGetTimeZoneInformation(CTimeZoneInformation tzi)
{
	CTimeZoneInformation t = tzi;
	CIPCExtraMemory* pBubble = t.BubbleFrom(this);
	if (pBubble)
	{
		WriteCmdWithExtraMemory(pBubble,
								INP_CONFIRM_GET_ZIME_ZONE_INFORMATION);
	}
	else
	{
		OnCommandSendError(INP_CONFIRM_GET_ZIME_ZONE_INFORMATION);
	}
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnConfirmGetTimeZoneInformation(CTimeZoneInformation tzi)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoRequestSetTimeZoneInformation(CTimeZoneInformation tzi)
{
	CTimeZoneInformation t = tzi;
	CIPCExtraMemory* pBubble = t.BubbleFrom(this);
	if (pBubble)
	{
		WriteCmdWithExtraMemory(pBubble,
								INP_REQUEST_SET_ZIME_ZONE_INFORMATION);
	}
	else
	{
		OnCommandSendError(INP_REQUEST_SET_ZIME_ZONE_INFORMATION);
	}
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnRequestSetTimeZoneInformation(CTimeZoneInformation tzi)
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoConfirmSetTimeZoneInformation()
{
	WriteCmd(INP_CONFIRM_SET_ZIME_ZONE_INFORMATION);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnConfirmSetTimeZoneInformation()
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoRequestGetTimeZoneInformations()
{
	WriteCmd(INP_REQUEST_GET_ZIME_ZONE_INFORMATIONS);
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnRequestGetTimeZoneInformations()
{
	OverrideError();
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::DoConfirmGetTimeZoneInformations(const CTimeZoneInformations& tzi)
{
	CTimeZoneInformations t = tzi;
	CIPCExtraMemory* pBubble = t.BubbleFrom(this);
	if (pBubble)
	{
		WriteCmdWithExtraMemory(pBubble,
								INP_CONFIRM_GET_ZIME_ZONE_INFORMATIONS,
								t.GetSize());
	}
	else
	{
		OnCommandSendError(INP_CONFIRM_GET_ZIME_ZONE_INFORMATIONS);
	}
}
///////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCInput::OnConfirmGetTimeZoneInformations(const CTimeZoneInformations& tzi)
{
	OverrideError();
}
