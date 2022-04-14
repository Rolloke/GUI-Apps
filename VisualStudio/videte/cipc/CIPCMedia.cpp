/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCMedia.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCMedia.cpp $
// CHECKIN:		$Date: 30.03.05 16:13 $
// VERSION:		$Revision: 29 $
// LAST CHANGE:	$Modtime: 30.03.05 16:04 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdcipc.h"

#include "CIPCMedia.h"
#include "CipcExtraMemory.h"

#include "MediaSampleRecord.h"

// 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*********************************************************************************************
 Class      : CIPCMedia
 Function   : CIPCMedia
 Description: Constructor with arguments

 Parameters:   
  shmName : (E): Shared memory name for communication  (LPCTSTR)
  asMaster: (E): One station has to be the master  (BOOL)

 Result type:  ()
 created: July, 18 2003
 <TITLE CIPCMedia>
*********************************************************************************************/
CIPCMedia::CIPCMedia(LPCTSTR shmName, BOOL asMaster)
	: CIPC(shmName, asMaster)
{
}
////////////////////////////////////////////////////////////////////////////////
CIPCMedia::~CIPCMedia()
{
	StopThread();
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : DoRequestReset
 Description: Starts a request to reset the media unit

 Parameters:   
  wGroupID: (E): Group ID of the unit  (WORD)

 Result type:  (void)
 created: July, 18 2003
 <TITLE DoRequestReset>
*********************************************************************************************/
void CIPCMedia::DoRequestReset(WORD wGroupID)
{
	WriteCmd(CIPC_MEDIA_REQUESTRESET, wGroupID);
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : OnRequestReset
 Description: Receives a request to reset the media unit. Overwrite this function and call
              DoConfirmReset() !

 Parameters:   
  wGroupID: (E): Group ID of the unit  (WORD)

 Result type:  (void)
 created: July, 18 2003
 <TITLE OnRequestReset>
*********************************************************************************************/
void CIPCMedia::OnRequestReset(WORD wGroupID)
{
	OverrideError();
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : DoConfirmReset
 Description: Confirms the reset request for the media unit

 Parameters:   
  wGroupID: (E): Group ID of the unit  (WORD)

 Result type:  (void)
 created: July, 18 2003
 <TITLE DoConfirmReset>
*********************************************************************************************/
void CIPCMedia::DoConfirmReset(WORD wGroupID)
{
	WriteCmd(CIPC_MEDIA_CONFIRMRESET, wGroupID);
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : OnConfirmReset
 Description: Receives the confirmation of the reset request. Overwrite this function!

 Parameters:   
  wGroupID: (E): Group ID of the unit  (WORD)

 Result type:  (void)
 created: July, 18 2003
 <TITLE OnConfirmReset>
*********************************************************************************************/
void CIPCMedia::OnConfirmReset(WORD wGroupID)
{
	OverrideError();
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : DoRequestHardware
 Description: Starts a request for the hardware capabilities of the media unit

 Parameters:   
  dwSecID:  (E): Contains the GroupID of the unit and the Clients SubID (CSecID)
  dwIDRequest(E): ID for the request

 Result type:  (void)
 created: July, 18 2003
 <TITLE DoRequestHardware>
*********************************************************************************************/
void CIPCMedia::DoRequestHardware(CSecID SecID, DWORD dwIDRequest)
{
	WriteCmd(CIPC_MEDIA_REQUESTHARDWARE, SecID.GetID(), dwIDRequest);
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : OnRequestHardware
 Description: Receives a requet for the hardware capabilities of the media unit. Overwrite
              this function and call DoConfirmHardware(..)!

 Parameters:   
  dwSecID:  (E): Contains the GroupID of the unit and the Clients SubID (CSecID)
  dwIDRequest(E): ID for the request

 Result type:  (void)
 created: July, 18 2003
 <TITLE OnRequestHardware>
*********************************************************************************************/
void CIPCMedia::OnRequestHardware(CSecID SecID, DWORD dwIDRequest)
{
	OverrideError();
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : DoConfirmHardware
 Description: Confirms the request for the hardware capabilities of the media unit

 Parameters:   
  dwSecID    :  (E): Contains the GroupID of the unit and the Clients SubID (CSecID)
  iErrorCode : (E): Errorcode for hardware error  (int)
  dwFlags    : (E): Flags for the capabilities  (DWORD)
  dwIDRequest: (E): ID for the request
  pEM        : (E): Extra Memory for extended information (< LINK CIPCExtraMemory, CIPCExtraMemory*>)

 Result type:  (void)
 created: July, 18 2003
 <TITLE DoConfirmHardware>
*********************************************************************************************/
void CIPCMedia::DoConfirmHardware(CSecID SecID, int iErrorCode, DWORD dwFlags, DWORD dwIDRequest, CIPCExtraMemory*pEM)
{
	if (pEM)
	{
		CIPCExtraMemory *pMem = new CIPCExtraMemory(*pEM);
		WriteCmdWithExtraMemory(pMem, CIPC_MEDIA_CONFIRMHARDWARE, SecID.GetID(), iErrorCode, dwFlags, dwIDRequest);
	}
	else
	{
		WriteCmd(CIPC_MEDIA_CONFIRMHARDWARE, SecID.GetID(), iErrorCode, dwFlags, dwIDRequest);
	}
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : OnConfirmHardware
 Description: Receives the hardware capabilities of the media unit. Overwrite this function!

 Parameters:   
  dwSecID    :  (E): Contains the GroupID of the unit and the Clients SubID (CSecID)
  iErrorCode: (E): Errorcode for hardware error  (int)
  dwFlags    : (E): Flags for the capabilities  (DWORD)
  dwIDRequest: (E): ID for the request
  pEM       : (E): Extra Memory for extended information (< LINK CIPCExtraMemory, CIPCExtraMemory*>)

 Result type:  (void)
 created: July, 18 2003
 <TITLE OnConfirmHardware>
*********************************************************************************************/
void CIPCMedia::OnConfirmHardware(CSecID SecID, int errorCode, DWORD dwFlags, DWORD dwIDRequest, const CIPCExtraMemory*pEM)
{ 
	OverrideError();
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : DoRequestStartMediaEncoding
 Description: Starts a request to the media unit to encode media data
 Declaration: 
	for dwFlags
             * MEDIA_ENC_NEW_SEGMENT			The first frame of a segment is sent with long header
             * MEDIA_ENC_ONLY_INITIALIZE		The unit is only initialised
             * MEDIA_ENC_NO_COMPRESSION 		The data is not encoded
             * MEDIA_ENC_LOCAL_PLAY				The data is used in the same unit

 Parameters:   
  sourceID: (E): SecID of the source media unit  (<LINK CSecID, CSecID>)
  dwFlags : (E): Encoding Flags  (DWORD)
  dwUserData  : (E): for server to reconnect data to processes or clients (DWORD)

 Result type:  (void)
 created: August, 05 2003
 <TITLE DoRequestStartMediaEncoding>
*********************************************************************************************/
void CIPCMedia::DoRequestStartMediaEncoding(CSecID sourceID, DWORD dwFlags, DWORD dwUserData)
{
	WriteCmd(CIPC_MEDIA_REQUESTSTART_ENCODING, sourceID.GetID(), dwFlags, dwUserData);
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : DoRequestStopMediaEncoding
 Description: Starts a request to the media unit to stop media encoding.

 Parameters:   
  mediaID: (E): SecID of the media unit    (<LINK CSecID, CSecID>)

 Result type:  (void)
 created: August, 05 2003
 <TITLE DoRequestStopMediaEncoding>
*********************************************************************************************/
void CIPCMedia::DoRequestStopMediaEncoding(CSecID mediaID, DWORD dwFlags)
{
	WriteCmd(CIPC_MEDIA_REQUESTSTOP_ENCODING, mediaID.GetID(), dwFlags);
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : OnRequestStartMediaEncoding
 Description: Receives a request to start media encoding. The encoded media samples are
              delivered by DoIndicateMediaData(..). This may be also a confirmation for the
				  request.
 Declaration: 
	for dwFlags
		* MEDIA_ENC_NEW_SEGMENT			The first frame of a segment is sent with long header
      * MEDIA_ENC_ONLY_INITIALIZE		The unit is only initialised
      * MEDIA_ENC_NO_COMPRESSION 		The data is not encoded
      * MEDIA_ENC_LOCAL_PLAY				The data is used in the same unit
      * MEDIA_ENC_HOST_IS_SOURCE		The HostID identifies the source host
      * MEDIA_ENC_HOST_IS_DESTINATION	The HostID identifies the destiantion host

 Parameters:   
  sourceID: (E): SecID of the source media unit  (<LINK CSecID, CSecID>)
  dwFlags : (E): Encoding Flags  (DWORD)
  dwUserData  : (E): link id for server (DWORD)

 Result type:  (void)
 created: July, 18 2003
 <TITLE OnRequestStartMediaEncoding>
*********************************************************************************************/
void CIPCMedia::OnRequestStartMediaEncoding(CSecID mediaID, DWORD dwFlags, DWORD dwUserData)
{
	OverrideError();
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : DoIndicateMediaData
 Description: Indicates new media data

 Parameters:   
  media  : (E): encapsulated media data  (<LINK CIPCMediaSampleRecord, const CIPCMediaSampleRecord&>)
  mediaID: (E): SecID of the media unit   (<LINK CSecID, CSecID>)
  dwUserData  : (E): link id for server (DWORD)

 Result type:  (void)
 created: July, 18 2003
 <TITLE DoIndicateMediaData>
*********************************************************************************************/
void CIPCMedia::DoIndicateMediaData(const CIPCMediaSampleRecord& media, CSecID mediaID, DWORD dwUserData)
{
	CIPCExtraMemory *pMem = new CIPCExtraMemory(*media.GetBubble());
	WriteCmdWithExtraMemory(pMem, CIPC_MEDIA_INDICATE_DATA, mediaID.GetID(), dwUserData);
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : OnIndicateMediaData
 Description: Receives media data. Overwrite this function!

 Parameters:   
  media  : (E): encapsulated media data  (<LINK CIPCMediaSampleRecord, const CIPCMediaSampleRecord&>)
  mediaID: (E): SecID of the media unit   (<LINK CSecID, CSecID>)
  dwUserData  : (E): link id for server (DWORD)

 Result type:  (void)
 created: July, 18 2003
 <TITLE OnIndicateMediaData>
*********************************************************************************************/
void CIPCMedia::OnIndicateMediaData(const CIPCMediaSampleRecord& em, CSecID mediaID, DWORD dwUserData)
{
	OverrideError();
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : OnRequestStopMediaEncoding
 Description: Receives a request to stop media encoding. Overwrite this function!
                            No confirmation has to be sent.

 Parameters:   
  mediaID: (E): SecID of the media unit     (<LINK CSecID, CSecID>)
  dwFlags: (E): Flags for specific behavior  (DWORD)

 Result type:  (void)
 created: December, 11 2003
 <TITLE OnRequestStopMediaEncoding>
*********************************************************************************************/
void CIPCMedia::OnRequestStopMediaEncoding(CSecID mediaID, DWORD dwFlags)
{
	OverrideError();
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : DoRequestMediaDecoding
 Description: Starts a request to the unit to decode media data

 Parameters:   
  media  : (E): media data    (<LINK CIPCMediaSampleRecord, const CIPCMediaSampleRecord&>)
  mediaID: (E): SecID of the media unit    (<LINK CSecID, CSecID>)
  dwFlags: (E): Flags for specific behaviour  (DWORD)

 Result type:  (void)
 created: December, 11 2003
 <TITLE DoRequestMediaDecoding>
*********************************************************************************************/
void CIPCMedia::DoRequestMediaDecoding(const CIPCMediaSampleRecord& media, CSecID mediaID, DWORD dwFlags)
{
	CIPCExtraMemory *pMem = new CIPCExtraMemory(*media.GetBubble());
	WriteCmdWithExtraMemory(pMem, CIPC_MEDIA_REQUEST_DECODING, mediaID.GetID(), dwFlags);
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : OnRequestMediaDecoding
 Description: Receives a request to decode media data. Overwrite this function!

 Parameters:   
  media  : (E): media data  (<LINK const CIPCMediaSampleRecord, const>)
  mediaID: (E): SecID of the media unit  (<LINK CSecID, CSecID>)
  dwFlags: (E): Flags for specific behaviour  (DWORD)

 Result type:  (void)
 created: July, 18 2003
 <TITLE OnRequestMediaDecoding>
*********************************************************************************************/
void CIPCMedia::OnRequestMediaDecoding(const CIPCMediaSampleRecord &media, CSecID mediaID, DWORD dwFlags)
{
	OverrideError();
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : DoRequestStopMediaDecoding
 Description: Starts a request to the media unit to stop media decoding

 Parameters:   
  mediaID: (E): SecID of the media unit  (<LINK CSecID, CSecID>)
  dwFlags: (E): Flags for specific behaviour  (DWORD)

 Result type:  (void)
 created: July, 18 2003
 <TITLE DoRequestStopMediaDecoding>
*********************************************************************************************/
void CIPCMedia::DoRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags)
{
	WriteCmd(CIPC_MEDIA_REQUESTSTOP_DECODING, mediaID.GetID());
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : OnRequestStopMediaDecoding
 Description: Receives the request to stop decoding media data. Overwrite this function!

 Parameters:   
  mediaID: (E): SecID of the media unit  (<LINK CSecID, CSecID>)
  dwFlags: (E): Flags for specific behaviour  (DWORD)

 Result type:  (void)
 created: July, 18 2003
 <TITLE OnRequestStopMediaDecoding>
*********************************************************************************************/
void CIPCMedia::OnRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags)
{
	OverrideError();
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : DoRequestValues
 Description: Starts a request for data exchange.
 Declaration:
	for dwCmd
		- To get values use the flag MEDIA_GET_VALUE
		- To set values use the flag MEDIA_SET_VALUE

 Parameters:   
  mediaID   : (E): SecID of the media Unit    (<LINK CSecID, CSecID>)
  dwCmd     : (E): Command ID    (DWORD)
  [dwID]    : (E): Device ID    (DWORD)
  [dwValue] : (E): Value    (DWORD)
  [pData]   : (E): extendet data  (const CIPCExtraMemory*)

 Result type:  (void)
 Author: Rolf Kary Ehlers
 created: July, 25 2003
 <TITLE DoRequestValues>
*********************************************************************************************/
void CIPCMedia::DoRequestValues(CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory* pData)
{
	if (pData)
	{
		CIPCExtraMemory *pMem = new CIPCExtraMemory(*pData);
		WriteCmdWithExtraMemory(pMem, CIPC_MEDIA_REQUEST_VALUES, mediaID.GetID(), dwCmd, dwID, dwValue);
	}
	else
	{
		WriteCmd(CIPC_MEDIA_REQUEST_VALUES, mediaID.GetID(), dwCmd, dwID, dwValue);
	}
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : OnRequestValues
 Description: Receives a request for data exchange.
 Declaration: 
	for dwCmd
		- To get the command ID use the makro MEDIA_COMMAND_VALUE(dwCmd)
      - MEDIA_GET_VALUE: get values
      - MEDIA_SET_VALUE: set values
		- MEDIA_ERROR_OCCURRED: dwValue contains error code

 Parameters:   
  mediaID : (E): SecID of the media unit  (CSecID)
  dwCmd   : (E): Command ID  (DWORD)
  dwID    : (E): Device ID  (DWORD)
  dwValue : (E): Value  (DWORD)
  pData   : (E): extended data  (const CIPCExtraMemory*)

 Result type:  (void)
 Author: Rolf Kary Ehlers
 created: July, 25 2003
 <TITLE OnRequestValues>
*********************************************************************************************/
void CIPCMedia::OnRequestValues(CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{
	OverrideError();
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : DoConfirmValues
 Description: Confirms a request for data exchange.
 Declaration:
	for dwCmd
		- If an error occurres, inidicate this with the flag MEDIA_ERROR_OCCURRED

 Parameters:   
  mediaID : (E): SecID of the media unit  (CSecID)
  dwCmd   : (E): Command ID  (DWORD)
  dwID    : (E): Device ID  (DWORD)
  dwValue : (E): Value  (DWORD)
  pData   : (E): extended data  (const CIPCExtraMemory*)

 Result type:  (void)
 Author: Rolf Kary Ehlers
 created: July, 25 2003
 <TITLE DoConfirmValues>
*********************************************************************************************/
void CIPCMedia::DoConfirmValues(CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{
	if (pData)
	{
		CIPCExtraMemory *pMem = new CIPCExtraMemory(*pData);
		WriteCmdWithExtraMemory(pMem, CIPC_MEDIA_CONFIRM_VALUES, mediaID.GetID(), dwCmd, dwID, dwValue);
	}
	else
	{
		WriteCmd(CIPC_MEDIA_CONFIRM_VALUES, mediaID.GetID(), dwCmd, dwID, dwValue);
	}
}
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : OnConfirmValues
 Description: Receives a confirmation for a request of data exchange.
 Declaration:
	for dwCmd
		- To get the command ID use the makro MEDIA_COMMAND_VALUE(dwCmd)
		- MEDIA_GET_VALUE     : get values
		- MEDIA_SET_VALUE     : set values
		- MEDIA_ERROR_OCCURRED: dwValue contains the error code

 Parameters:   
  mediaID : (E): SecID of the media unit  (CSecID)
  dwCmd   : (E): Command ID  (DWORD)
  dwID    : (E): Device ID  (DWORD)
  dwValue : (E): Value  (DWORD)
  pData   : (E): extended data  (const CIPCExtraMemory*)


 Result type:  (void)
 Author: Rolf Kary Ehlers
 created: July, 25 2003
 <TITLE OnConfirmValues>
*********************************************************************************************/
void CIPCMedia::OnConfirmValues(CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : DoRequestInfoMedia
 Description: Requests information about media items, like microfons, loudspeakers ...
              Confirmation will be in OnConfirmInfoMedia

 Parameters:   
  wGroupID: (E): id of group requested 0 for all groups  (WORD)

 Result type:  (void)
 Author: Uwe Freiwald
 <GROUP CIPCMedia>
 <TOPICORDER 0>
 <KEYWORDS media>
*********************************************************************************************/
void CIPCMedia::DoRequestInfoMedia(WORD wGroupID)
{
	WriteCmd(CIPC_MEDIA_REQUEST_INFO,wGroupID);
}
////////////////////////////////////////////////////////////////////////////////
void CIPCMedia::OnRequestInfoMedia(WORD wGroupID)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CIPCMedia
 Function   : DoConfirmInfoMedia
 Description: Confirms the information about the mediagroups

 Parameters:   
  wGroupID   : (E): Requested GroupID  (WORD)
  iNumGroups : (E): Number of groups  (int)
  iNumRecords: (E): Number of Records  (int)
  records    : (E): The Record Array  (<LINK CIPCMediaRecordArray, const CIPCMediaRecordArray&>)

 Result type:  (void)
 created: September, 25 2003
 <TITLE DoConfirmInfoMedia>
*********************************************************************************************/
void CIPCMedia::DoConfirmInfoMedia(WORD wGroupID, int iNumGroups, int iNumRecords, const CIPCMediaRecordArray& records)
{
	if (records.GetSize()>0)
	{
		DWORD dwSum = 0;
	#ifdef _UNICODE
		BOOL bUnicode = (GetCodePage() == CODEPAGE_UNICODE);
	#endif
		for (int i=0;i<records.GetSize();i++)
		{
			// id and flags
			dwSum += 8;
			// length of string
			dwSum += 2;
	#ifdef _UNICODE
			CWK_String wks(records.GetAtFast(i)->GetName());
			dwSum += wks.CopyToMemory(bUnicode,NULL,-1,GetCodePage());
	#else
			dwSum += records.GetAtFast(i)->GetName().GetLength();
	#endif
		}
		CIPCExtraMemory *pBubble = new CIPCExtraMemory();
		if (pBubble->Create(this,dwSum))
		{
			BYTE *pPtr = (BYTE *)pBubble->GetAddressForWrite();
			BYTE *pTemp;
			WORD  wLen;
			for (int i=0;i<records.GetSize();i++)
			{
				CIPCExtraMemory::DWORD2Memory(pPtr,records.GetAtFast(i)->GetID().GetID());
				CIPCExtraMemory::DWORD2Memory(pPtr,records.GetAtFast(i)->GetFlags());
				CWK_String wks(records.GetAtFast(i)->GetName());
				pTemp = pPtr;
				pPtr += sizeof(WORD);
	#ifdef _UNICODE
				wLen = (WORD)wks.CopyToMemory(bUnicode,pPtr,-1,GetCodePage());
	#else
				wLen = (WORD)wks.CopyToMemory(pPtr,-1);
	#endif
				CIPCExtraMemory::WORD2Memory(pTemp, wLen);
				pPtr += wLen;
			}
			WriteCmdWithExtraMemory( pBubble, CIPC_MEDIA_CONFIRM_INFO, 
									 wGroupID, iNumGroups, iNumRecords,0);
		}
		else
		{
			WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in DoConfirmInfoMedia\n"));
		}
		WK_DELETE(pBubble);
	}
	else
	{
		WriteCmd(CIPC_MEDIA_CONFIRM_INFO,wGroupID,0,0,0);
	}
}
////////////////////////////////////////////////////////////////////////////////
void CIPCMedia::OnConfirmInfoMedia(WORD wGroupID, 
								    int iNumGroups, 
								    int iNumRecords, 
								    const CIPCMediaRecordArray& records)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////////////////////
BOOL CIPCMedia::HandleCmd(DWORD dwCmd, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, DWORD dwParam4, const CIPCExtraMemory *pExtraMem)
{
	BOOL isOkay=TRUE;
	switch (dwCmd) 
	{
		// requests
		case CIPC_MEDIA_REQUESTHARDWARE:		
			OnRequestHardware(dwParam1, dwParam2); 
			break;
		case CIPC_MEDIA_REQUESTRESET:		
			OnRequestReset((WORD)dwParam1); 
			break;
		case CIPC_MEDIA_REQUESTSTART_ENCODING:
			OnRequestStartMediaEncoding(CSecID(dwParam1), dwParam2, dwParam3);
			break;
		case CIPC_MEDIA_REQUESTSTOP_ENCODING:
			OnRequestStopMediaEncoding(CSecID(dwParam1), dwParam2);
			break;
		case CIPC_MEDIA_REQUEST_DECODING:
			if (pExtraMem)
			{
				CIPCMediaSampleRecord rec(CSecID(dwParam1), *pExtraMem);
				OnRequestMediaDecoding(rec, CSecID(dwParam1), dwParam2);
			}
			else
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
		case CIPC_MEDIA_REQUESTSTOP_DECODING:
			OnRequestStopMediaDecoding(CSecID(dwParam1), dwParam2);
			break;
		case CIPC_MEDIA_REQUEST_VALUES:
			OnRequestValues(CSecID(dwParam1), dwParam2, dwParam3, dwParam4, pExtraMem);
			break;
		// confirmations
		case CIPC_MEDIA_CONFIRMRESET:
			OnConfirmReset((WORD)dwParam1);
			break;
		case CIPC_MEDIA_CONFIRMHARDWARE:
			OnConfirmHardware(dwParam1,(int)dwParam2, dwParam3, dwParam4, pExtraMem);		
		   break;
		case CIPC_MEDIA_INDICATE_DATA:
			if (pExtraMem)
			{
				CIPCMediaSampleRecord rec(CSecID(dwParam1), *pExtraMem, false);
				OnIndicateMediaData(rec, CSecID(dwParam1), dwParam2);
			}
			else
			{
				OnMissingExtraMemory(dwCmd);
			}break;
		case  CIPC_MEDIA_CONFIRM_VALUES:
			OnConfirmValues(CSecID(dwParam1), dwParam2, dwParam3, dwParam4, pExtraMem);
			break;
		case CIPC_MEDIA_REQUEST_INFO:
			OnRequestInfoMedia((WORD)dwParam1);
			break;
		case CIPC_MEDIA_CONFIRM_INFO:
			if (pExtraMem)
			{
#ifdef _UNICODE
				BOOL bUnicode = pExtraMem->GetCIPC()->GetCodePage() == CODEPAGE_UNICODE;
#endif
				const BYTE *pByte = (const BYTE *)pExtraMem->GetAddress();
				CIPCMediaRecordArray records;
				records.SetAutoDelete(TRUE);
				int iNumRecords = (int)dwParam3;
				for (int i=0;i<iNumRecords;i++)
				{
					CIPCMediaRecord* pRecord = new CIPCMediaRecord();
					DWORD dwID = SECID_NO_ID;
					DWORD dwFlags = 0;
					WORD wLen = 0;

					dwID = CIPCExtraMemory::Memory2DWORD(pByte);
					dwFlags = CIPCExtraMemory::Memory2DWORD(pByte);
					wLen = CIPCExtraMemory::Memory2WORD(pByte);

					CWK_String s;
#ifdef _UNICODE
					s.InitFromMemory(bUnicode,pByte,wLen,pExtraMem->GetCIPC()->GetCodePage());
					pByte += wLen;
#else
					pByte += s.InitFromMemory(pByte,wLen);
#endif
					pRecord->Set(s,dwID,dwFlags);
					records.Add(pRecord);
				}
				
				OnConfirmInfoMedia((WORD)dwParam1,dwParam2,iNumRecords,records);
			}
			else
			{
				CIPCMediaRecordArray records;
				OnConfirmInfoMedia((WORD)dwParam1,dwParam2,0,records);
			}
			break;
		//////////////////
		default:
			// call superclass
			isOkay=CIPC::HandleCmd(dwCmd,dwParam1,dwParam2,dwParam3,dwParam4,pExtraMem);
			break;
	} // end of switch
	return isOkay;
}	// end  of ::HandleCmd
//////////////////////////////////////////////////////////////////////////////

