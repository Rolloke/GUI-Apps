

#include "stdafx.h"
#include "PTBox.h"
#include "pt.h"

#include "RawDataArray.h"
#include "CIPCPipeInputPT.h"
#include "CIPCPipeOutputPT.h"
#include "PictureRecord.h"

#include "ISDNConnection.h"
#include "CapiQueue.h"
#include "CIPCStringDefs.h"

#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

int CPTBox::m_iFrameCount=0;
BOOL CPTBox::m_bDoHexDumpSend = FALSE;
BOOL CPTBox::m_bDoHexDumpReceive = FALSE;
BOOL CPTBox::m_bDisableFrameGrabbing = FALSE;
BOOL CPTBox::m_bDoTraceFrameSize = FALSE;
BOOL CPTBox::m_bDisableEEPWrite = FALSE;
BOOL CPTBox::m_bLowAsMid = FALSE;

static const BYTE theInitUserData = 0xAB;	// CAVEAT, it's const (ReadOnly)

BYTE WORD2BCD(WORD w)
{
	BYTE l = (BYTE)(w % 10);
	BYTE h = (BYTE)(w / 10);
	BYTE r;

	r = (BYTE)(h * 16 + l);

	return r;
}

CPTBox::CPTBox(CISDNConnection *pConnection)
{
	m_pConnection = pConnection;
	SetPTConnectionState(PCS_NOT_CONNECTED);

	m_pInputPT = NULL;
	m_pOutputPT = NULL;

	m_byPictureType = 0;
	m_requestedCamID = SECID_NO_ID;
	m_bRequestForceIntra = FALSE;

	m_dwRelayBits = 0;
	m_iVersion=0;

	m_NumOutstandingCmds=0;
}

CPTBox::~CPTBox()
{
	m_cameras.DeleteAll();
	m_alarms.DeleteAll();
}

void CPTBox::SetPTConnectionState(PTConnectionState newState)
{
	WK_TRACE(_T("New PTState %s\n"),(const char *)NameOfEnum(newState));
	m_PTState = newState;
	m_dwLastStateChange = GetTickCount();
}

void CPTBox::ClearInternalData()
{
	m_NumOutstandingCmds=0;
	m_byPictureType = 0;
	m_rawData.Clear();
	m_requestedCamID = SECID_NO_ID;
	m_alarmCamID.Set( SECID_GROUP_OUTPUT, 0);	// default is first camera
	
	m_cameras.DeleteAll();	// filled via SendPTConfig requests
	m_alarms.DeleteAll();	// filled via SendPTConfig requests

	m_sVersion.Empty();
	m_sName.Empty();
	m_sNumber.Empty();

}


void CPTBox::OnRawData(const BYTE *pData,DWORD dwDataLen)
{
	// commands are not aligned to capi blocks
	// so it might be necessary to collect data until a cmd is complete
	// a CByteArray should do the job, slow but simple to use
	if (m_bDoHexDumpReceive) {
		CISDNConnection::HexDump(pData,dwDataLen,_T("recv"));
	}

	m_rawData.Append(pData,dwDataLen);

	BOOL bAbort=FALSE;
	// check for first iST protocol command
	if (GetPTConnectionState()!=PCS_CONNECTED 
		&& m_rawData.GetSize()>4 
		&& m_rawData[0]==0x7c
		&& m_rawData[1]==0	// channel nr, should always be 0 the first time
		) {
		BYTE byService = m_rawData[3];
		if (byService==0x81
			|| byService==0xc1
			|| byService==0x61)
		{
			m_pConnection->PanicClose(_T("Protocol error, called from remote ISDNUnit"));
			// NOT YET some nifty DoIndicate.... according to the protocol
			// NOT YET some trick to catch the second BChannel as early as possible
			CString sMsg;
			if (sMsg.LoadString(IDS_PROCTOCOL_ERROR_REMOTE_IST)==FALSE) {
				sMsg=_T("Protocol error, called by remote ISDNUnit.");
			}

			CWK_RunTimeError runTimeError(REL_MESSAGE, RTE_PROTOCOL, sMsg);
			runTimeError.Send();

			bAbort=TRUE;
		}
	}
	// NOT YET check for other invalid first data
	if (bAbort==FALSE) {
		CheckForPTCmd();
	}
}

void CPTBox::CheckForPTCmd()
{
	BOOL bThereWasACmd = FALSE;
	do {
		bThereWasACmd = FALSE;
		const BYTE *pFullData = m_rawData.GetData();
		if (m_rawData.GetSize()>2) {
			if (m_rawData[0]==0xfe) {	// OOPS SOP without Ack ?
				// special check for fe 00 fe NACK ???? OOOPS
				if (m_rawData.GetSize()>2
					&& m_rawData[1]==0
					&& m_rawData[2]==0xFE) {
					m_NumOutstandingCmds--;
					if (m_NumOutstandingCmds<0) {
						m_NumOutstandingCmds=0;
					}
					WK_TRACE(_T("NACK1? in state %s (outstanding %d)\n"),NameOfEnum(GetPTConnectionState()),GetNumOutstandingCmds());
					m_rawData.RemoveHead(3);
					CISDNConnection::HexDump(m_rawData.GetData(),m_rawData.GetSize(),_T("Data after NACK"));
					if (GetPTConnectionState()==PCS_CONNECTED) {	// TESTRHACK 15.1.99
						DoGrabFrame(FALSE);
					} else if (GetPTConnectionState()==PCS_INITIALIZING) {
						// try again to get the config data (new 0199)
						// OOPS can NOT do that ClearInternalData();
						// since more data arrives from the first try
						// probably we can overwrite data
						SendPTConfig();
					}
					bThereWasACmd = TRUE;
				} else {
					int ixEndOfPicture = m_rawData.GetFFFFIndex();
					if (ixEndOfPicture != -1) {
						// +1 for second FF
						// +1 to 'convert' from 0..n-1 index to size 1..n
						OnPTFrame(m_rawData.GetData(),ixEndOfPicture+1+1);	// second FF and length not index
						// special check for triple FF FF FF
						int iAddOne = 0;
						if (m_rawData.GetSize()>ixEndOfPicture+1+1
							&& m_rawData[ixEndOfPicture+1+1]==0xFF) {
							iAddOne = 1;
							WK_STAT_PEAK(_T("Reset"),1,_T("TrailingFF"));
						}
						m_rawData.RemoveHead(ixEndOfPicture+1+1+iAddOne);			// second FF and length not index
						bThereWasACmd = TRUE;
					} else {
						// still no EndOfPicture found
						// WK_TRACE(_T("NACK2? in state %s\n"),NameOfEnum(GetPTConnectionState()));
					}
				}
			} else {
				if (pFullData[0]==0xff 
					&& pFullData[1]==0xd3
					&& pFullData[2]==0x01) {
					WK_STAT_PEAK(_T("Reset"),1,_T("StrangeFF"));
					m_rawData.RemoveHead(1);
					bThereWasACmd = TRUE;	// fake it to stay in loop
				} else {
					// no pict data
					// UNUSED BYTE byCmd = m_rawData[0];
					BYTE byLen = m_rawData[1];
					if (byLen+1+1+1<= m_rawData.GetSize()) {
						// extract first cmd
						OnPTCmd(m_rawData.GetData(),byLen+1+1+1);
						m_rawData.RemoveHead(byLen+1+1+1);
						bThereWasACmd = TRUE;
					} else {
						// still need data
					}
				}
			}
		} else {
			// mini data less than 3
		}
	} while (bThereWasACmd);
}
// CAVEAT this functions is thread safe
// it only adds a record to the SendQueue
// the SendQueue is handled by the CapiThread
void CPTBox::SendPTCmd(BYTE byCmd, DWORD dwDataLen, const BYTE *pData)
{
	if (dwDataLen>255) {
		WK_TRACE_ERROR(_T("invalid PTBlock len %d\n"),dwDataLen);
		return;	// <<< EXIT >>>
	}

	// OOPS make local copy each time ? or callers resposibility to provide enough space ?
	BYTE *pTmp=new BYTE[dwDataLen+1+1+1];	// plus cmd len and checksum
	BYTE bySum=0;
	
	pTmp[0] = byCmd;
	pTmp[1] = (BYTE)dwDataLen;

	bySum = (BYTE)(pTmp[0]+pTmp[1]);	// include the first two bytes

	// calc checksum and copy data
	for (DWORD i=0;i<dwDataLen;i++) {
		pTmp[2+i] = pData[i];	// copy into the tmp buffer
		bySum = (BYTE)(bySum + pData[i]);
	}
	pTmp[dwDataLen+2] = bySum;

	if (m_bDoHexDumpSend) {
		CISDNConnection::HexDump(pTmp,dwDataLen+1+1+1,_T("send"));
	}

	m_NumOutstandingCmds++;
	// WK_STAT_LOG(_T("PTCmds"),GetNumOutstandingCmds(),_T("SendingCmds"));	// TESTHACK

	CCapiQueueRecord *pNewRecord = new CCapiQueueRecord(dwDataLen+1+1+1,pTmp);
	m_pConnection->GetSendQueue()->AddToSendQueue(pNewRecord, FALSE);


	WK_DELETE_ARRAY(pTmp);
}

static inline BYTE DecodeBCD(BYTE byIn)
{
	BYTE byResult = (BYTE)((byIn & 15) + (BYTE)(byIn>>4)*10);
	return byResult;
}

void CPTBox::OnPTCmd(const BYTE *pFullData, DWORD dwFullLen)
{
	m_NumOutstandingCmds--;
	if (m_NumOutstandingCmds<0 ) {
		m_NumOutstandingCmds=0;
	}
	// WK_STAT_LOG(_T("PTCmds"),GetNumOutstandingCmds(),_T("ReceivedCmds"));	// TESTHACK
	// translate PT protocl do DoXXXs call in Pipes
	// and manage internal PT data
	// first BYTE is cmd
	// second data len
	// following is data
	// last byte checksum
	// NOT YET
	BYTE byCmd = pFullData[0];
	DWORD dwDataLen = pFullData[1];
	const BYTE *pData = pFullData+2;

	BYTE byCheckSum = pFullData[dwFullLen-1];

	BYTE byCalcSum = 0;
	// calc checksum and copy data
	for (DWORD i=0;i<dwFullLen-1;i++) {
		byCalcSum = (BYTE)(byCalcSum + pFullData[i]);
	}
	if (byCalcSum!=byCheckSum) {
		WK_TRACE_ERROR(_T("CheckSum error expected %x found %x\n"),
			byCalcSum, byCheckSum);
		CISDNConnection::HexDump(pFullData,dwFullLen,_T("error") );
		DoGrabFrame(FALSE);
		return;	// <<< EXIT >>>
	}


	switch (byCmd) {
		case 0xc0: // BoxInfo
			HandlePTBoxInfo(pData,dwDataLen);
		break;	// end of BoxConfig
		case 0xc6: // BoxInfo
			HandlePTBoxConfig(pData,dwDataLen);
		break;	// end of BoxConfig

		case 0xc4:
			WK_TRACE(_T("Received camState %d bytes data\n"),dwDataLen);
			// TESTHACK
			// SendPTCmd(0x9B,0,NULL);	// SetClock
			break;
		case ACK_REMOTE_OUT:
			HandlePTRemoteOut(pData,dwDataLen);
			break;
		case ACK_REMOTE_IN:
			HandlePTRemoteIn(pData,dwDataLen);
			break;
		case 0xD3:
			// picture information
			m_byPictureType = pData[0];
			if (m_byPictureType!=0x20
				&& m_byPictureType!=0x60) {
				WK_TRACE_ERROR(_T("Unsupported PictureType\n"));
			}
	
			break;
		case 0xDB: // Clock reply, BCD codiert
			if (dwDataLen==6) {
				int iSeconds = DecodeBCD(*pData++);
				int iMinutes = DecodeBCD(*pData++);
				int iHour = DecodeBCD(*pData++);
				int iDay= DecodeBCD(*pData++);
				int iMonth= DecodeBCD(*pData++);
				int iYear = DecodeBCD(*pData++)+1980;

				WK_TRACE(_T("Received Time %2d:%02d:%02d, %d.%d %d\n"),
					iHour,iMinutes,iSeconds,iDay,iMonth,iYear);

			} else {
				WK_TRACE_ERROR(_T("Invalid time reply len %d\n"),dwDataLen);
			}
			break;
		case ACK_CLEAR_ALARM:	// 0xE0
			if (dwDataLen) 
			{
				CISDNConnection::HexDump(pData,dwDataLen,_T("ACK_CLEAR_ALARM"));
			} 
			break;
		case ACK_GET_ALARM_STATE: // 0xE4
			// also first cmd for alarm calls
			WK_TRACE(_T("Got alarm state in PTState %s\n"),(const char *)NameOfEnum(GetPTConnectionState()));
			if (GetPTConnectionState()==PCS_CALLING
				|| GetPTConnectionState()==PCS_CONNECTED
				|| GetPTConnectionState()==PCS_INITIALIZING) {
				HandleAlarmState(pData,dwDataLen);
			} else {
				// OOPS 14.1.99 ever called?
				WK_TRACE(_T("Received alarm call!!!\n"));
				
				// CAVEAT important to reset internal data
				SetPTConnectionState(PCS_CALLING);	// OOPS

				ClearInternalData();

				SendPTConfig();
				// NOT YET alarm pipes
			}
			break;
		case ACK_CHECK_BOX_PWD:	// 0xE8
			WK_TRACE(_T("ACK_CHECK_BOX_PWD len %d\n"),dwDataLen);
			break;
		case ACK_SET_BOX_CONFIG:
			if (dwDataLen) 
			{
				CISDNConnection::HexDump(pData,dwDataLen,_T("ACK_SET_BOX_CONFIG"));
			}
			break;
		case ACK_GET_CAMERA:
			if (dwDataLen) 
			{
				TCMDSetCamera ackcamera;
				ZeroMemory(&ackcamera,sizeof(ackcamera));
				CopyMemory(&ackcamera,pData,dwDataLen);
				OnAcknowledgeCamera(ackcamera);
			} 
			break;
		case 0xFE:
			{
				WK_TRACE(_T("NACK %d (outstanding %d)\n"),dwDataLen,GetNumOutstandingCmds());
				// TESTHACK
				Sleep(50);
				// NOT YET retry last command, needs a cmdSendStack and more
				if (m_bDisableFrameGrabbing==FALSE) {
					// TESTHACK get more pictures
					DoGrabFrame(FALSE);
				}

			}
		break;
		case 0xFF:
			HandlePTError(pData,dwDataLen);
			break;
		default:
			WK_TRACE(_T("Received cmd 0x%02x len %d\n"),byCmd,dwDataLen);
	}

}

static CRawDataArray pictureCollection;

void CPTBox::OnPTFrame(const BYTE *pFrameData, DWORD dwFrameLen)
{
	if (m_bDoTraceFrameSize) {
		WK_STAT_PEAK(_T("Reset"),dwFrameLen,_T("FrameSize"));
	}

	m_NumOutstandingCmds--;
	if (m_NumOutstandingCmds<0) {
		m_NumOutstandingCmds=0;
	}

	// already ask for next frame before data is received
	if (m_bDisableFrameGrabbing==FALSE) {
		DoGrabFrame(FALSE);
	}

	DecodePictureTime(pFrameData,dwFrameLen);
	
	if (m_wPicTimeYear<1997 || m_wPicTimeYear>2036) {
		WK_TRACE_ERROR(_T("Invalid year %d\n"),m_wPicTimeYear);
		m_wPicTimeYear = (WORD) CTime::GetCurrentTime().GetYear();
	}
	if (m_byPicTimeMonth<1|| m_byPicTimeMonth>12) {
		WK_TRACE_ERROR(_T("Invalid month %d\n"),m_byPicTimeMonth);
		m_byPicTimeMonth= (BYTE) CTime::GetCurrentTime().GetMonth();
	}
	if (m_byPicTimeDay <1|| m_byPicTimeDay >31) {
		WK_TRACE_ERROR(_T("Invalid day %d\n"),m_byPicTimeDay);
		m_byPicTimeDay = (BYTE) CTime::GetCurrentTime().GetDay();
	}
	if (m_byPicTimeHour >31) {
		WK_TRACE_ERROR(_T("Invalid hour %d\n"),m_byPicTimeHour);
		m_byPicTimeHour = (BYTE) CTime::GetCurrentTime().GetHour();
	}
	if (m_byPicTimeMinute>59) {
		WK_TRACE_ERROR(_T("Invalid minute%d\n"),m_byPicTimeMinute);
		m_byPicTimeMinute = (BYTE) CTime::GetCurrentTime().GetMinute();
	}
	if (m_byPicTimeSecond>59) {
		WK_TRACE_ERROR(_T("Invalid second %d\n"),m_byPicTimeSecond);
		m_byPicTimeSecond = (BYTE) CTime::GetCurrentTime().GetSecond();
	}

	CSystemTime picTime;

	picTime.wYear = m_wPicTimeYear;
	picTime.wMonth =	m_byPicTimeMonth;
	picTime.wDay = m_byPicTimeDay;
	picTime.wHour = m_byPicTimeHour;
	picTime.wMinute = m_byPicTimeMinute;
	picTime.wSecond = m_byPicTimeSecond;

	m_iFrameCount++;

	if (GetPTConnectionState()==PCS_CONNECTED && m_pOutputPT
		&& m_pOutputPT->GetCIPC()->GetIPCState()==CIPC_STATE_CONNECTED) {

		WORD wCamID = (WORD)( (pFrameData[3] & (16|32))>>4 );
		if (wCamID>2) {	// OOPS hardcoded
			WK_TRACE(_T("CamID error %d not within 0..2\n"),wCamID);
			wCamID=0;	// OOPS
		}

		CIPCSavePictureType picType = SPT_FULL_PICTURE;
		if (m_byPictureType==0x20) {
			picType = SPT_FULL_PICTURE;
			if (m_bDoTraceFrameSize) {
				WK_STAT_PEAK(_T("Reset"),2,_T("FrameType"));
			}
		} else if (m_byPictureType==0x60) {
			picType = SPT_DIFF_PICTURE;
			if (m_bDoTraceFrameSize) {
				WK_STAT_PEAK(_T("Reset"),1,_T("FrameType"));
			}

		} else {
			WK_TRACE_ERROR(_T("Invalid picture format %x\n"),(int)m_byPictureType);
		}


		Resolution res = RESOLUTION_CIF;
		CSize frameSize;
		if (GetPTFrameSize(pFrameData, dwFrameLen, frameSize)==FALSE) {
			WK_TRACE_ERROR(_T("Invalid picture size\n"));
		} else {
			if (frameSize.cx < 100 ) {
				res = RESOLUTION_QCIF;
			} else if (frameSize.cx < 200) {
				res = RESOLUTION_CIF;
			} else {
				res = RESOLUTION_2CIF;
			}
			// WK_TRACE(_T("FrameSize %d / %d\n"),frameSize.cx,frameSize.cy);
		}

		if (m_pOutputPT) {
			CIPCPictureRecord pict(m_pOutputPT->GetCIPC(),
							pFrameData, dwFrameLen,
							CSecID(SECID_GROUP_OUTPUT,wCamID),
							res,
							COMPRESSION_11,
							COMPRESSION_PRESENCE,	// compression type
							picTime,
							0,	// dwJobTime,
							NULL,	// const char *szInfoString=NULL,
							0,	// DWORD dwBitrate=0,
							picType,
							0	// blockNr
							);
			m_pOutputPT->DoConfirmEncodedVideo(pict, 0, SECID_NO_ID);
		}
	} else {
		// no need for a bubble
	}
}

void CPTBox::SendPTConfig()
{
	// initial cmd password etc
	m_iFrameCount = 0;
	ClearInternalData();
	SetPTConnectionState(PCS_INITIALIZING);

	// NOT YET password from permission
	// NOT YET special config password
	// OOPS dunno how to crypt
	// CMDCheckBoxPassword (0A8h)
	// Reply: - oder BYTE MsgWrongXPassword falls eines der Passwoerter nicht ok ist.
	BYTE initData[]={
		0,0,0,0,0,0,0,0,0,	// 9, STRING[8] + len
		0,0,0,0,0,0,0,0,0,	// 9, STRING[8] + len
		0x14,0x03,			// 2
		0x01,0x00			// 2
	};	
	SendPTCmd(CMD_CHECK_BOX_PWD,9+9+2+2,initData);

	// request info
	BYTE infoData[]={0,0,0,0};
	SendPTCmd(CMD_GET_BOX_INFO,4,infoData);

	BYTE configData[4];
	
	configData[0]= 0;
	configData[1]= theInitUserData;	// user data (hostID) 
	configData[2]= 0;	// len==0, no data but request min/max index 
	configData[3]= DS_CAMERAS;	
	SendPTCmd(CMD_GET_BOX_CONFIG,4,configData);	// CAVEAT order is important for m_cameras

	configData[3]= DS_CAMERA_STATE;
	SendPTCmd(CMD_GET_BOX_CONFIG,4,configData);	// CAVEAT order is important for m_cameras

	configData[3]= DS_ALARM_LINES;
	SendPTCmd(CMD_GET_BOX_CONFIG,4,configData);

	configData[3]= DS_ALARM_ACTIONS;
	SendPTCmd(CMD_GET_BOX_CONFIG,4,configData);

	// relais Zustand abfragen
	BYTE relaisData[]={ 
		CHANNEL_IO_PORT,
		4,	// len
		IOMODE_GET,
		LOBYTE(PORT_RELAIS),
		HIBYTE(PORT_RELAIS),
		0
	};
	SendPTCmd(CMD_REMOTE_IN,6,relaisData);

#if 0
	// OOPS
	BYTE alarmClearData[]={ 7,2 };	// 7, alarmLines 2==Scharf
	SendPTCmd(CMD_CLEAR_ALARM,2,alarmClearData);
#endif


#if 1
	//	Flags (Bitmaske):
	// Bit 0: 1=Automatisch bei Aenderung des Alarmzustandes einen Update (ACKGetAlarmState) an den Host senden.
	// Bit 1: 0
	// Bit 2: 1=Aktuellen Zustand abfragen; 0=Nur Scharf/Unscharf setzen.
	BYTE alarmStateData[]={ (1|4) };
	SendPTCmd(CMD_GET_ALARM_STATE,1,alarmStateData);
#endif


}

void CPTBox::HandlePTBoxInfo(const BYTE *pData,DWORD dwDataLen)
{
	int i=0;
	int iLen=0;

	m_sVersion.Empty();
	m_sName.Empty();
	m_sNumber.Empty();

	iLen = *pData++;
	for (i=0;i<24;i++) 
	{
		if (i<iLen) 
		{
			m_sVersion += (_TCHAR)*pData;
		} 
		else 
		{
			// skip unused chars
		}
		pData++;
	}
	iLen = *pData++;
	for (i=0;i<20;i++) 
	{
		if (i<iLen) 
		{
			m_sName+= (_TCHAR)*pData;
		} 
		else 
		{
			// skip unused chars
		}
		pData++;
	}

	iLen = *pData++;
	for (i=0;i<15;i++) 
	{
		if (i<iLen) 
		{
			m_sNumber+= (_TCHAR)*pData;
		} 
		else 
		{
			// skip unused chars
		}
		pData++;
	}

	WK_TRACE(_T("PTConfig Version %s, Name %s, Number %s\n"),
		LPCTSTR(m_sVersion),
		LPCTSTR(m_sName),
		LPCTSTR(m_sNumber)
		);

	m_iVersion=100;	// default pt-100
	if (m_sVersion.GetLength()>2 && m_sVersion[1]==_T('.')) {
		// .XX is ignored
		// NOT YET does the software version make any difference
		if (m_sVersion[0]==_T('1')) {
			m_iVersion=100;
		} else if (m_sVersion[0]==_T('2')) {
			m_iVersion=200;
		} else {
			m_iVersion=100;
		}
	}
	WK_TRACE(_T("Version %d\n"),m_iVersion);
#if 1
	if (GetPTConnectionState() == PCS_INITIALIZING) {
		// OOPS hardcoded to input first
		if (m_pInputPT) {

			CIPCType cipcRequestType = CIPC_INPUT_SERVER;
			CIPCFetchResult remoteFetchResult(
					_T("remote PT"),	// hidden remote hostname
					CIPC_ERROR_OKAY, 0,	
					_T(""),
					0,
					SECID_NO_ID
#ifdef _UNICODE
					, 0 //we do not know, which codepage PT supports
#endif
			);

			if (m_pConnection
				&& m_pConnection->GetCallState()==CALL_ACTIVE) {
				// alarm pipes do not need a confirm
				// HandlePTAlarmCall has already done that
				m_pConnection->OnCipcPipeConfirm(cipcRequestType,
										m_pInputPT->GetID(),
										CSecID(SECID_NO_ID),
										CSecID(SECID_NO_ID),
										CSecID(SECID_NO_ID),
										remoteFetchResult
										);
			}
		}
	} else {
		// not in PCS_INITIALIZING
		// NOT YET
	}
#endif

}
void CPTBox::HandlePTBoxConfig(const BYTE *pData,DWORD dwDataLen)
{
	BYTE configData[4];
	
	configData[0]= 0;		// index, set below
	configData[1]= 0xbb;	// user data
	configData[2]= 0;		// record len, set below
	configData[3]= 0;		// DsID, set below


	int ix = pData[0];
	BYTE byHostId = pData[1];
	// UNUSED int iLen = pData[2];
	int dsId = pData[3];
	const BYTE *pConfigData = pData+4;
	int i=0;

	switch(dsId) {
		case DS_BOX_INFO:
			WK_TRACE(_T("BoxInfo\n"));
			break;
		case DS_ALARM_LINES:
			// conf000:  01 ab 31 08 09 f0 14       |..1.... |
			// STRUCT (BYTE MinIndex, HostId, EntrySize, DSId,MaxIndex, WORD Ptr)
			if (byHostId==theInitUserData) {
				// request the size of the structures
				int iMinIndex = pData[0];
				int iRecordSize = pData[2];
				int iMaxIndex = pData[4];
				WK_TRACE(_T("BoxConfig AlarmInfo %d..%d len %d\n"),iMinIndex,iMaxIndex,iRecordSize);
				for (ix = iMinIndex; ix <= iMaxIndex;ix++) {
					// aus der Mail PORT1:<-- 86: 01 00 10 09 
					configData[0]= (BYTE)ix;		// index, set below
					configData[1]= 0;	// user data
					configData[2]= (BYTE)iRecordSize;		// record len, set below
					configData[3]= DS_ALARM_LINES;
					SendPTCmd(CMD_GET_BOX_CONFIG,4,configData);

					CPTAlarm *pNewAlarm = new CPTAlarm ((WORD)ix);
					m_alarms.Add(pNewAlarm);
				}
			} else {
				// BYTE Kameras;
				// BYTE TriesNAction;
				// BYTE Flags,
				// BYTE NameLen;
				// BYTE Name[12],
				// DWORD AutoClearTime
				// CISDNConnection::HexDump(pConfigData,dwDataLen,_T("AlarmInfo"));	// OBSOLETE

				BYTE byCams = pConfigData[0];
				BYTE byTries = pConfigData[1];
				BYTE byFlags = pConfigData[2];

				CString sAlarmName;
				int iNameLen = pConfigData[3];
				for (i=0;i<iNameLen;i++) 
				{
					sAlarmName += (_TCHAR)pConfigData[4+i];
				}
				DWORD dwAutoClearTime = 
					  pConfigData[3+12] 
					| (pConfigData[3+12+1] << 8)
					| (pConfigData[3+12+2] << 16)
					| (pConfigData[3+12+3] << 24);

				WK_TRACE(_T("Alarm[%d] named '%s' cams %d tries %d flags %d autoClearTime %d\n"),
					ix,
					LPCTSTR(sAlarmName),
					byCams,
					byTries,
					byFlags,
					dwAutoClearTime
					);
				if (ix<m_alarms.GetSize()) {
					m_alarms[ix]->m_sName = sAlarmName;
					// NOT YET state
				} else {
					WK_TRACE(_T(":Invalid alarm index %d not within 0..%d\n"),
						ix,m_alarms.GetSize()
						);
				}

				if (ix == m_alarms.GetSize()-1) {	// last alarm is lasrt config data?
					SetPTConnectionState(PCS_CONNECTED);	// OOPS wrong place
					
					if (m_pConnection
						&& m_pConnection->GetCallState()==CALL_PASSIVE) {
						m_pConnection->HandlePTAlarmCall();	// OOPS here ?
					}
				}


			}
			break;
		case DS_CAMERAS:	// CAVEAT this one is requested first
			if (byHostId==theInitUserData) {
				// request the size of the structures
				int iMinIndex = pData[0];
				int iRecordSize = pData[2];
				int iMaxIndex = pData[4];
				WK_TRACE(_T("BoxConfig Cameras %d..%d len %d\n"),iMinIndex,iMaxIndex,iRecordSize);
				// NOT YET
				for (ix = iMinIndex; ix <= iMaxIndex;ix++) {
					configData[0]= (BYTE)ix;		// index, set below
					configData[1]= 0;	// user data
					configData[2]= (BYTE)iRecordSize;		// record len, set below
					configData[3]= DS_CAMERAS;
					SendPTCmd(CMD_GET_BOX_CONFIG,4,configData);

					CPTCamera *pNewCamera = new CPTCamera((WORD)ix);
					m_cameras.Add(pNewCamera);
				}

			} else {
				CString sMsg;
				sMsg.Format(_T("CameraInfo[%d]"),ix);
				// CISDNConnection::HexDump(pConfigData,iLen,sMsg);	// OBSOLETE
				//	WORD XSize, YSize;
				//	BYTE Saturation; /* 0..255 */
				//	BYTE MaxGrey;    /* Grau: 15, 31, 63, Farbe: 31+128, 63+128 */
				//	BYTE Contrast;   /* 0..255 */
				//	BYTE Brightness; /* 0..255 */
				//	BYTE Sensitivity; /* Min.Pixelaenderungen pro 8x8 Block */
				//	BYTE Threshold;   /* Min.Helligkeitsabweichung pro Pixel */
				WORD wXSize = (WORD)(pConfigData[0]+(pConfigData[1]<<8));
				WORD wYSize = (WORD)(pConfigData[2]+(pConfigData[3]<<8));
				CString sCameraName;
				int iNameLen = pConfigData[0x24];
				for (i=0;i<iNameLen;i++) 
				{
					sCameraName += (_TCHAR)pConfigData[i+0x25];
				}
				if (ix<m_cameras.GetSize())
				{
					CPTCamera &oneCam = *m_cameras[ix];
					oneCam.m_sName = sCameraName;
					oneCam.m_wSizeX= wXSize;
					oneCam.m_wSizeY= wYSize;
				}

				WK_TRACE(_T("Camera[%d] named '%s' dim %d/%d\n"),ix,
					LPCTSTR(sCameraName),
					wXSize,wYSize
					);
			}
			break;
		case DS_CAMERA_STATE:
			if (byHostId==theInitUserData) {
				// request the size of the structures
				int iMinIndex = pData[0];
				int iRecordSize = pData[2];
				int iMaxIndex = pData[4];
				// NOT YET
				for (ix = iMinIndex; ix <= iMaxIndex;ix++) {
					configData[0]= (BYTE)ix;		// index
					configData[1]= 0;	// user data
					configData[2]= (BYTE)iRecordSize;		// record len
					configData[3]= DS_CAMERA_STATE;
					SendPTCmd(CMD_GET_BOX_CONFIG,4,configData);
				}
			} else {
				// BYTE Active; 0xFF=Kamera angeschlossen & aktiv, 0x00 Kein Videosignal
				// BYTE Flags;   CS_xxx NOT YET
				BOOL bHasCamera = pConfigData[0]==(BYTE)0xff;
				WK_TRACE(_T("CameraState[%d] hasCam %d\n"),ix,bHasCamera);

				if (ix<m_cameras.GetSize()) {
					m_cameras[ix]->m_byState = pConfigData[0];
				}
			}
			break;
		case DS_ALARM_ACTIONS:
			if (byHostId==theInitUserData) {
				// request the size of the structures
				int iMinIndex = pData[0];
				int iRecordSize = pData[2];
				int iMaxIndex = pData[4];
				// NOT YET
				for (ix = iMinIndex; ix <= iMaxIndex;ix++) {
					configData[0]= (BYTE)ix;		// index
					configData[1]= 0;	// user data
					configData[2]= (BYTE)iRecordSize;		// record len
					configData[3]= DS_ALARM_ACTIONS;
					SendPTCmd(CMD_GET_BOX_CONFIG,4,configData);
				}

			} 
			else
			{
				// BOOL bHasCamera = pConfigData[0]==(BYTE)0xff;
				CString sPre;
				sPre.Format(_T("Action[%d]"),ix);

				CString sTel;
				BYTE byTelLen = pConfigData[0];
				for (int i=0;i<byTelLen && i < 20;i++) 
				{
					sTel += (_TCHAR)pConfigData[1+i];
				}

				BYTE byType = pConfigData[1+20+0];
				BYTE byFail = pConfigData[1+20+1];
				BYTE byRetryDelay = pConfigData[1+20+2];

				// UNUSED WORD wFromTime=0;	// NOT YET
				// UNUSED WORD wToTime=0;	// NOT YET
				CString sParams;	// NOT YET char[20]

				WK_TRACE(_T("Action[%d]: Tel '%s', type %x fail %x retry %x\n"),
					ix, LPCTSTR(sTel),
					byType,byFail,byRetryDelay
					);
				// CISDNConnection::HexDump(pConfigData,dwDataLen,sPre);
			}
			break;
		default:
			WK_TRACE_ERROR(_T("Unsupported box config type %d\n"),dsId);
	}

}

void CPTBox::HandleAlarmState(const BYTE *pData,DWORD dwDataLen)
{
	// CISDNConnection::HexDump(pData,dwDataLen,_T("alarm"));
	// STRUCT (BYTE LatchedAlarmInputs, CurrentAlarmInputs, 
	//				ControlInfo, InvolvedKameras,
    //		{WORD ErrorType, ErrorParam, ErrorPC}
	//	) ;

	BYTE byLatchedAlarms = pData[0];
	BYTE byCurrentAlarmInputs = pData[1];
	BYTE byControlInfo = pData[2];
	BYTE byCams = pData[3];

	WK_TRACE(_T("Got AlarmState latchedAlarms %d current %d control %d cams %d\n"),
		byLatchedAlarms, byCurrentAlarmInputs,
		byControlInfo,
		byCams
		);

	if (GetPTConnectionState()!=PCS_CONNECTED && byCams!=0) {
		// exclusive if, only ONE CAM is supported
		// that mean, if multiple cams are given, the first one if taken
		// the 'old' (pre 4.0) default of cam0 is still active
		WORD wSubID = 0;
		if (byCams & 1) {
			wSubID = 0;
		} else if (byCams & 2) {
			wSubID = 1;
		} else if (byCams & 4) {
			wSubID = 2;
		} else {
			// no cam at all, but always select a cam
			wSubID = 0;
		}
		WK_TRACE(_T("Got alarm cam ID %d\n"),wSubID);
		m_alarmCamID.Set( SECID_GROUP_OUTPUT, wSubID);
	}	// end of alarm cam id setting

	/*
	LatchedAlarmInputs entspricht der Anzeige der roten LEDs auf der Frontplatte.
	Der Alarmspeicher muss durch CMDClearAlarm geloescht werden.

	CurrentAlarmInputs zeigt an, ob der Melder an der Linie wieder deaktiviert ist
	(Eingang ohne Speicherfunktion).

	ControlInfo (Bitmaske):
	Bit 0: 1=Scharf geschaltet
	Bit 1: 1=Alarm (Relais ist an) liegt vor
	Bit 2: 1=Stoerung/Fehler liegt an.
			 Die Felder ErrorType, ErrorParam, ErrorPC werden uebermittelt.

	InvolvedKameras enthaelt die Bitmaske aller der bei diesem Alarm involvierten
	Kameras. Dies ist eine Oder-Verkuepfung des Feldes "Zugerodnete Kameras" aller
	aktiven Alarmlinien.
	*/

	// Test mit Alarm2 ergibt:
	// Got AlarmState latchedAlarms 2 current 2 control 3 cams 0
	// Got AlarmState latchedAlarms 0 current 0 control 1 cams 0
	if ((byControlInfo & 4)==0) {
		// OOPS brute force attack
		DWORD dwInputMask=0;
		DWORD dwChangeMask=0;
		for (int a=0;a<m_alarms.GetSize();a++) {
			BYTE byOneBit = (BYTE)(1 << a);
			// BOOL bActive = (byCurrentAlarmInputs & byOneBit)!=0;
			BOOL bActive = (byLatchedAlarms & byOneBit)!=0;
			if (bActive != m_alarms[a]->IsActive()) {
				// state changed
				m_alarms[a]->m_bIsActive =bActive;
				dwChangeMask |= (1 << (a+1));
			}
			// build bitmask on the fly/in the loops
			if (m_alarms[a]->IsActive()) {
				dwInputMask |= (1 << (a+1));
			}
		}

		// inform client
		if (dwChangeMask != 0 && m_pInputPT) {
			if (m_pInputPT && m_pInputPT->GetIPCState()==CIPC_STATE_CONNECTED) {
				m_pInputPT->DoIndicateAlarmState(m_alarms[0]->GetID().GetGroupID(),
					  dwInputMask,  // 1 high, 0 low
					  dwChangeMask	// 1 changed, 0 unchanged
					  );
			}
		}
	} else {
		// some error occured
		WK_TRACE(_T("Some error occured in AlarmState indication !?\n"));
		// NOT YET ask for current state to sync internals with box
		// NOT YET read     {WORD ErrorType, ErrorParam, ErrorPC}
	}
}

#define READ_TIME(x) \
	x = *pTime++; \
	bHasFollowUps = ((x & 0x80)!=0); \
	x &= 0x7f; \
	x = DecodeBCD(x); \
	if (bHasFollowUps==FALSE) { \
		return; \
	}

void CPTBox::DecodePictureTime(const BYTE *pFrameData, DWORD dwFrameDataLen)
{
	// skip SOP XBlocks YBlocks GrayAndCameras
	const BYTE *pTime = pFrameData+4;
	// NOT YET +1 for FrameCount in DCT format
	BOOL bHasFollowUps=FALSE;

	READ_TIME(m_byPicTimeSecond);
	READ_TIME(m_byPicTimeMinute);
	READ_TIME(m_byPicTimeHour);
	READ_TIME(m_byPicTimeDay);
	READ_TIME(m_byPicTimeMonth);

	WORD wPicTimeYear = (WORD) ( ((*pTime++) & 0x7F) + 0x80);
	if (wPicTimeYear >= 0xA0) {
		wPicTimeYear += 0x1F60; /* BCD --> 2000 */
	} else {
		wPicTimeYear += 0x1900;
	}
	m_wPicTimeYear = (WORD) (
		DecodeBCD( (BYTE)((wPicTimeYear & 0xff00)>>8) )*100
		+ DecodeBCD( (BYTE)(wPicTimeYear & 0x00ff) )
		);

}

void CPTBox::DoSelectCamera(CSecID camID, BOOL bActive, Resolution res)
{
	// NOT YET some validation
	CSecID lastCamID = m_requestedCamID;

	if (bActive) {
		m_requestedCamID = camID;
	} else {
		m_requestedCamID = SECID_NO_ID;
	}

	if (res==RESOLUTION_QCIF && m_bLowAsMid) {
		res = RESOLUTION_CIF;
	}

	m_requestedResolution = res;


	if (lastCamID == SECID_NO_ID) {
		// no grabbing in action
		if (bActive) {
			if (m_bDisableFrameGrabbing==FALSE) {
				WK_TRACE(_T("Start grabbing %x\n"),m_requestedCamID.GetID());
				DoGrabFrame(TRUE);	// force IntraFrame
			} else {
				// disabled
			}
		} else {
			// stop the stopped grabbing, so no need to do anything
		}
	} else {
		// already grabbing
		// next DoGrabFrame will notice, either switch or stop
		// make sure to to get an 
		m_bRequestForceIntra = TRUE;
	}
}
///////////////////////////////////////////////////////////////////////////
void CPTBox::DoGrabFrame(BOOL bForceIntra)
{
	// no switch to the requested camera
	// this way incoming pictures still have the correct id
	// and the next picture will have the new id
	CSecID requestID(m_requestedCamID);

	if (m_requestedCamID==SECID_NO_ID) {
		// stop grabbing
		WK_TRACE(_T("Grabbing stopped\n"));	// OBSOLETE
		return;
	}

	CPTCamera *pActiveCam = m_cameras.GetCameraByID(m_requestedCamID);
	if (pActiveCam && m_requestedResolution!=pActiveCam->GetResolution()
		|| m_bRequestForceIntra) 
	{
		// modify resolution in the box
		DoSetResolution(m_requestedCamID, m_requestedResolution, FALSE);	// not permanent
		bForceIntra=TRUE;
	}
	m_bRequestForceIntra = FALSE;	// reset

	TCMDGrabFrame grab;
	// start auto grabbing until stop
	grab.Control = 0x10;
	if (bForceIntra) 
	{
		grab.Control = (BYTE) (grab.Control | 0x01);
	}
	grab.VideoSource = (BYTE) m_requestedCamID.GetSubID();	// OOPS no range check

	SendPTCmd(CMD_GRAB_FRAME,sizeof(grab),(BYTE*)&grab);
}
///////////////////////////////////////////////////////////////////////////
void CPTBox::HandlePTError(const BYTE *pData,DWORD dwDataLen)
{
	// STRUCT (WORD ErrorNo, ErrorParam).
	WORD wErrorNumber = (WORD) ( (((WORD)pData[1])<<8) | pData[0] );
	WORD wErrorParam= (WORD) ( (((WORD)pData[3])<<8) | pData[2] );

	WK_TRACE(_T("Error code %x param %x\n"),wErrorNumber,wErrorParam);
	switch (wErrorNumber) 
	{
	case 0x350e:	// kein VideoSignal
		{
			WORD wCamNumber = (WORD)(wErrorParam & 7);
			// OOPS camId in error data ?
			WK_TRACE_ERROR(_T("Kein Videosignal auf Kamera %d\n"),wCamNumber);
			// a picture with width/height 1/1
			// is a special picture
			BYTE fakeData[] = {
				0xFE,	// SOP 
				1,1,	// x/y blocks faked --> testpicture
				0,	// 128, wenn mit Uhrzzeit
				// NOT YET CurrentTime
				0xFF, 0xFF
			};	// end of fake data
			DWORD dwFakeDataLen = 1 + 2 + 1 +2;
			if (m_pOutputPT) {
				CSystemTime st;
				st.GetLocalTime();
				CIPCPictureRecord pict(
						m_pOutputPT->GetCIPC(),
						fakeData, dwFakeDataLen,
						CSecID(SECID_GROUP_OUTPUT,wCamNumber),
						RESOLUTION_CIF,
						COMPRESSION_11,
						COMPRESSION_PRESENCE,
						st,	// OOPS
						0,	// dwJobTime,
						NULL,	// const char *szInfoString=NULL,
						0,	// DWORD dwBitrate=0,
						SPT_FULL_PICTURE,
						0	// blockNr
						);
				m_pOutputPT->DoConfirmEncodedVideo(pict, 0, SECID_NO_ID);
			}
			m_requestedCamID = SECID_NO_ID;	// OOPS
		}
		break;
	}
}
///////////////////////////////////////////////////////////////////////////
void CPTBox::HandlePTRemoteIn(const BYTE *pData,DWORD dwDataLen)
{
	// n*{ BYTE Channel, BYTE Len, ARRAY[Len] BYTE IOResult }
    // Falls ReplyLen=1 ist, wird BYTE MsgByte mit dem Fehlergrund zurueckgeliefert.
	// relais000:  08 04 /00 ca 00 f3/         |......  |
	// Channel_IOPORT   = 8
	
	DWORD ix = 0;
	while (ix<dwDataLen) {
		BYTE byChannel=pData[ix];
		BYTE byLen = pData[ix+1];
		ix += 2;
		switch (byChannel) {
			case 8:	// relais
				{
				// STRUCT (BYTE Mode, WORD IOAddr, BYTE Mask/Bit) IOParam
				BYTE byMode = pData[ix];
				WORD wIOAddress = (WORD)(pData[ix+1] | (pData[ix+2]<<8));
				DWORD dwMask = pData[ix+3];
				int maxBits=0;
				if (GetVersion()<200) {
					// pt-100 one relay, bit 1
					dwMask = ((dwMask & 0x02) >> 1);
					maxBits = 1;
				} else {
					// pt-200 three relays, bits 0..2
					dwMask = (dwMask & 7 );
					maxBits = 3;
				}
				for (int b=0;b<maxBits;b++) {
					BYTE oneBit = (BYTE)(1 << b);
					if ((dwMask & oneBit) != (m_dwRelayBits & oneBit)) {
						// relay state changed
						if (GetPTConnectionState()==PCS_CONNECTED) {
							// inform client
							BOOL bClosed = (dwMask & oneBit)!=0;
							if (m_pOutputPT) {
								m_pOutputPT->DoConfirmSetRelay(
									// OOPS 'hardcoded' relay offset
									CSecID(SECID_GROUP_OUTPUT,(WORD)(m_cameras.GetSize()+b)), 
									bClosed
									);
							}
						}
					}
				}
				m_dwRelayBits = dwMask;
				WK_TRACE(_T("Relais addr %04x mode %d mask %x\n"),wIOAddress,byMode,dwMask);
				}
			break;
			default: WK_TRACE_ERROR(_T("Unsupported channel %d in RemoteIn\n"),byChannel);
		}
		ix += byLen;	// skip to next block
	}
}
///////////////////////////////////////////////////////////////////////////
void CPTBox::HandlePTRemoteOut(const BYTE *pData,DWORD dwDataLen)
{
	if (dwDataLen==0) {
		// WK_TRACE(_T("RemoteOut okay\n"));
	} else {
		WK_TRACE_ERROR(_T("RemoteOut result %d\n"),pData[0]);
		// NOT YET any error recovery
		// could ask for the relais state again
	}
}
///////////////////////////////////////////////////////////////////////////
void CPTBox::DoSetRelay(CSecID relayID, BOOL bClosed)
{
	// m_dwRelayBits are NOT modified here, but in the RemoteIn 'ack'
	BYTE byRelayIndex = (BYTE)(relayID.GetSubID()-m_cameras.GetSize());	// OOPS relay offset

	BYTE byRelayBit;
	if (GetVersion()<200) {
		byRelayBit = (BYTE)(2 << byRelayIndex);	// bit 1
	} else {
		byRelayBit = (BYTE)(1 << (byRelayIndex));	// bit 0..2
	}

	BYTE relaisData[]={ 
		CHANNEL_IO_PORT,
		4,	// len
		IOMODE_OR,	// set below, depending from bClosed
		LOBYTE(PORT_RELAIS),
		HIBYTE(PORT_RELAIS),
		byRelayBit
	};

	if (bClosed) {
		relaisData[2] = IOMODE_OR;
	} else {
		relaisData[2] = IOMODE_XOR;
	}
	SendPTCmd(CMD_REMOTE_OUT,6,relaisData);

	// and ask for the current state to make sure the states are sychronized
	BYTE relaisQueryData[]={ 
		CHANNEL_IO_PORT,
		4,	// len
		IOMODE_GET,
		LOBYTE(PORT_RELAIS),
		HIBYTE(PORT_RELAIS),
		0
	};
	SendPTCmd(CMD_REMOTE_IN,6,relaisQueryData);
}
///////////////////////////////////////////////////////////////////////////
void CPTBox::DoConfirmAlarm(CSecID inputID)
{
	BYTE alarmConfirmData[]=
	{ 
		(BYTE)inputID.GetSubID(),
		0x03,
		0,
		0,
	};
	SendPTCmd(CMD_CLEAR_ALARM,4,alarmConfirmData);
}
///////////////////////////////////////////////////////////////////////////
void CPTBox::DoSetTime(const CSystemTime& st)
{
	TCMDSetTime pttime;
	pttime.Sec = WORD2BCD(st.wSecond);
	pttime.Min = WORD2BCD(st.wMinute);
	pttime.Hour = WORD2BCD(st.wHour);
	pttime.Day = WORD2BCD(st.wDay);
	pttime.Month = WORD2BCD(st.wMonth);
	pttime.Year = WORD2BCD((WORD)(st.wYear - 1980));
	SendPTCmd(CMD_SET_CLOCK,sizeof(pttime),(BYTE*)&pttime);
}
///////////////////////////////////////////////////////////////////////////
void CPTBox::DoSetCameraName(int ix, const CString &sNewName)
{
	// NOT YET 'holes',second cam no signal, but third
	m_cameras[ix]->m_sName = sNewName;	// update internal 

	TCameraInfo info;

	int i=0;
	CString sName;
	for (i=0;i<sNewName.GetLength() && i<20;i++) {
		sName += sNewName[i];
	}

	WORD wOffset;
	wOffset = (WORD)(((BYTE*)&(info.m_iNameLen)) - ((BYTE *)&(info)));
	WK_TRACE(_T("NameLenOffset is %d 0x%x len %d\n"),wOffset,wOffset,ix);
	BYTE setCamData[2+2+1+20];	// wOffset 2, wordIX 2, len 1, char[20]

	for (i=0;i<2+2+1+20;i++) {
		setCamData[i]=0;
	}
	setCamData[0] = LOBYTE(wOffset);
	setCamData[1] = HIBYTE(wOffset);
	setCamData[2] = (BYTE)ix;
	setCamData[3] = 0;

	setCamData[4] = (BYTE)sName.GetLength(); // first data byte
	for (i=0;i<sName.GetLength();i++) 
	{
		setCamData[5+i] = (BYTE)sName[i];
	}

	SendPTCmd(CMD_SET_CAMERA,2+2+1+20,setCamData);
	// NOT YET update client
	// SendPTConfig();	// OOPS or something similar
}
///////////////////////////////////////////////////////////////////////////
void CPTBox::DoSetHostName(const CString &sNewName)
{
	int i=0;
	CString sName;
	for (i=0;i<sNewName.GetLength() && i<20;i++) {
		sName += sNewName[i];
	}

	// UNUSED WORD wOffset = 0;
	BYTE setNameData[3+1+20];
	for (i=0;i<3+1+20;i++) 
	{
		setNameData[i]=0;
	}
	// CMDSetBoxConfig (87h)
	// Input: STRUCT (BYTE Index, HostId, Len, DSId, ARRAY[...] BYTE Data) ;
	// OOPS dropped len

	setNameData[0] = 0;
	setNameData[1] = 0; 
	setNameData[2] = 0;

	setNameData[3] = (BYTE)sName.GetLength(); // first data byte
	for (i=0;i<sName.GetLength();i++) 
	{
		setNameData[4+i] = (BYTE)sName[i];
	}

	m_sName = sName;	// OOPS
	SendPTCmd(0x87,3+1+20,setNameData);
}
///////////////////////////////////////////////////////////////////////////
void CPTBox::DoWriteEEP()
{
	if (m_bDisableEEPWrite==FALSE) {
		WK_TRACE(_T("WriteEEP....\n"));
		SendPTCmd(0x99,	0,NULL);	// write EEProm
	}
}
///////////////////////////////////////////////////////////////////////////
void CPTBox::DoSetResolution(CSecID camID, Resolution res, BOOL bPermanent)
{

	CPTCamera *pCamera = m_cameras.GetCameraByID(camID);
	if (pCamera==NULL) {
		WK_TRACE_ERROR(_T("Invalid camID %x\n"),camID.GetID());
		return;	// <<< EXIT >>>
	}

	if (res==RESOLUTION_QCIF && m_bLowAsMid) {
		res = RESOLUTION_CIF;
	}
	// convert resolution to x/y size
	WORD wXSize;
	WORD wYSize;
	switch (res) {
		case RESOLUTION_QCIF:
			wXSize = 96;
			wYSize = 72;
			break;
		case RESOLUTION_CIF:
			wXSize = 192;
			wYSize = 144;
			break;
		case RESOLUTION_2CIF:
			wXSize = 384;
			wYSize = 288;
		break;
		default:
			wXSize = 384;
			wYSize = 288;
	}

	// update internal data, directly there is no explicit confirm
	pCamera->m_wSizeX = wXSize;
	pCamera->m_wSizeY = wYSize;
	WK_TRACE(_T("New resolution for %x is %d/%d\n"),
		pCamera->GetID(),
		wXSize, wYSize
		);

	// offset 0 WORD XSize;
	// offset 2	WORD YSize;
	BYTE setCamData[2+2+4];	// wOffset 2, wordIX 2, wXSize, wYSize

	int i;
	for (i=0;i<2+2+4;i++) {
		setCamData[i]=0;
	}
	WORD wOffset = 0;
	setCamData[0] = LOBYTE(wOffset);
	setCamData[1] = HIBYTE(wOffset);
	setCamData[2] = (BYTE)pCamera->GetID().GetSubID();	// OOPS, always in that order
	setCamData[3] = 0;

	setCamData[4] = LOBYTE(wXSize);
	setCamData[5] = HIBYTE(wXSize);
	setCamData[6] = LOBYTE(wYSize);
	setCamData[7] = HIBYTE(wYSize);

	SendPTCmd(0x8F,2+2+4,setCamData);

	if (bPermanent) {
		DoWriteEEP();
	}
}
///////////////////////////////////////////////////////////////////////////
void CPTBox::DoRequestCameraValue(CSecID camID, const CString &sKey)
{
	CPTCamera *pCamera = m_cameras.GetCameraByID(camID);
	if (pCamera==NULL) 
	{
		WK_TRACE_ERROR(_T("Invalid camID %x\n"),camID.GetID());
		return;	// <<< EXIT >>>
	}

	TCMDGetCamera getcamera;

	getcamera.size = 1;
	getcamera.camerano = camID.GetSubID();

	if (sKey == CSD_CONTRAST) 
	{
		getcamera.offset = 0x0006;
	} 
	else if (sKey == CSD_BRIGHTNESS) 
	{
		getcamera.offset = 0x0007;
	} 
	else if (sKey == CSD_SATURATION) 
	{
		getcamera.offset = 0x0004;
	} 
	else 
	{
		WK_TRACE_ERROR(_T("Invalid key %s\n"),LPCTSTR(sKey));
		return;	// <<< EXIT >>>
	}
	WK_TRACE(_T("requesting %s for %s\n"),sKey,pCamera->GetName());

	SendPTCmd(CMD_GET_CAMERA,sizeof(getcamera), (BYTE*)&getcamera);
}
///////////////////////////////////////////////////////////////////////////
void CPTBox::OnAcknowledgeCamera(const TCMDSetCamera& ackcamera)
{
	if (m_cameras.GetSize()==0)
	{
		// no cams
		return;
	}

	WORD wGroupID = m_cameras.GetAt(0)->GetID().GetGroupID();
	CSecID id(wGroupID,ackcamera.camerano);
	CString sFormatValue;

	switch (ackcamera.offset)
	{
	case 0x0006:
		// contrast
		WK_TRACE(_T("ACK CONTRAST %d cam %d \n"),ackcamera.data[0],ackcamera.camerano);
		if (   m_pOutputPT 
			&& (m_pOutputPT->GetIPCState()==CIPC_STATE_CONNECTED))
		{
			sFormatValue.Format(_T("%d"),ackcamera.data[0]*2);
			m_pOutputPT->DoConfirmGetValue(id,CSD_CONTRAST,sFormatValue,0);
		}
		break;
	case 0x0007:
		// brightness
		WK_TRACE(_T("ACK BRIGHTNESS %d cam %d \n"),ackcamera.data[0],ackcamera.camerano);
		if (   m_pOutputPT 
			&& (m_pOutputPT->GetIPCState()==CIPC_STATE_CONNECTED))
		{
			sFormatValue.Format(_T("%d"),ackcamera.data[0]);
			m_pOutputPT->DoConfirmGetValue(id,CSD_BRIGHTNESS,sFormatValue,0);
		}
		break;
	case 0x0004:
		// saturation
		WK_TRACE(_T("ACK SATURATION %d cam %d \n"),ackcamera.data[0],ackcamera.camerano);
		if (   m_pOutputPT 
			&& (m_pOutputPT->GetIPCState()==CIPC_STATE_CONNECTED))
		{
			sFormatValue.Format(_T("%d"),ackcamera.data[0]*2);
			m_pOutputPT->DoConfirmGetValue(id,CSD_SATURATION,sFormatValue,0);
		}
		break;
	default:
		WK_TRACE(_T("OnAcknowledgeCamera unknown offset %lx\n"),ackcamera.offset);
		break;
	}
}
///////////////////////////////////////////////////////////////////////////
void CPTBox::DoSetCameraValue(CSecID camID, const CString &sKey, const CString &sValue)
{
	CPTCamera *pCamera = m_cameras.GetCameraByID(camID);
	if (pCamera==NULL) 
	{
		WK_TRACE_ERROR(_T("Invalid camID %x\n"),camID.GetID());
		return;	// <<< EXIT >>>
	}

	int iValue = _ttoi(sValue);
	BYTE byValue=0;
	WORD wOffset;

	if (sKey == CSD_CONTRAST) 
	{
		// range is 0..511
		byValue = (BYTE) ( iValue/2);
		wOffset = 6;
	} 
	else if (sKey == CSD_BRIGHTNESS) 
	{
		// range is 0..255
		byValue = (BYTE) (iValue);
		wOffset = 7;
	} 
	else if (sKey == CSD_SATURATION) 
	{
		// range is 0..511
		byValue = (BYTE) ( iValue/2);
		wOffset = 4;
	} 
	else 
	{
		WK_TRACE_ERROR(_T("Invalid key %s\n"), LPCTSTR(sKey));
		return;	// <<< EXIT >>>
	}

	WK_TRACE(_T("New value cam %x %s: %d\n"),camID.GetID(), LPCTSTR(sKey), byValue);
	// offset 0 WORD XSize;
	// offset 2	WORD YSize;
	BYTE setCamData[2+2+1];	// wOffset 2, wordIX 2, byXXX

	ZeroMemory(&setCamData,sizeof(setCamData));

	setCamData[0] = LOBYTE(wOffset);
	setCamData[1] = HIBYTE(wOffset);
	setCamData[2] = (BYTE)pCamera->GetID().GetSubID();	// OOPS, always in that order
	setCamData[3] = 0;

	setCamData[4] = byValue;

	SendPTCmd(CMD_SET_CAMERA,2+2+1,setCamData);
}
///////////////////////////////////////////////////////////////////////////
LPCTSTR NameOfEnum(PTConnectionState s)
{
	switch (s) 
	{
	case PCS_NOT_CONNECTED: return _T("PCS_NOT_CONNECTED"); break;
	case PCS_CALLING: return _T("PCS_CALLING"); break;
	case PCS_INITIALIZING: return _T("PCS_INITIALIZING"); break;
	case PCS_CONNECTED: return _T("PCS_CONNECTED"); break;
	case PCS_CLOSING: return _T("PCS_CLOSING"); break;
	default:
		return _T("PCS_INVALID");

	};
}

