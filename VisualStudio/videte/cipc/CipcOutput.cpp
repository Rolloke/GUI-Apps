/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcOutput.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CipcOutput.cpp $
// CHECKIN:		$Date: 6.06.06 9:55 $
// VERSION:		$Revision: 162 $
// LAST CHANGE:	$Modtime: 6.06.06 9:53 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdcipc.h"

#include "CipcOutput.h"
#include "CipcExtraMemory.h"
#include "VideoJob.h"
#include "CipcOutputRecord.h"

// 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
 Compression |

The following values are defined:
<nl>
COMPRESSION_NONE <nl>
COMPRESSION_1	lowest compression, best quality, more data <nl>
COMPRESSION_2	<nl>
COMPRESSION_3	<nl>
COMPRESSION_4	<nl>
COMPRESSION_5	,highest compression, least quality, smallest data <nl>
 <c CIPCPictureRecord>, <c CIPCOutput>
*/

/*
 CompressionType |
The following values are defined
<nl>
COMPRESSION_UNKNOWN <nl>
COMPRESSION_JPEG <nl>
COMPRESSION_MJPEG <nl>
COMPRESSION_MPEG1 <nl>
COMPRESSION_MPEG2 <nl>
COMPRESSION_H261 <nl>
COMPRESSION_COLORJPEG <nl>

 <c CIPCPictureRecord>, <c CIPCOutput>
*/

/*
 Resolution |
The following values are defined
<nl>
RESOLUTION_NONE <nl>
RESOLUTION_HIGH <nl>
RESOLUTION_MID <nl>
RESOLUTION_LOW <nl>
 <c CIPCPictureRecord>, <c CIPCOutput>
*/

////////////////////////////////////////////////////////////////////////////
// nyd
static inline void RectToDwords(const CRect &rect, DWORD &dw1, DWORD &dw2)
{
	dw1 = (((DWORD)rect.left)<<16) |(DWORD)rect.top;
	dw2 = (((DWORD)rect.right)<<16)|(DWORD)rect.bottom;
}
////////////////////////////////////////////////////////////////////////////
// nyd
static inline CRect DwordsToRect(DWORD dw1, DWORD dw2)
{
	// l t r b
	CRect tmpRect(	HIWORD(dw1),LOWORD(dw1),
					HIWORD(dw2), LOWORD(dw2)
				);
	return tmpRect;
}
////////////////////////////////////////////////////////////////////////////
// nyd
static inline BYTE ResolutionToByte(Resolution res)
{
	return (BYTE) res;
}
////////////////////////////////////////////////////////////////////////////
// nyd
static inline Resolution ResolutionFromByte(BYTE b)
{
	if (b!=255) 
	{
		return (Resolution) b;
	} 
	else 
	{
		return RESOLUTION_NONE;
	}
}
////////////////////////////////////////////////////////////////////////////
// nyd
static inline BYTE CompressionToByte(Compression comp)
{
	return (BYTE)comp;
}
////////////////////////////////////////////////////////////////////////////
// nyd
static inline Compression CompressionFromByte(BYTE b)
{
	if (b!=255) 
	{
		return (Compression)b;
	} 
	else 
	{
		return COMPRESSION_NONE;
	}
}
////////////////////////////////////////////////////////////////////////////
// nyd
static inline void EncodedParametersToDwords(CSecID camID,
											 Resolution res,
											 Compression comp,
											 WORD wNumPicture,
											 DWORD dwBitrate,
											 DWORD dwUserData,
											 DWORD &dw1, 
											 DWORD &dw2, 
											 DWORD &dw3, 
											 DWORD &dw4)
{
	dw1 = camID.GetID();
	dw2 = dwBitrate;
	dw3 = dwUserData;
	// packed as: WORD num | BYTE res | BYTE comp
	dw4 = ((DWORD)wNumPicture)<<16
			| ((DWORD)ResolutionToByte(res))<<8
			| CompressionToByte(comp);
}
////////////////////////////////////////////////////////////////////////////
// nyd
static inline void DwordsToEncodedParameters(DWORD dw1, 
											 DWORD dw2, 
											 DWORD dw3, 
											 DWORD dw4,
											 CSecID &camID,
											 Resolution &res,
											 Compression &comp,
											 WORD &wNumPictures,
											 DWORD &dwBitrate,
											 DWORD &dwUserData)
{
	camID.Set(dw1);
	dwBitrate = dw2;
	dwUserData = dw3;
	// packed as: WORD num | BYTE res | BYTE comp
	wNumPictures =HIWORD(dw4);
	res = ResolutionFromByte(HIBYTE(LOWORD(dw4)));
	comp = CompressionFromByte(LOBYTE(LOWORD(dw4)));
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: constructor 
param: const char * | shmName | The systemwide unique shared memory name
for the CIPC object. The other process must use the same name.
param: BOOL | asMaster | The process which creates it's object first is master, so
asMaster must be TRUE, the second process must use FALSE. The server process
uses most times the asMaster flag as TRUE.
*/
CIPCOutput::CIPCOutput(LPCTSTR shmName, BOOL asMaster)
	: CIPC(shmName, asMaster)
{
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: destructor 
*/
CIPCOutput::~CIPCOutput()
{
	StopThread();
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::DoRequestReset(WORD wGroupID)
{
	WriteCmd(OUTP_REQ_RESET, wGroupID);
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::OnRequestReset(WORD wGroupID)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::DoConfirmReset(WORD wGroupID)
{
	WriteCmd(OUTP_CONF_RESET, wGroupID);
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::OnConfirmReset(WORD wGroupID)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::DoRequestSetRelay(CSecID relayID, BOOL bClosed)
{
	WriteCmd(OUTP_REQUEST_SET_RELAY,relayID.GetID(),bClosed);
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::OnRequestSetRelay(CSecID relayID, BOOL bClosed)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::DoConfirmSetRelay(CSecID relayID, BOOL bClosed)
{
	WriteCmd(OUTP_CONFIRM_SET_RELAY,relayID.GetID(),bClosed);
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::OnConfirmSetRelay(CSecID relayID, BOOL bClosed)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::DoRequestCurrentState(WORD wGroupID)
{
	WriteCmd(OUTP_REQ_CUR_STATE, wGroupID);
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::OnRequestCurrentState(WORD wGroupID)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::DoConfirmCurrentState(WORD wGroupID, DWORD dwStateBitmask)
{
	WriteCmd(OUTP_CONF_CUR_STATE, 
				wGroupID,
				dwStateBitmask
			);
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::OnConfirmCurrentState(WORD wGroupID,DWORD stateMask)
{
	OverrideError();
}

////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::DoRequestHardware(WORD wGroupID)
{
	WriteCmd(OUTP_REQ_HARDWARE, wGroupID);
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::OnRequestHardware(WORD wGroupID)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::DoConfirmHardware(WORD wGroupID,
								int iErrorCode,
								BOOL bCanBWCompress,
								BOOL bCanBWDecompress,
								BOOL bCanColorCompress,
								BOOL bCanColorDecompress,
								BOOL bCanOverlay
								)
{
	DWORD bitmask=0;
	bitmask |= (bCanBWCompress!=0);
	bitmask |= (bCanBWDecompress!=0)<<1;
	bitmask |= (bCanColorCompress!=0)<<2;
	bitmask |= (bCanColorDecompress!=0)<<3;
	bitmask |= (bCanOverlay!=0)<<4;
	
	WriteCmd(OUTP_CONF_HARDWARE,wGroupID,iErrorCode,bitmask);
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::OnConfirmHardware(WORD wGroupID, int errorCode,
								BOOL bCanSWCompress,
								BOOL bCanSWDecompress,
								BOOL bCanColorCompress,
								BOOL bCanColorDecompress,
								BOOL bCanOverlay)	// 0==no error, else some error code
{
	OverrideError();
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::DoIndicateClientChanged(ClientDisplayResource cdr)
{
	WriteCmd(OUTP_INDICATE_CLIENT_CHANGED,(DWORD)cdr);
}
////////////////////////////////////////////////////////////////////////////////
/*
Function: 
nyd 
*/
void CIPCOutput::OnIndicateClientChanged(ClientDisplayResource cdr)
{
	OverrideError();
}
////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutput::HandleCmd(DWORD dwCmd,
						DWORD dwParam1, DWORD dwParam2,
						DWORD dwParam3, DWORD dwParam4,
						const CIPCExtraMemory *pExtraMem
						)
{
	BOOL isOkay=TRUE;
	switch (dwCmd) 
	{
		case OUTP_REQ_HARDWARE:		
			OnRequestHardware((WORD)dwParam1); 
			break;
		case OUTP_REQ_RESET:		
			OnRequestReset((WORD)dwParam1); 
			break;
		case OUTP_REQ_CUR_STATE:	
			OnRequestCurrentState((WORD)dwParam1); 
			break;
		case OUTP_REQUEST_SET_RELAY:
			{
				CSecID relayID(dwParam1);
				BOOL bClosed = (BOOL) dwParam2;
				OnRequestSetRelay(relayID, bClosed);
			}
			break;
		case OUTP_CONFIRM_SET_RELAY:
			{
				CSecID relayID(dwParam1);
				BOOL bClosed = (BOOL) dwParam2;
				OnConfirmSetRelay(relayID, bClosed);
			}
			break;
		// confirmations
		case OUTP_CONF_RESET:
			OnConfirmReset((WORD)dwParam1);
			break;
		case OUTP_CONF_CUR_STATE:
			OnConfirmCurrentState((WORD)dwParam1,dwParam2);
			break;
		case OUTP_CONF_HARDWARE:
			{
				WK_ASSERT(dwParam1<=0xffff);
				WORD wGroupID=(WORD)dwParam1;
				DWORD bitmask=dwParam3;
				BOOL bCanSWCompress = (bitmask & 1)!=0;
				BOOL bCanSWDecompress = (bitmask & 2)!=0;
				BOOL bCanColorCompress = (bitmask & 4)!=0;
				BOOL bCanColorDecompress = (bitmask & 8)!=0;
				BOOL bCanOverlay = (bitmask & 16)!=0;
				OnConfirmHardware(wGroupID,(int)dwParam2,
					bCanSWCompress, bCanSWDecompress,
					bCanColorCompress, bCanColorDecompress,
					bCanOverlay
					);		
			}
			break;
		// PART: picture functions
		case PICT_REQ_WND_DISPLAY:
				{ 
				// dwParam1:	(lo,hi) oben links				  
				// dwParam2:	(lo,hi) unten rechts
				WORD wGroupID = (WORD) dwParam1;
				CRect rect = DwordsToRect(dwParam2,dwParam3);
				OnRequestSetDisplayWindow(wGroupID,rect) ;
				}
				break;
			case PICT_REQ_WND_OUTP:
				// dwParam1:	Camera system-number
				// dwParam2:	(lo,hi) oben links
				// dwParam3:	(lo,hi) unten rechts
				// dwParam4:	displayMode
				{
				WORD wGroupID = (WORD) dwParam1;
				CRect rect = DwordsToRect(dwParam2,dwParam3);
				OverlayDisplayMode odm = ODM_LOCAL_VIDEO;
				switch (dwParam4) {
					case ODM_LOCAL_VIDEO:
						odm = ODM_LOCAL_VIDEO;
					break;
					case ODM_DECODE:
						odm = ODM_DECODE;
					break;
					case ODM_ENCODE:
						odm = ODM_ENCODE;
					break;
				}

				OnRequestSetOutputWindow(wGroupID, rect, odm);
				}
				break;
			case PICT_REQ_DECOMPRESS:
			{
				Resolution res;
				WORD wGroupID = (WORD)dwParam1;
				if (dwParam2==RESOLUTION_QCIF) res=RESOLUTION_QCIF;
				else if (dwParam2==RESOLUTION_CIF) res=RESOLUTION_CIF;
				else res=RESOLUTION_2CIF;
				DWORD dwUserData = dwParam3;
				if (pExtraMem) {
					OnRequestJpegDecoding(wGroupID, res, *pExtraMem, dwUserData);
				} else {
					// NOT YET
				}
			}
			break;
		case PICT_REQ_JPEG_ENCODING:
			{
				CSecID camID(dwParam1);
				Resolution res= (Resolution)HIWORD(dwParam2);
				Compression comp=(Compression)LOWORD(dwParam2);
				int iNumPictures = (int) dwParam3;
				DWORD dwUserID = dwParam4;
				OnRequestNewJpegEncoding(camID, res, comp,
										iNumPictures,
										dwUserID
										);
			}
			break;
		case PICT_REQ_UNIT_VIDEO:
			{
				WORD wGroupID = (WORD)(dwParam1>>16);
				BYTE byCam =	(BYTE) ((dwParam1>>8)&255);
				BYTE byNextCam = (BYTE)(dwParam1&255);
				
				Compression comp = (Compression) LOBYTE(LOWORD(dwParam2));
				CompressionType ct = (CompressionType) HIBYTE(LOWORD(dwParam2));
				Resolution res = (Resolution)LOBYTE(HIWORD(dwParam2));
				// reserved byte for future use
				// BYTE bReserved = HIBYTE(HIWORD(dwParam2));
				int iNumPictures = (int) dwParam3;
				DWORD dwUserID = dwParam4;
				OnRequestUnitVideo(wGroupID,byCam,byNextCam,res,comp,ct,iNumPictures,dwUserID);
			}
			break;

		case PICT_CONF_WND_DISPLAY:	
			OnConfirmSetDisplayWindow((WORD)dwParam1);	
			break;
		case PICT_CONF_WND_OUTP:	
			OnConfirmSetOutputWindow((WORD)dwParam1, (int) dwParam2);	
			break;

		case PICT_REQ_START_H263:	
			// NOT YET safe enum type cast
			OnRequestStartH263Encoding(CSecID(dwParam1),(Resolution)HIWORD(dwParam2),(Compression)LOWORD(dwParam2), dwParam3,dwParam4);
			break;
		case PICT_CONF_START_H263:		
			OnConfirmStartH263Encoding(CSecID(dwParam1));
			break;
		case PICT_REQ_STOP_H263:		
			OnRequestStopH263Encoding(CSecID(dwParam1));
			break;
		case PICT_CONF_STOP_H263:	
			OnConfirmStopH263Encoding(CSecID(dwParam1));
			break;
		case PICT_INDICATE_H263_DATA:
			if (pExtraMem) 
			{
				CIPCPictureRecord pict(*pExtraMem);
				CSecID idArchive;
				if (dwParam2 != 0)
				{
					idArchive = dwParam2;
				}
				OnIndicateH263Data(pict, dwParam1,idArchive);
			} 
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
		case PICT_REQ_DISPLAY_H263:	
			if (pExtraMem) 
			{
				WORD wGroupID=(WORD)dwParam1;
				CRect rect = DwordsToRect(dwParam2,dwParam3);
				DWORD dwUserData = dwParam4;
				CIPCPictureRecord pict(*pExtraMem);
				OnRequestH263Decoding(wGroupID, rect, pict, dwUserData);
			} 
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
		case PICT_CONF_DISPLAY_H263:
			{
				WORD wGroupID = (WORD)dwParam1;
				OnConfirmH263Decoding(wGroupID, dwParam2);
			}
			break;
		case OUTP_REQ_INFO:
			OnRequestInfoOutputs((WORD)dwParam1);
			break;
		case OUTP_CONF_INFO:
			{
				WORD wGroupID = (WORD)dwParam1;
				int iNumGroups = dwParam2;
				int iNumRecords = dwParam3;	// sigh, can be zero!
				CIPCOutputRecord *records = NULL;
				if (iNumRecords && pExtraMem) 
				{
					records = new CIPCOutputRecord[max(1,iNumRecords)];	// make sure it's an array

					const BYTE *pPtr = (const BYTE *) pExtraMem->GetAddress();
					for (int i=0;i<iNumRecords;i++) {
						// reverse the sending
						WORD wRecLen= (WORD)(((WORD)pPtr[0]<<(WORD)8) | (WORD)pPtr[1]);
						pPtr += 2;
						CSecID tmpID( 
							  (DWORD)pPtr[0]<<24 
							| (DWORD)pPtr[1]<<16
							| (DWORD)pPtr[2]<<8
							| (DWORD)pPtr[3]
							);
						pPtr += 4;
						// flag
						WORD tmpFlags = (WORD)(((WORD)pPtr[0])<<8|pPtr[1]);
						pPtr+=2;
						// rest is text
						WK_ASSERT(wRecLen>=7);
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
					OnConfirmInfoOutputs(wGroupID,iNumGroups, iNumRecords,records);
					WK_DELETE_ARRAY(records);
				} else {
					CIPCOutputRecord dummyRecords[1];
					// no records or NULL bubble
					OnConfirmInfoOutputs(wGroupID,iNumGroups, iNumRecords,dummyRecords);
				}
			}
			break;
		case OUTP_REQUEST_LOCAL_VIDEO:
			{
				CSecID camID(dwParam1);
				BOOL bActive = (dwParam2 & (1L<<16))!=0;
				Resolution res = (Resolution) LOWORD(dwParam2);
				CRect rect = DwordsToRect(dwParam3,dwParam4);
				OnRequestLocalVideo(camID, res, rect, bActive);
			}
			break;
		case OUTP_REQUEST_STOP_H263_DECODING:
			{
				WORD wGroupID = (WORD)dwParam1;
				CRect rect = DwordsToRect(dwParam2,dwParam3);
				OnRequestStopH263Decoding(wGroupID, rect);
			}
			break;
		case OUTP_CONFIRM_JPEG_ENCODING_NEW:
			if (pExtraMem) 
			{
				CIPCPictureRecord *pPictRec;
				pPictRec = new CIPCPictureRecord(*pExtraMem);
				CSecID idArchive;
				if (dwParam2 != 0)
				{
					idArchive = dwParam2;
				}
				OnConfirmJpegEncoding(*pPictRec, dwParam1,idArchive);
				WK_DELETE(pPictRec);
			} 
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
		case OUTP_CONFIRM_JPEG_DECODING_NEW:
			if (pExtraMem) 
			{
				CIPCPictureRecord *pPictRec;
				pPictRec = new CIPCPictureRecord(*pExtraMem);
				WORD wGroupID = (WORD)dwParam1;
				WORD wXSize = HIWORD(dwParam2);
				WORD wYSize = LOWORD(dwParam2);
				OnConfirmJpegDecoding(
					wGroupID,
					wXSize, wYSize, 
					dwParam3,
					*pPictRec
					);
				WK_DELETE(pPictRec);
			} 
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
		case OUTP_INDICATE_LOCAL_VIDEO_DATA_NEW:
			if (pExtraMem) 
			{
				CIPCPictureRecord *pPictRec;
				pPictRec = new CIPCPictureRecord(*pExtraMem);
				CSecID camID = dwParam1;
				WORD wXSize = HIWORD(dwParam2);
				WORD wYSize = LOWORD(dwParam2);
				OnIndicateLocalVideoData( 
					camID,
					wXSize, wYSize,
					*pPictRec
					);
				WK_DELETE(pPictRec);
			}
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
		case CIPC_OUTPUT_REQUEST_DUMP_OUTPUT_WINDOW:
			{
				WORD wGroupID = HIWORD(dwParam1);
				WORD wUserID = LOWORD(dwParam1);
				CRect outputRect = DwordsToRect(dwParam2,dwParam3);
				int iFormat = (int) dwParam4;
				OnRequestDumpOutputWindow(
					wGroupID,
					wUserID,
					outputRect,
					iFormat);
			}
			break;
		case CIPC_OUTPUT_CONFIRM_DUMP_OUTPUT_WINDOW:
			{
				WORD wGroupID = HIWORD(dwParam1);
				WORD wUserID = LOWORD(dwParam1);
				int iFormat = (int) dwParam2;
				OnConfirmDumpOutputWindow(
					wGroupID,
					wUserID,
					iFormat,
					*pExtraMem
					);
			}
			break;
		////////////////
		case CIPC_REQUEST_SYNC:
			OnRequestSync(dwParam1,dwParam2,dwParam3);
			break;
		case CIPC_CONFIRM_SYNC:
			OnConfirmSync(dwParam1,dwParam2,dwParam3,dwParam4);
			break;
		case OUTP_REQUEST_IDLE_PICTURES:
			OnRequestIdlePictures((int)dwParam1);
			break;
		case OUTP_INDICATE_CLIENT_ACTIVE:
			{
				BOOL bClientIsActive = (dwParam1 != 0);
				WORD wGroupID = (WORD) dwParam2;
				OnIndicateClientActive(bClientIsActive,wGroupID);
			}
			break;
		case OUTP_REQUEST_SELECT_CAMERA:
			{
				CSecID camID(dwParam1);
				OnRequestSelectCamera(camID);
			}
			break;
		case OUTP_REQUEST_ENCODED_VIDEO:
			{
				CSecID camID;
				Resolution res;
				Compression comp;
				WORD wNumPictures;
				DWORD dwUserData;
				DWORD dwBitrate;
				DwordsToEncodedParameters(
					dwParam1, dwParam2, dwParam3, dwParam4,
					camID,res,comp,
					wNumPictures, dwBitrate, dwUserData
					);
				OnRequestEncodedVideo(camID,res,comp,wNumPictures,
										dwBitrate, dwUserData);
			}
			break;
		case OUTP_CONFIRM_ENCODED_VIDEO:
			if (pExtraMem) 
			{
				CIPCPictureRecord *pPictRec;
				pPictRec = new CIPCPictureRecord(*pExtraMem);
				CSecID idArchive;
				if (dwParam2 != 0)
				{
					idArchive = dwParam2;
				}
				OnConfirmEncodedVideo(*pPictRec, dwParam1,idArchive);
				WK_DELETE(pPictRec);
			} 
			else 
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
		case OUTP_INDICATE_CLIENT_CHANGED:
			{
				ClientDisplayResource cdr = CDR_INVALID;
				switch (dwParam1) 
				{
					case CDR_OVERLAY:
					case CDR_DECOMPRESS_H263:
						cdr = (ClientDisplayResource)dwParam1;
						break;
				}
				OnIndicateClientChanged(cdr);
			}
			break;
		case CIPC_OUTPUT_REQUEST_GET_MASK:
			OnRequestGetMask(CSecID(dwParam1),(MaskType)dwParam2,dwParam3);
			break;
		case CIPC_OUTPUT_CONFIRM_GET_MASK:
			if (pExtraMem)
			{
				CIPCActivityMask mask;
				if (mask.MaskFromBubble(pExtraMem))
				{
					OnConfirmGetMask(CSecID(dwParam1),dwParam2,mask);
				}
				else
				{
					OnCommandReceiveError(dwCmd);
				}
			}
			else
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
		case CIPC_OUTPUT_REQUEST_SET_MASK:
			if (pExtraMem)
			{
				CIPCActivityMask mask;
				if (mask.MaskFromBubble(pExtraMem))
				{
					OnRequestSetMask(CSecID(dwParam1),dwParam2,mask);
				}
				else
				{
					OnCommandReceiveError(dwCmd);
				}
			}
			else
			{
				OnMissingExtraMemory(dwCmd);
			}
			break;
		case CIPC_OUTPUT_CONFIRM_SET_MASK:
			OnConfirmSetMask(CSecID(dwParam1),(MaskType)dwParam2,dwParam3);
			break;
		case CIPC_OUTPUT_REQUEST_START_VIDEO:
			{
				Resolution res = (Resolution)LOBYTE(LOWORD(dwParam2));
				Compression comp = (Compression)HIBYTE(LOWORD(dwParam2));
				CompressionType ct = (CompressionType)LOBYTE(HIWORD(dwParam2));
				int iFPS = LOBYTE(LOWORD(dwParam3));
				int iIFrameInterval = HIBYTE(LOWORD(dwParam3));
				OnRequestStartVideo(CSecID(dwParam1),res,comp,ct,iFPS,iIFrameInterval,dwParam4);
			}
			break;
		case CIPC_OUTPUT_REQUEST_STOP_VIDEO:
			OnRequestStopVideo(CSecID(dwParam1),dwParam2);
			break;
		case CIPC_OUTPUT_INDICATE_VIDEO:
			if (pExtraMem) 
			{
				CIPCPictureRecord pict(*pExtraMem);
				
				// Abwärtskompatibilität alte Systeme senden eine 0!
				CSecID idArchive;
				if (dwParam4 != 0)
				{
					idArchive = dwParam4;
				}
				OnIndicateVideo(pict, dwParam1,dwParam2,dwParam3,idArchive);
			} 
			else 
			{
				OnMissingExtraMemory(dwCmd);
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
/*
Function:
Dimension and position of the view window relative to the origin of
the desktop window. Actions like decoding or life video ar display in the
so called output window.
 <mf CIPCOutput.DoRequestSetOutputWindow>, <mf CIPCOutput.OnRequestSetDisplayWindow>
*/
void CIPCOutput::DoRequestSetDisplayWindow(WORD wGroupID, const CRect &rect)
{
	DWORD dwRect1,dwRect2;
	RectToDwords(rect,dwRect1,dwRect2);
	WriteCmd(PICT_REQ_WND_DISPLAY, 
				wGroupID,
				dwRect1,
				dwRect2
				);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestSetDisplayWindow(WORD wGroupID,const CRect& rect)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoConfirmSetDisplayWindow(WORD wGroupID)
{
	WriteCmd(PICT_CONF_WND_DISPLAY,wGroupID);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnConfirmSetDisplayWindow(WORD wGroupID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
Child window within the DisplayWindow, which is used to display live 
or decoded video. The window with the action in it. The rest of the
display window is still buffered by the hardware.

 position and dimension are relative to the origin of the display window.

 <mf CIPCOutput.DoRequestSetDisplayWindow>, <mf CIPCOutput.OnRequestSetOutputWindow>
*/
void CIPCOutput::DoRequestSetOutputWindow(WORD wGroupID,
										  const CRect &rect,
										  OverlayDisplayMode odm)
{
	DWORD dwRect1, dwRect2;
	RectToDwords(rect,dwRect1,dwRect2);
	WriteCmd(PICT_REQ_WND_OUTP, 
				wGroupID,
				dwRect1,
				dwRect2,
				(DWORD)odm
				);
}
//////////////////////////////////////////////////////////////////////////////
void CIPCOutput::OnRequestSetOutputWindow(WORD wGroupID,
										  const CRect& rect,
										  OverlayDisplayMode odm
										  )
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoConfirmSetOutputWindow(WORD wGroupID, int iError)
{
	WriteCmd(PICT_CONF_WND_OUTP, wGroupID, iError);
}

//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnConfirmSetOutputWindow(WORD /*wGroupID*/, int /*iPictureResult*/)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestLocalVideo(CSecID camID, 
										Resolution res,
										const CRect &rect,
										BOOL bActive
									)
{
	DWORD dwRect1,dwRect2;
	RectToDwords(rect, dwRect1,dwRect2);
	
	WriteCmd(OUTP_REQUEST_LOCAL_VIDEO,
				camID.GetID(),
				(((DWORD)bActive)<<16) | (WORD)res,
				dwRect1, 
				dwRect2
				);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestLocalVideo(CSecID camID, 
									 Resolution res,
									 const CRect& rect,
									 BOOL bActive)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoConfirmLocalVideo(CSecID camID, 
										Resolution res,
										const CRect &rect,
										BOOL bActive,
										BOOL bIsOverlay
									)
{
	DWORD dwRect1,dwRect2;
	RectToDwords(rect, dwRect1,dwRect2);
	
	WriteCmd(OUTP_REQUEST_LOCAL_VIDEO,
				camID.GetID(),
				(((DWORD)bActive)<<16)
					| ((DWORD)bIsOverlay)<<17
					| (WORD)res,
				dwRect1, 
				dwRect2
				);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnConfirmLocalVideo(CSecID camID, 
									 Resolution res,
									 const CRect& rect,
									 BOOL bActive,
									 BOOL bIsOverlay)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoIndicateLocalVideoData(CSecID camID,
							WORD wXSize, WORD wYSize, 
							const CIPCPictureRecord &pict
							)
{
	if (pict.GetBubble())
	{
		CIPCExtraMemory *pBubble = new CIPCExtraMemory(*pict.GetBubble());
		WriteCmdWithExtraMemory(pBubble,
								OUTP_INDICATE_LOCAL_VIDEO_DATA_NEW,
								camID.GetID(),
								((DWORD)wXSize)<<16|wYSize
								);
	}
	else
	{
		OnCommandSendError(OUTP_INDICATE_LOCAL_VIDEO_DATA_NEW);
	}
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnIndicateLocalVideoData(CSecID camID,
										  WORD wXSize, 
										  WORD wYSize,
										  const CIPCPictureRecord& pict)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestEncodedVideo(CSecID camID, 
									   Resolution res,
									   Compression comp,
									   WORD wNumPictures,
									   DWORD dwBitrate,
									   DWORD dwUserData,
									   BOOL bDoSleep
									   )
{
	DWORD dw1,dw2,dw3,dw4;

	EncodedParametersToDwords(
				camID,
				res,
				comp,
				wNumPictures,
				dwBitrate,
				dwUserData,
				dw1, dw2, dw3, dw4
				);
	WriteCmd(OUTP_REQUEST_ENCODED_VIDEO,dw1,dw2,dw3,dw4, bDoSleep);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestEncodedVideo(CSecID camID, 
									   Resolution res,
									   Compression comp,
									   WORD wNumPictures,
									   DWORD dwBitrate,
									   DWORD dwUserData
									   )
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoConfirmEncodedVideo(const CIPCPictureRecord& pict,
									   DWORD dwUserData,
									   CSecID idArchive)
{
	if (pict.GetBubble())
	{
		CIPCExtraMemory *pBubble = new CIPCExtraMemory(*pict.GetBubble());
		WriteCmdWithExtraMemory(pBubble,
								OUTP_CONFIRM_ENCODED_VIDEO,
								dwUserData,
								idArchive.GetID());
	}
	else
	{
		OnCommandSendError(OUTP_CONFIRM_ENCODED_VIDEO);
	}
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnConfirmEncodedVideo(const CIPCPictureRecord& ref,
										DWORD dwUserData,CSecID idArchive)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestStartH263Encoding(CSecID camID, 
											Resolution res, Compression comp,
											DWORD dwBitrate,
											DWORD dwRecordTime
											)
{
	WriteCmd(PICT_REQ_START_H263,
		camID.GetID(),
		(((DWORD)res)&0xff)<<16 | ((BYTE)comp ),
		dwBitrate,
		dwRecordTime
		);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestStartH263Encoding(CSecID camID, 
											Resolution res,
											Compression comp,
											DWORD dwBitrate,
											DWORD dwRecordTime
											)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoConfirmStartH263Encoding(CSecID camID)
{
	WriteCmd(PICT_CONF_START_H263,camID.GetID());
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnConfirmStartH263Encoding(CSecID camID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoIndicateH263Data(const CIPCPictureRecord &pict, DWORD dwJobData,CSecID idArchive)
{
	if (pict.GetBubble())
	{
		CIPCExtraMemory *pMem = new CIPCExtraMemory(*pict.GetBubble());
		WriteCmdWithExtraMemory(pMem,PICT_INDICATE_H263_DATA, dwJobData,idArchive.GetID());
	}
	else
	{
		OnCommandSendError(PICT_INDICATE_H263_DATA);
	}
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnIndicateH263Data(const CIPCPictureRecord& pict, DWORD dwJobData,CSecID idArchive)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestStopH263Encoding(CSecID camID)
{
	WriteCmd(PICT_REQ_STOP_H263,camID.GetID());
}
/*
Function: nyd
 <mf CIPCOutput.DoRequestStopH263Encoding>
*/
void CIPCOutput::OnRequestStopH263Encoding(CSecID camID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoConfirmStopH263Encoding(CSecID camID)
{
	WriteCmd(PICT_CONF_STOP_H263,camID.GetID());
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnConfirmStopH263Encoding(CSecID camID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function: makes use of bitrate
 <mf CIPCOutput.OnRequestH263Decoding>
*/
void CIPCOutput::DoRequestH263Decoding(WORD wGroupID, 
										const CRect &rect, 
										const CIPCPictureRecord &pict,
										DWORD dwUserData
										)
{
	if (pict.GetBubble())
	{
		DWORD dwRect1,dwRect2;
		RectToDwords(rect,dwRect1,dwRect2);
		CIPCExtraMemory *pBubble = new CIPCExtraMemory(*pict.GetBubble());
		WriteCmdWithExtraMemory(pBubble,
							PICT_REQ_DISPLAY_H263,
							wGroupID,
							dwRect1,
							dwRect2,
							dwUserData
							);
	}
	else
	{
		OnCommandSendError(PICT_REQ_DISPLAY_H263);
	}
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestH263Decoding(WORD wGroupID,
									   const CRect& rect,
									   const CIPCPictureRecord& pict,
									   DWORD dwUserData
									 )
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoConfirmH263Decoding(WORD wGroupID, DWORD dwUserData)
{
	WriteCmd(PICT_CONF_DISPLAY_H263,wGroupID, dwUserData);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnConfirmH263Decoding(WORD wGroupID, DWORD dwUserData)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestStopH263Decoding(WORD wGroupID, const CRect &rect)
{
	DWORD dwRect1,dwRect2;
	RectToDwords(rect,dwRect1,dwRect2);
	WriteCmd(OUTP_REQUEST_STOP_H263_DECODING,wGroupID, dwRect1,dwRect2);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestStopH263Decoding(WORD wGroupID, const CRect& rect)
{
	OverrideError();
}
//
// JPEG Encoding

/*Function: NOT YET DOCUMENTED */
void CIPCOutput::DoRequestNewJpegEncoding(CSecID		camID, 
										Resolution	res, 
										Compression comp,
										int iNumPictures,
										DWORD		dwUserId,
										BOOL bDoSleep
								)
{
	WriteCmd(PICT_REQ_JPEG_ENCODING,
				camID.GetID(),					// param1
				(((DWORD)res<<16)|(WORD) comp),	// param2
				(DWORD) iNumPictures,			// param3
				dwUserId,						// param4
				bDoSleep
				);
}
/*Function: NOT YET DOCUMENTED */
void CIPCOutput::OnRequestNewJpegEncoding(CSecID camID, 
										Resolution res, 
										Compression comp,
										int iNumPictures,
										DWORD dwUserID
										)
{
	OverrideError();
}

void CIPCOutput::DoRequestUnitVideo(WORD wGroupID,
									BYTE byCam,
									BYTE byNextCam,
									Resolution	res, 
									Compression comp,
									CompressionType ct,
									int iNumPictures,
									DWORD		dwUserId,
									BOOL bDoSleep)
{
	DWORD dwParam1 = (wGroupID<<16)| (byCam<<8) | byNextCam;
	DWORD dwParam2 = MAKELONG(MAKEWORD(comp,ct),MAKEWORD(res,0));
	DWORD dwParam3 = (DWORD)iNumPictures;
	DWORD dwParam4 = (DWORD)dwUserId;


	WriteCmd(PICT_REQ_UNIT_VIDEO,dwParam1,dwParam2,dwParam3,dwParam4,bDoSleep);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestUnitVideo(WORD wGroupID,
									BYTE byCam,
									BYTE byNextCam,
									Resolution res, 
									Compression comp,
									CompressionType ct,
									int iNumPictures,
									DWORD dwUserID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoConfirmJpegEncoding(const CIPCPictureRecord& pict,
									   DWORD dwUserData,
									   CSecID idArchive,
									   BOOL bDoSleep /* = TRUE */ )
{
	if (pict.GetBubble())
	{
		CIPCExtraMemory *pBubble = new CIPCExtraMemory(*pict.GetBubble());
		WriteCmdWithExtraMemory(pBubble,
								OUTP_CONFIRM_JPEG_ENCODING_NEW,
								dwUserData,idArchive.GetID(),0,0,
								bDoSleep);
	}
	else
	{
		OnCommandSendError(OUTP_CONFIRM_JPEG_ENCODING_NEW);
	}
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnConfirmJpegEncoding(const CIPCPictureRecord& ref,
										DWORD dwUserData,CSecID idArchive)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestJpegDecodingExtra(WORD wGroupID, 
										  Resolution res, 
										  CIPCExtraMemoryPtr &ptrRef,
										  DWORD dwUserData
										  )
{
	WriteCmdWithExtraMemory(ptrRef,
		PICT_REQ_DECOMPRESS, 
		wGroupID, (DWORD)res, dwUserData, 0
		);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoConfirmJpegDecoding(WORD wGroupID,
									   WORD wXSize, 
									   WORD wYSize,
									   DWORD dwUserData,
									   const CIPCPictureRecord &pict)
{
	if (pict.GetBubble())
	{
		CIPCExtraMemory *pBubble = new CIPCExtraMemory(*pict.GetBubble());
		WriteCmdWithExtraMemory(pBubble,
								OUTP_CONFIRM_JPEG_DECODING_NEW,
								wGroupID,
								((DWORD)wXSize)<<16 | wYSize,
								dwUserData
								);
	}
	else
	{
		OnCommandSendError(OUTP_CONFIRM_JPEG_DECODING_NEW);
	}
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnConfirmJpegDecoding(	WORD wGroupID,
										WORD wXSize,
										WORD wYSize, 
										DWORD dwUserData,
										const CIPCPictureRecord& pict
									)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestJpegDecoding(	WORD wGroupID,
										Resolution res, 
										const CIPCExtraMemory& data,
										DWORD dwUserData
									 )
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestDumpOutputWindow(	WORD wGroupID,
											WORD wUserID,
											const CRect& outputRect,
											int iFormat
											)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoConfirmDumpOutputWindow(
				WORD wGroupID,
				WORD wUserID,
				int iFormat,
				const CIPCExtraMemory &bitmapData
				)
{
	CIPCExtraMemory *pTmp = new CIPCExtraMemory(bitmapData);
	WriteCmdWithExtraMemory(pTmp,
		CIPC_OUTPUT_CONFIRM_DUMP_OUTPUT_WINDOW,
		((DWORD)wGroupID)<<16 | wUserID,
		iFormat
		);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnConfirmDumpOutputWindow(	WORD wGroupID,
											WORD wUserID,
											int iFormat,
											const CIPCExtraMemory& bitmapData
											)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestInfoOutputs(WORD wGroupID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnConfirmInfoOutputs(	WORD wGroupID,
										int iNumGroups, 
										int numRecords, 
										const CIPCOutputRecord records[])
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestInfoOutputs(WORD wGroupID)
{
	WriteCmd(OUTP_REQ_INFO,wGroupID);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoConfirmInfoOutputs(WORD wGroupID, 
									  int iNumGroups, 
									  int iNumRecords, 
									 const CIPCOutputRecord records[])
{
	if (iNumRecords) 
	{
		// calc size for memory bubble. Each record has
		// wLEN:dwID:wFLAGS:TEXT
		DWORD dwMemoryLen=0;
#ifdef _UNICODE
		BOOL bUnicode = m_wCodePage == CODEPAGE_UNICODE;
		WORD  *pnLen = (WORD*) _alloca(sizeof(WORD)*iNumRecords);
#endif
		int i;
		for (i=0;i<iNumRecords;i++) 
		{
			// text + flags + id + wRecLen
#ifdef _UNICODE
			const CWK_String &str = records[i].GetName();
			pnLen[i] = (WORD)str.CopyToMemory(bUnicode, NULL, -1, m_wCodePage);
			dwMemoryLen += pnLen[i] + 2 + 4 + 2;
#else
			WORD wRecLen = (WORD)(records[i].GetName().GetLength() + 2 + 4 +2);
			dwMemoryLen += wRecLen;
#endif
		}

		CIPCExtraMemory *pMem = new CIPCExtraMemory();
		if (pMem->Create(this, max(1,dwMemoryLen)))
		{
			BYTE *pPtr = (BYTE *)pMem->GetAddressForWrite();
			// ugly byte filling to avoid struct alignment flags
			for (i=0;i<iNumRecords;i++) 
			{
#ifdef _UNICODE
				const CWK_String &str = records[i].GetName();
				WORD_TO_MEMORY(pPtr, (WORD)(pnLen[i] + 1 + 4 +2));
				// id 4 bytes
				DWORD_TO_MEMORY(pPtr, records[i].GetID().GetID());
				// flags 2 bytes
				WORD_TO_MEMORY(pPtr, records[i].GetFlags());

				pPtr += (WORD)str.CopyToMemory(bUnicode, pPtr, -1, m_wCodePage);
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
				// flags 2 bytes
				*pPtr++ = HIBYTE(records[i].GetFlags());
				*pPtr++ = LOBYTE(records[i].GetFlags());
				int len = records[i].GetName().GetLength();
				for (int j=0;j<len;j++) 
				{
					*pPtr++ = records[i].GetName()[j];
				}
#endif
			}
			WriteCmdWithExtraMemory( pMem, OUTP_CONF_INFO, 
										wGroupID, iNumGroups, iNumRecords,0);
		}
		else
		{
			WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in DoConfirmInfoOutputs\n"));
		}
		WK_DELETE(pMem);
	} 
	else 
	{
		WriteCmd( OUTP_CONF_INFO, wGroupID, iNumGroups, iNumRecords,0);
	}
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoIndicateClientActive(BOOL bClientIsActive, WORD wGroupID)
{
	WriteCmd(OUTP_INDICATE_CLIENT_ACTIVE,bClientIsActive,wGroupID);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnIndicateClientActive(BOOL bClientIsActive, WORD wGroupID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestDumpOutputWindow(WORD wGroupID,
										   WORD wUserID,
										   const CRect &outputRect,
										   int iFormat)
{
	DWORD dw2,dw3;
	RectToDwords(outputRect, dw2,dw3);
	WriteCmd(CIPC_OUTPUT_REQUEST_DUMP_OUTPUT_WINDOW,
		(((DWORD)wGroupID)<<16)|wUserID,
		dw2, dw3,
		iFormat
		);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestSync(DWORD dwTickSend, DWORD dwType, DWORD dwUserID)
{
	WriteCmd(CIPC_REQUEST_SYNC,dwTickSend,dwType,dwUserID);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestSync(DWORD dwTickSend, DWORD dwType, DWORD dwUserID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestIdlePictures(int iNumPics)
{
	WriteCmd(OUTP_REQUEST_IDLE_PICTURES,(DWORD)iNumPics);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestIdlePictures(int iNumPics)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoConfirmSync(DWORD dwTickConfirm, 
							   DWORD dwTickSend, 
							   DWORD dwType, 
							   DWORD dwUserID)
{
	WriteCmd(CIPC_CONFIRM_SYNC,dwTickConfirm, dwTickSend,dwType,dwUserID);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnConfirmSync(DWORD dwTickConfirm, 
							   DWORD dwTickSend, 
							   DWORD dwType,
							   DWORD dwUserID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestSelectCamera(CSecID camID)
{
	WriteCmd(OUTP_REQUEST_SELECT_CAMERA,camID.GetID());
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestSelectCamera(CSecID camID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
	WriteCmd(CIPC_OUTPUT_REQUEST_GET_MASK,camID.GetID(),type,dwUserID);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoConfirmGetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
	CIPCExtraMemory* pBubble = mask.BubbleFromMask(this);
	if (pBubble)
	{
		WriteCmdWithExtraMemory(pBubble,CIPC_OUTPUT_CONFIRM_GET_MASK,camID.GetID(),dwUserID);
	}
	else
	{
		OnCommandSendError(CIPC_OUTPUT_CONFIRM_GET_MASK);
	}
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnConfirmGetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
	CIPCExtraMemory* pBubble = mask.BubbleFromMask(this);
	if (pBubble)
	{
		WriteCmdWithExtraMemory(pBubble,CIPC_OUTPUT_REQUEST_SET_MASK,camID.GetID(),dwUserID);
	}
	else
	{
		OnCommandSendError(CIPC_OUTPUT_REQUEST_SET_MASK);
	}
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoConfirmSetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
	WriteCmd(CIPC_OUTPUT_CONFIRM_SET_MASK,camID.GetID(),type,dwUserID);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnConfirmSetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestStartVideo(CSecID id,			// camera nr.
									 Resolution res,		// resolution
									 Compression comp,		// image size or bitrate
									 CompressionType ct,	// JPEG, YUV, MPEG4 ...
									 int iFPS,				// frames per second
									 int iIFrameInterval,   // for MPEG4 I-Frame interval, 1=I-Frame only
									 DWORD dwUserData		// unique server id
									)
{
	DWORD dw1,dw2,dw3;

	dw1 = id.GetID();
	dw2 = MAKELONG(MAKEWORD((BYTE)res,(BYTE)comp),MAKEWORD((BYTE)ct,0));
	dw3 = MAKELONG(MAKEWORD((BYTE)iFPS,(BYTE)iIFrameInterval),MAKEWORD(0,0));

	WriteCmd(CIPC_OUTPUT_REQUEST_START_VIDEO,
			 dw1,dw2,dw3,dwUserData);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestStartVideo(CSecID id,			// camera nr.
									 Resolution res,		// resolution
									 Compression comp,		// image size or bitrate
									 CompressionType ct,	// JPEG, YUV, MPEG4 ...
									 int iFPS,
									 int iIFrameInterval, 
									 DWORD dwUserData		// unique server id
									)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoIndicateVideo(const CIPCPictureRecord& pict,
								 DWORD dwMDX,
								 DWORD dwMDY,
								 DWORD dwUserData,
								 CSecID idArchive)
{
	if (pict.GetBubble())
	{
		CIPCExtraMemory *pBubble = new CIPCExtraMemory(*pict.GetBubble());
		WriteCmdWithExtraMemory(pBubble,
								CIPC_OUTPUT_INDICATE_VIDEO,
								dwMDX,dwMDY,dwUserData,idArchive.GetID());
	}
	else
	{
		OnCommandSendError(CIPC_OUTPUT_INDICATE_VIDEO);
	}
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnIndicateVideo(const CIPCPictureRecord& pict,
								 DWORD dwMDX,
								 DWORD dwMDY,
								 DWORD dwUserData,
								 CSecID idArchive)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::DoRequestStopVideo(CSecID camID,
									DWORD dwUserData)
{
	WriteCmd(CIPC_OUTPUT_REQUEST_STOP_VIDEO,camID.GetID(),dwUserData);
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
void CIPCOutput::OnRequestStopVideo(CSecID camID,
								    DWORD dwUserData)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
CIPCPictureRecords::~CIPCPictureRecords()
{
	SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////////////
/*
Function:
nyd
*/
CIPCPictureRecord *CIPCPictureRecords::CreateSuperBubble(const CIPC *pCipc,
														 LPCTSTR pInfoString)
{
	CIPCPictureRecord *pResult = NULL;
	int i;

	Lock();
	if (GetSize()==0)
	{
		WK_TRACE(_T("No data for CreateSuperBubble\n"));
	}
	else if (pCipc==NULL)
	{
		WK_TRACE(_T("No cipc for CreateSuperBubble\n"));
	}
	else 
	{
		// calc required space
		DWORD dwSum=0;
		for (i=0;i<GetSize();i++) 
		{
			CIPCPictureRecord *pRec=GetAtFast(i);
			dwSum += pRec->GetDataLength();
#if 0
			if (wBlockNr!=0 && wBlockNr+1!=pRec->GetBlockNr()) 
			{
				WK_TRACE(_T("H263Data block error %d followed by %d\n"), wBlockNr, pRec->GetBlockNr());
			}
#endif
		}
		BYTE *pTmp = new BYTE[dwSum];
		DWORD dwOffset=0;

		for (i=0;i<GetSize();i++) 
		{
			CIPCPictureRecord *pRec=GetAtFast(i);
			CopyMemory(pTmp+dwOffset,pRec->GetData(),pRec->GetDataLength());
			dwOffset += pRec->GetDataLength();
		}
		// create result
		const CIPCPictureRecord &firstPict = *GetAtFast(0);
		pResult = new CIPCPictureRecord(
			pCipc,
			pTmp, dwSum,
			firstPict.GetCamID(),
			firstPict.GetResolution(),
			firstPict.GetCompression(),
			firstPict.GetCompressionType(),
			firstPict.GetTimeStamp(),
			0,		// job time, OOPS no sum
			pInfoString,	// infoString
			firstPict.GetBitrate(),
			SPT_GOP_PICTURE,	// OOPS always or special singles
			firstPict.GetBlockNr()	// OOPS
		);
		WK_DELETE_ARRAY(pTmp);

	}
	DeleteAll();
	Unlock();

	return pResult;
}
//////////////////////////////////////////////////////////////////////////////
const _TCHAR *NameOfEnum(Resolution r)
{
	switch(r) 
	{
		NAME_OF_ENUM(RESOLUTION_NONE);
		NAME_OF_ENUM(RESOLUTION_2CIF);
		NAME_OF_ENUM(RESOLUTION_CIF);
		NAME_OF_ENUM(RESOLUTION_QCIF);
		NAME_OF_ENUM(RESOLUTION_4CIF);
		NAME_OF_ENUM(RESOLUTION_VGA);
		NAME_OF_ENUM(RESOLUTION_QVGA);
		NAME_OF_ENUM(RESOLUTION_QSIF);
		NAME_OF_ENUM(RESOLUTION_XVGA);
		NAME_OF_ENUM(RESOLUTION_4VGA);
		NAME_OF_ENUM(RESOLUTION_HDTV);
		default: 
			return _T("RESOLUTION_INVALID");
	}
}
//////////////////////////////////////////////////////////////////////////////
CString ShortNameOfEnum(Resolution r)
{
	CString sName = NameOfEnum(r);
	int nFind = sName.Find(_T("_"));
	if (nFind != -1)
	{
		sName = sName.Mid(nFind+1);
	}
	return sName;
}
//////////////////////////////////////////////////////////////////////////////
Resolution EnumOfName(LPCTSTR psz)
{
	int i;
	CString sName, sSearch(psz);
	sSearch.MakeUpper();
	for (i=RESOLUTION_2CIF; i<RESOLUTION_LAST; i++)
	{
		sName = NameOfEnum((Resolution) i);
		if (sName.Find(sSearch) != -1)
		{
			return (Resolution)i;
		}
	}
	return RESOLUTION_NONE;
}
//////////////////////////////////////////////////////////////////////////////
SIZE SizeOfEnum(Resolution r, BOOL bNTSC/*=FALSE*/)
{
	SIZE sz = {0,0};
	if (HIWORD(r))
	{
		sz.cx = LOWORD(r);
		sz.cy = HIWORD(r);
	}
	else if (bNTSC)
	{
		switch(r) 
		{
			case RESOLUTION_2CIF: sz.cx = 720; sz.cy = 240; break;
			case RESOLUTION_CIF:  sz.cx = 352; sz.cy = 240; break;
			case RESOLUTION_QCIF: sz.cx = 176; sz.cy = 120; break;
			case RESOLUTION_4CIF: sz.cx = 720; sz.cy = 480; break;
			case RESOLUTION_NONE: sz.cx =   0; sz.cy =   0; break;
			default: 
				break;
		}
	}
	else
	{
		switch(r) 
		{
			case RESOLUTION_2CIF: sz.cx =  704; sz.cy =  288; break;
			case RESOLUTION_CIF:  sz.cx =  352; sz.cy =  288; break;
			case RESOLUTION_QCIF: sz.cx =  176; sz.cy =  144; break;
			case RESOLUTION_4CIF: sz.cx =  704; sz.cy =  576; break;
			case RESOLUTION_VGA:  sz.cx =  640; sz.cy =  480; break;
			case RESOLUTION_QVGA: sz.cx =  320; sz.cy =  240; break;
			case RESOLUTION_QSIF: sz.cx =  160; sz.cy =  120; break;
			case RESOLUTION_XVGA: sz.cx = 1024; sz.cy =  768; break;
			case RESOLUTION_4VGA: sz.cx = 1280; sz.cy =  960; break;
			case RESOLUTION_HDTV: sz.cx = 1920; sz.cy = 1080; break;
			case RESOLUTION_NONE: sz.cx =    0; sz.cy =    0; break;
			default: 
				break;
		}
	}
	return sz;
}
//////////////////////////////////////////////////////////////////////////////
Resolution EnumOfSize(SIZE sz)
{
	int i;
	for (i=RESOLUTION_2CIF; i<RESOLUTION_LAST; i++)
	{
		SIZE szEnum = SizeOfEnum((Resolution)i);
		if (abs(szEnum.cx-sz.cx)<=16 && szEnum.cy == sz.cy)
		{
			return (Resolution)i;
		}
		else if (i<RESOLUTION_VGA)
		{
			SIZE szEnum = SizeOfEnum((Resolution)i, TRUE);
			if (abs(szEnum.cx-sz.cx)<=16 && szEnum.cy == sz.cy)
			{
				return (Resolution)i;
			}
		}
	}
	return RESOLUTION_NONE;
}

static CString sCompression;
//////////////////////////////////////////////////////////////////////////////
// nyd
const _TCHAR *NameOfEnum(Compression c)
{
	switch (c) 
	{
	case COMPRESSION_NONE: 
		sCompression = _T("COMPRESSION_NONE"); 
		break;
	case COMPRESSION_1:
	case COMPRESSION_2:
	case COMPRESSION_3:
	case COMPRESSION_4:
	case COMPRESSION_5:
	case COMPRESSION_6:
	case COMPRESSION_7:
	case COMPRESSION_8:
	case COMPRESSION_9:
	case COMPRESSION_10:
	case COMPRESSION_11:
	case COMPRESSION_12:
	case COMPRESSION_13:
	case COMPRESSION_14:
	case COMPRESSION_15:
	case COMPRESSION_16:
	case COMPRESSION_17:
	case COMPRESSION_18:
	case COMPRESSION_19:
	case COMPRESSION_20:
	case COMPRESSION_21:
	case COMPRESSION_22:
	case COMPRESSION_23:
	case COMPRESSION_24:
	case COMPRESSION_25:
	case COMPRESSION_26:
		sCompression.Format(_T("COMPRESSION_%d"),(int)c+1);
		break;
	default:
		sCompression = _T("COMPRESSION_INVALID");
	};
	return (const _TCHAR *)sCompression;
}
//////////////////////////////////////////////////////////////////////////////
// nyd
extern const _TCHAR *NameOfEnum(CompressionType ct)
{
	switch (ct) 
	{
		NAME_OF_ENUM(COMPRESSION_UNKNOWN);
		NAME_OF_ENUM(COMPRESSION_H263); 
		NAME_OF_ENUM(COMPRESSION_JPEG); 
		NAME_OF_ENUM(COMPRESSION_PRESENCE); 
		NAME_OF_ENUM(COMPRESSION_RGB_24);
		NAME_OF_ENUM(COMPRESSION_YUV_422);
		NAME_OF_ENUM(COMPRESSION_YUV_420);
		NAME_OF_ENUM(COMPRESSION_MPEG4);
		default:
			return _T("COMPRESSIONTYPE_INVALID");
	}
}

