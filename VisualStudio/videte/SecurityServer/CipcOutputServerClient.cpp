/* GlobalReplace: GetTickCount --> WK_GetTickCount */
/* GlobalReplace: pApp\-\> --> theApp. */
/* GlobalReplace: Relais --> Relay */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcOutputServerClient.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/CipcOutputServerClient.cpp $
// CHECKIN:		$Date: 30.08.06 13:02 $
// VERSION:		$Revision: 262 $
// LAST CHANGE:	$Modtime: 30.08.06 12:56 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$


#include "stdafx.h"
#include "CipcOutputServerClient.h"	
#include "CipcServerControlServerSide.h"	
#include "CipcOutputServer.h"
#include "OutputList.h"
#include "sec3.h"
#include "ProcessScheduler.h"
#include "ProcessSchedulerVideo.h"
#include "TranscoderThread.h"
#include "CipcStringDefs.h"
#include "wkclasses\WK_RuntimeErrors.h" //for notifiable errors like CPU temperature too high


#define RTE_CONFIRMED		_T("/RTE Confirmed/")	

// OEMGUI
#include <oemgui\res\ApplicationDefines.h> //defines for maintenance nag screen & RTEs

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CIPCOutputClientsArray::CIPCOutputClientsArray()
{
#ifdef TRACE_LOCKS
	m_nCurrentThread = theApp.m_bTraceLocks ? 0 : -1;
#endif
}

CIPCOutputServerClient* CIPCOutputClientsArray::GetClientByID(CSecID id) const
{
	if (id != SECID_NO_ID)
	{
		for (int i=0;i<GetSize();i++) 
		{
			if (GetAtFast(i)->GetID()==id) 
			{
				return GetAtFast(i);	// EXIT
			}
		}
	}
	return NULL;
}
CIPCOutputServerClient* CIPCOutputClientsArray::GetClientBySubID(WORD wSubID) const
{
	if (wSubID != SECID_NO_SUBID)
	{
		for (int i=0;i<GetSize();i++) 
		{
			if (GetAtFast(i)->GetID().GetSubID() == wSubID) 
			{
				return GetAtFast(i);	// EXIT
			}
		}
	}
	return NULL;
}
BOOL CIPCOutputClientsArray::IsConnected(const CString& sSerial,const CString& sSourceHost)
{
	int i;
	BOOL bFound = FALSE;
	CIPCOutputServerClient* pOutputClient = NULL;

	Lock(_T(__FUNCTION__));

	for (i=0;i<GetSize();i++)
	{
		pOutputClient = GetAtFast(i);
		if (   pOutputClient
			&& pOutputClient->GetIPCState() == CIPC_STATE_CONNECTED
			&& pOutputClient->GetSerial()==sSerial
			&& sSourceHost != pOutputClient->GetRemoteHost())
		{
			bFound = TRUE;
			break;
		}
	}
	Unlock();

	return bFound;
}
int	 CIPCOutputClientsArray::GetNrOfLicensed()
{
	int r = 0;

	int i;
	CIPCOutputServerClient* pOutputClient = NULL;

	Lock(_T(__FUNCTION__));

	for (i=0;i<GetSize();i++)
	{
		pOutputClient = GetAtFast(i);
		if (   pOutputClient 
			&& !pOutputClient->GetSerial().IsEmpty()
			)
		{
			r++;
		}
	}
	Unlock();

	return r;
}


// NEW 260497 static CIPCOutputServerClient *pLastDisplayClient=NULL;
// NEW 260497 extern CIPCPictureRecord *CreateEmptyPicture(const CIPC *pCipc);
// NEW 120298 m_bTraceClientJpegRequests


BOOL CIPCOutputServerClient::m_bAllowAnyOverlayClient=FALSE;
BOOL CIPCOutputServerClient::m_bTraceClientJpegRequests=FALSE;
int  CIPCOutputServerClient::m_iCoCoClientPicsPerSecond=1;

/*@TOPIC{CIPCOutputServerClient Overview|CIPCOutputServerClient,Overview}
@RELATED @LINK{members|CIPCOutputServerClient},
*/
/*
@MLIST @RELATED @LINK{CIPCInputServerClient}, @LINK{CIPCOutput}
*/

/*@TOPIC{ClientOverlay}
@RELATED 
@LINK{CIPCOutputServerClient::OnRequestH263Decoding},
@LINK{CIPCOutputServerClient::OnSetDisplayWindow},
@LINK{CIPCOutputServerClient::OnSetOutputWindow},
@LINK{CIPCOutputServerClient::OnRequestLocalVideo},
@LINK{CIPCOutputServerClient::OnRequestOverlay}

Clients melden via OnRequestOverlay, ob sie aktiv/OnTop sind.
Der letzte aktive client erhaelt den Zuschlag fuer die
overlay/decompress resource der CoCo. Wenn ein overlay/decompress
request gemacht, wird der alte client via DoIndicataClientChanged
informiert. Dabei wird abgegeben ob es overlay CDR_OVERLAY oder
CDR_DECOMPRESS_H263 ist.

Zum Beispiel kann der DatabaseClient einfach mit dem naechsten
P-Bild weitermachen, wenn der Explorer nur den overlay benutzt hat.
Hat der Exploerer jedoch auch dekoprimiert, muss beim letzten I-Bild
weiter gemacht werden.
*/

/*@MHEAD{constructor:}*/
/*@MD NYD*/
CIPCOutputServerClient::CIPCOutputServerClient(const CConnectionRecord &c,
												CIPCServerControlServerSide *pControl,
												LPCTSTR shmName, 
												WORD wSubID,
												BOOL bAlarm,
												const CString& sRemoteName)
	: CIPCOutput(shmName,TRUE)	// base class constructor
{
	m_bMpeg4 = FALSE;
	m_bH263 = FALSE;
	m_bCanGOP = FALSE;
	m_pTranscoder = NULL;
	m_bTimeoutDisconnected = FALSE;
	
	c.GetFieldValue(CRF_SERIAL,m_sSerial);
	c.GetFieldValue(CRF_VERSION,m_sVersion);
	
	m_dwOptions = c.GetOptions();
	CString s;
	if (m_dwOptions & SCO_WANT_RELAYS)
	{
		s += _T("SCO_WANT_RELAYS|");
	}
	if (m_dwOptions & SCO_WANT_CAMERAS_BW)
	{
		s += _T("SCO_WANT_CAMERAS_BW|");
	}
	if (m_dwOptions & SCO_WANT_CAMERAS_COLOR)
	{
		s += _T("SCO_WANT_CAMERAS_COLOR|");
	}
	if (m_dwOptions & SCO_WANT_DECOMPRESS_BW)
	{
		s += _T("SCO_WANT_DECOMPRESS_BW|");
	}
	if (m_dwOptions & SCO_WANT_DECOMPRESS_COLOR)
	{
		s += _T("SCO_WANT_DECOMPRESS_COLOR|");
	}
	if (m_dwOptions & SCO_IS_DV)
	{
		s += _T("SCO_IS_DV|");
	}
	if (m_dwOptions & SCO_JPEG_AS_H263)
	{
		s += _T("SCO_JPEG_AS_H263|");
	}
	if (m_dwOptions & SCO_NO_STREAM)
	{
		s += _T("SCO_NO_STREAM|");
	}
	if (m_dwOptions & SCO_MULTI_CAMERA)
	{
		s += _T("SCO_MULTI_CAMERA|");
	}
	if (m_dwOptions & SCO_CAN_GOP)
	{
		s += _T("SCO_CAN_GOP|");
	}

	m_remotePID = 0;
	m_pControl = NULL;	// INIT
	m_pControl = pControl;
	m_ConnectionRecord = c;
	m_bAlarm = bAlarm;
	m_dwConnectionTime = 0;

	if (m_bAlarm)
	{
		m_ConnectionRecord.SetDestinationHost(m_ConnectionRecord.GetSourceHost());
		m_ConnectionRecord.SetSourceHost(sRemoteName);
	}
	WK_TRACE(_T("new output client %s %s %s\n"),GetRemoteHost(),m_sVersion,m_sSerial);
	WK_TRACE(_T("RequestedOutputOption %s\n"),s);

	m_dwPriority = 3;
	// m_Processes;
	m_id = CSecID(SECID_OUTPUT_CLIENT,wSubID);
	// m_idDecoderBW
	// m_idDecoderColor
	SetConnectRetryTime(50);

	CPermission *pPermission = theApp.GetPermissions().GetPermission(c.GetPermission());
	if (pPermission) 
	{
		m_dwPriority = pPermission->GetPriority()-1;	// OOPS 1..5
		m_permissionID = pPermission->GetID();
	}

	if (m_ConnectionRecord.GetOptions() & SCO_WANT_DECOMPRESS_BW) 
	{
		COutputList& outputList = theApp.GetOutputGroups();
		for (int i=0;i<outputList.GetSize() && m_idDecoderBW==SECID_NO_ID;i++) 
		{
			if (outputList.GetGroupAt(i)->m_bCanBWDecompress) 
			{
				m_idDecoderBW = CSecID(outputList.GetGroupAt(i)->GetGroupID(),0);
			}
		}
		if (m_idDecoderBW==SECID_NO_ID) 
		{
			m_dwOptions &= ~SCO_WANT_DECOMPRESS_BW;
		}
	}
	// COLOR
	if (m_dwOptions & SCO_WANT_DECOMPRESS_COLOR) 
	{
		COutputList &outputList = theApp.GetOutputGroups();
		for (int i=0;i<outputList.GetSize() && m_idDecoderColor==SECID_NO_ID;i++) 
		{
			if (outputList.GetGroupAt(i)->m_bCanColorDecompress
				&& outputList.GetGroupAt(i)->GetCompressionType()==COMPRESSION_H263) 
			{
				m_idDecoderColor = CSecID(outputList.GetGroupAt(i)->GetGroupID(),0);
			}
		}
		if (m_idDecoderColor==SECID_NO_ID) 
		{
			m_dwOptions &= ~SCO_WANT_DECOMPRESS_COLOR;
		}
	}
	// JPEG to H.263
	if (m_dwOptions & SCO_JPEG_AS_H263) 				   
	{
		COutputList &outputList = theApp.GetOutputGroups();
		CSecID coco;
		for (int i=0;i<outputList.GetSize() && coco==SECID_NO_ID;i++) 
		{
			if (outputList.GetGroupAt(i)->GetCompressionType()==COMPRESSION_H263) 
			{
				coco = CSecID(outputList.GetGroupAt(i)->GetGroupID(),0);
			}
		}
		if (coco!=SECID_NO_ID) 
		{
			m_dwOptions &= ~SCO_JPEG_AS_H263;
			m_dwOptions &= ~SCO_NO_STREAM;
			m_dwOptions &= ~SCO_MULTI_CAMERA;
		}
	}

	m_GOPSender.SetParent(this);
	m_bCanGOP = (m_dwOptions & SCO_CAN_GOP)>0;

	// we can do the following options
	m_dwOptions &= SCO_WANT_RELAYS|SCO_WANT_CAMERAS_COLOR|SCO_JPEG_AS_H263|SCO_NO_STREAM|SCO_MULTI_CAMERA;

	m_dwRemoteBitrate = 1024*1024;
	m_bIsLocal = TRUE;

	if (c.GetFieldValue(CRF_MSN,m_sRemoteMSN))
	{
		// assume max 2 B Channels
		m_dwRemoteBitrate = 2*64*1024;
		m_bIsLocal = FALSE;
		WK_TRACE(_T("ISDN %s, %s\n"),m_sRemoteMSN,c.GetSourceHost());
	}
	if (c.GetFieldValue(CRF_IPADDRESS,m_sRemoteIP))
	{
		WK_TRACE(_T("TCP/IP %s, %s\n"),m_sRemoteIP,c.GetSourceHost());
		m_bIsLocal = FALSE;
	}
	CString sBitrate;
	if (c.GetFieldValue(CRF_BITRATE,sBitrate))
	{
		_stscanf(sBitrate,_T("%d"),&m_dwRemoteBitrate);
		WK_TRACE(_T("bitrate is %d\n"),m_dwRemoteBitrate);
		m_bIsLocal = FALSE;
	}
	m_dwStarttimeForNotConnectedTimeout	= GetTickCount();
	m_dwNotConnectedTimeout = m_ConnectionRecord.GetTimeout();
	m_bInfoConfirmed = FALSE;
	m_dwTickInfoConfirmed = 0;
	m_dwSelectCameraTick = 0;

	InitH263Mpeg4();

	m_pEncoder = NULL;
	if (!m_bIsLocal)
	{
		m_pEncoder = new CEncoderThread();
		m_pEncoder->StartThread();
	}
	if (!m_bCanGOP)
	{
		m_GOPSender.StartThread();
	}
}

/*@MHEAD{destructor:}*/
/*@MD NYD*/
CIPCOutputServerClient::~CIPCOutputServerClient()
{
	m_GOPSender.StopThread();
	if (m_pEncoder)
	{
		m_pEncoder->StopThread();
		WK_DELETE(m_pEncoder);
	}
	DestroyTranscoder();
	StopThread();
	m_Processes.SafeRemoveAll();
}
/////////////////////////////////////////////////////////////////
void CIPCOutputServerClient::InitH263Mpeg4()
{
	if (m_dwOptions & SCO_JPEG_AS_H263) 				   
	{
		if (m_sVersion >= _T("5"))
		{
			m_bH263 = IsLowBandwidth() && !theApp.m_bMpeg4Transmission;
			m_bMpeg4 = theApp.m_bMpeg4Transmission;
		}
		else
		{
			m_bH263 = IsLowBandwidth();
		}
	}
}
/////////////////////////////////////////////////////////////////
void CIPCOutputServerClient::SetFetchResult(CIPCFetchResult& fr)
{
	// alarm case
	DWORD dwRemoteBitrate = 0;
	if (1==_stscanf(fr.GetShmName(),_T("%d"),&dwRemoteBitrate))
	{
		SetBitrate(dwRemoteBitrate);
	}
	SetOptions(fr.GetOptions());
	m_bCanGOP = (m_dwOptions & SCO_CAN_GOP)>0;
	SetCodePage(fr.GetCodePage());
	m_sVersion = fr.GetErrorMessage();
	WK_TRACE(_T("confirmed alarm connection %s, %d\n"),m_sVersion,dwRemoteBitrate);
	InitH263Mpeg4();
}
/////////////////////////////////////////////////////////////////
void CIPCOutputServerClient::CreateTranscoder(int iNumPictures,DWORD dwCalculatedBitrate)
{
	m_csTranscoder.Lock();
	if (   UseH263()
		&& !IsMultiCamera()
		&& m_pTranscoder == NULL)
	{
		m_pTranscoder = new CTranscoderThread(GetID());
		m_pTranscoder->OnModified(iNumPictures,dwCalculatedBitrate);
		m_pTranscoder->StartThread();
	}
	m_csTranscoder.Unlock();
}
/////////////////////////////////////////////////////////////////
void CIPCOutputServerClient::DestroyTranscoder()
{
	CTranscoderThread* pTranscoder = NULL;
	m_csTranscoder.Lock();
	pTranscoder = m_pTranscoder;
	m_pTranscoder = NULL;
	m_csTranscoder.Unlock();

	if (pTranscoder)
	{
		pTranscoder->StopThread();
		WK_DELETE(pTranscoder);
	}
}
/////////////////////////////////////////////////////////////////
BOOL CIPCOutputServerClient::UseH263()
{
	return m_bH263;
}
/////////////////////////////////////////////////////////////////
BOOL CIPCOutputServerClient::IsStreaming()
{
	return !(m_dwOptions & SCO_NO_STREAM);
}
/////////////////////////////////////////////////////////////////
BOOL CIPCOutputServerClient::IsMultiCamera()
{
	return m_dwOptions & SCO_MULTI_CAMERA;
}

/*@MHEAD{access:}*/
/*@MINCLUDE{\Project\SecurityServer\CIPCOutputServerClient.h,CIPCOutputServerClient}*/

/*@MHEAD{basic CIPC stuff:}*/
/*@MD NYD*/
void CIPCOutputServerClient::OnRequestConnection()
{
	WK_TRACE(_T("CIPCOutputServerClient::OnRequestConnection %s\n"),m_ConnectionRecord.GetSourceHost());
	WK_STAT_LOG(_T("Client"),1,GetShmName());
	CIPC::OnRequestConnection();

	// keep a copy, to know it after disconnect
	m_remotePID = GetRemoteProcessID();
	m_dwStarttimeForNotConnectedTimeout = GetTickCount();
}

/*@MD NYD*/
void CIPCOutputServerClient::OnRequestDisconnect()
{
	WK_STAT_LOG(_T("Client"),0,GetShmName());
	TRACE(_T("CIPCOutputServerClient::OnRequestDisconnect %s %d in Queue\n"),
		m_ConnectionRecord.GetSourceHost(),GetReceiveQueueLength());

	DestroyTranscoder();
	if (m_bInfoConfirmed)
	{
		theApp.StopClientProcesses(this);
	}
	// sleep here to make sure the process schedulers second grid
	// goes one second and the client process is done
	m_pControl->RemoveMe(this);
}

/*@MD NYD*/
BOOL CIPCOutputServerClient::OnTimeoutWrite(DWORD dwCmd)
{
	TRACE(_T("%s TimeoutWrite\n"),GetShmName());
	return TRUE;
}
/*@MD NYD*/
BOOL CIPCOutputServerClient::OnTimeoutCmdReceive()
{
	return TRUE;
}

/*@MD 
@ITEM 1 default from CIPC
@ITEM 2 supports VersioInfo
*/
void CIPCOutputServerClient::OnRequestVersionInfo(WORD wGroupID)
{
	DoConfirmVersionInfo(wGroupID,2);
}

/*@MHEAD{infos:}*/
/*@MD
@RELATED @LINK{CIPCOutputRecord}
@ARGS
@ITEM @CW{wGroupID}=SECID_NO_ID for all available groups
@ITEM @CW{wGroupID}=specific groupID, only for this single group

Sends information about the outputs via @LINK{CIPCOutput::DoConfirmInfoOutputs}.

@ALSO @LINK{CIPCOutputRecord}
*/
void CIPCOutputServerClient::OnRequestInfoOutputs(WORD wGroupID)
{
	CHECK_ACCESS(OUTP_REQ_INFO);

	TRACE(_T("OnRequestInfoOutputs %s %s\n"),GetShmName(),GetRemoteHost());
	
	CPermission *pPermission = theApp.GetPermissions().GetPermission(GetConnection().GetPermission());
	
	int iNumRecords = 0;
	int iNumUsedGroups=0;
	const CIPCOutputServer *pOutputGroup=NULL;
	CSecID id;	// used for permission checks

	// pre-calc the required space
	for (int g=0;g<theApp.GetOutputGroups().GetSize();g++) 
	{
		if (  wGroupID==SECID_NO_GROUPID	// all groups ?
			|| wGroupID==theApp.GetOutputGroups().GetGroupAt(g)->GetGroupID()) 
		{
			pOutputGroup = theApp.GetOutputGroups().GetGroupAt(g);
			BOOL bAtLeastOne=FALSE;
			for (WORD j=0;j<pOutputGroup->GetSize();j++) 
			{
				id.Set(pOutputGroup->GetGroupID(),j);
				if (pPermission && pPermission->IsOutputAllowed(id)) 
				{
					iNumRecords++;
					bAtLeastOne=TRUE;
				} 
				else 
				{
					// access denied
				}
			}
			if (bAtLeastOne) 
			{
				iNumUsedGroups++;
			}
		}
	}

	if (iNumRecords) 
	{

		CIPCOutputRecord *records = new CIPCOutputRecord[iNumRecords];

		// fill allowed records
		int r=0; // record counter
		for (g=0; g<theApp.GetOutputGroups().GetSize() && records;g++)
		{
			const CIPCOutputServer *pOutputGroup = theApp.GetOutputGroups().GetGroupAt(g);
/*
			WK_TRACE(_T("Group %s %d Elements Hardware State %d\n"),
				pOutputGroup->GetShmName(),
				pOutputGroup->GetSize(),
				pOutputGroup->GetHardwareState());
*/
			if (   wGroupID==SECID_NO_GROUPID
				|| wGroupID==pOutputGroup->GetGroupID()) 
			{
				DWORD dwOneBit=1;
				for (WORD i=0;i<pOutputGroup->GetSize();i++, dwOneBit<<1) 
				{
					id.Set(pOutputGroup->GetGroupID(),i);
					if (   pPermission 
						&& pPermission->IsOutputAllowed(id)) 
					{
						const COutput *pOutput = pOutputGroup->GetOutputAt(i);
						WORD wFlags=0;

						if (pOutputGroup->HasCameras())
						{
							wFlags |= OREC_IS_CAMERA;
						}

						switch (pOutputGroup->GetState(i)) 
						{
							case GMS_ERROR:
								if (pOutputGroup->GetFreeMask() & dwOneBit) 
								{
									wFlags |= OREC_IS_ENABLED;
								}						
								break;
							case GMS_INACTIVE:
							case GMS_ACTIVE:
								wFlags |= OREC_IS_OKAY | OREC_IS_ENABLED;
								break;
							case GMS_OFF:
								wFlags |= OREC_IS_OKAY;
								break;
							default:
								WK_TRACE_ERROR(_T("GMS:Wrong state\n"));
						}
						switch (pOutput->GetOutputType()) 
						{
							case OUTPUT_CAMERA: 
								wFlags |= OREC_IS_CAMERA|OREC_CAMERA_COLOR;

								if (pOutput->IsFixed()) 
								{
									wFlags |= OREC_CAMERA_IS_FIXED;
								}
								if (pOutput->IsReference()) 
								{
									wFlags |= OREC_CAMERA_REFERENCE;
								}
								
								if (pOutputGroup->IsTasha())
								{
									// Tasha nur MPEG4
									wFlags |= OREC_CAMERA_DOES_MPEG;
								}
								else if (   pOutputGroup->IsJaCob()
									     || pOutputGroup->IsIP())
								{
									if (IsLowBandwidth())
									{
										// ISDN or DSL
										if (CanMpeg4())
										{
											wFlags |= OREC_CAMERA_DOES_MPEG;
										}
										else if (UseH263())
										{
											// H.263 is JPEG
											wFlags |= OREC_CAMERA_DOES_JPEG;
										}
										else
										{
											// unknown JPEG
											wFlags |= OREC_CAMERA_DOES_JPEG;
										}
									}
									else
									{
										// Jacob Local und Netz JPEG
										wFlags |= OREC_CAMERA_DOES_JPEG;
									}
								}
								else if (   pOutputGroup->IsSaVic()
									     || pOutputGroup->IsQ())
								{
									if (IsLowBandwidth())
									{
										// ISDN or DSL
										if (CanMpeg4())
										{
											wFlags |= OREC_CAMERA_DOES_MPEG;
										}
										else if (UseH263())
										{
											// H.263 is JPEG
											wFlags |= OREC_CAMERA_DOES_JPEG;
										}
										else
										{
											wFlags |= OREC_CAMERA_DOES_JPEG;
										}
									}
									else if (IsLocal())
									{
										// local is YUV422 but handled as jpeg
										if (pOutputGroup->IsSaVic())
										{
											wFlags |= OREC_CAMERA_DOES_JPEG;
										}
										else if (pOutputGroup->IsQ())
										{
											wFlags |= OREC_CAMERA_YUV;
										}
									}
									else
									{
										if (CanMpeg4())
										{
											// new clients make  MPEG4
											wFlags |= OREC_CAMERA_DOES_MPEG;
										}
										else
										{
											// old ones do JPEG
											wFlags |= OREC_CAMERA_DOES_JPEG;
										}
									}
								}
								break;
							case OUTPUT_RELAY: 
								wFlags |= OREC_IS_RELAY;
								if (pOutput->m_bRelayClosed) 
								{
									wFlags |= OREC_RELAY_CLOSED;
								}
								if (pOutput->GetRelayControlType() == RCT_EDGE)
								{
									wFlags |= OREC_RELAY_IS_EDGE;
								}
								break;
						}

						ASSERT(r<iNumRecords);
						records[r].Set(pOutput->GetName(),pOutput->GetID(),wFlags); 
						r++;
					} else {
						// accesss denied
					}
				}
			}
		}
		DoConfirmInfoOutputs(wGroupID, iNumUsedGroups, iNumRecords, records);
		WK_DELETE_ARRAY(records);
	} else {
		// no records at all
		CIPCOutputRecord dummyRecords[1];
		DoConfirmInfoOutputs(wGroupID, iNumUsedGroups, 0, dummyRecords);
	}
	m_dwTickInfoConfirmed = GetTickCount();
	m_bInfoConfirmed = TRUE;

	CheckNagScreen(); //checks if maintenance and notifable RTEs should be shown on screen
}
/////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputServerClient::CheckNagScreen()
{
	if (!IsAlarm())
	{
		//check for notifiable errors: HDD loss, CPU temperature too high, fan speed too slow
		CString sErrors = CheckNotifiableErrors();

		//check for maintenance
		CString sMaintenance = CheckMaintenance();

		CString sMsg = sErrors + sMaintenance;
		//send notification message
		if(!sMsg.IsEmpty())
		{
			CSecID id(SECID_GROUP_NOTIFICATIONMESSAGE,0);
			DoRequestSetValue(id,NM_MESSAGE,sMsg);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////
CString CIPCOutputServerClient::CheckNotifiableErrors()
{
	//check RTEs for HDD loss, CPU temperature too high, fan speed too slow

	CWK_Profile wkp;
	CWK_RunTimeErrors RTEErrors;
	CString sErrorPath;
	int iStartPos = 0;

	sErrorPath = wkp.GetString(SECTION_COMMON, COMMON_RUNTIME_ERRORS, DEFAULT_ERRORFILE);

	BOOL bLoad = RTEErrors.LoadFromFile(sErrorPath);
	if (!bLoad && sErrorPath.CompareNoCase(DEFAULT_ERRORFILE) != 0)
	{
		bLoad = RTEErrors.LoadFromFile(DEFAULT_ERRORFILE);
	}

	CString sError, sErrorText, sReturnErrorText, sTimestamp;
	CMapDWordToCString dwMapRTE;

	if(bLoad)
	{
		iStartPos = max(RTEErrors.GetSize()-1, 0);
		if (iStartPos > 0)
		{	
			CWK_RunTimeError* pRTE = NULL;
			int i = iStartPos;
			CString *psDummy;

			BOOL bGetErrorText = FALSE;
			for (i=iStartPos; i >= 0; i--)
			{
				pRTE = RTEErrors.GetAtFast(i);
				if(pRTE)
				{
					//every non confirmed error trace only once, the latest one
					WK_RunTimeError WKRTEError = pRTE->GetError();
					WORD wDevice = 0;
					BOOL bFoundNotifableError = TRUE;

					switch(WKRTEError)
					{
					case RTE_HARDDISK_FALURE:
						wDevice = LOWORD(pRTE->GetParam1()); // drive letter
						break;
					case RTE_SMART:
						wDevice = LOWORD(pRTE->GetParam1()); // drive number
						break;
					case RTE_TEMPERATURE:
						wDevice = HIWORD(pRTE->GetParam1()); // device (HDD, CPU, Power Supp, board, case, system)
						break;
					case RTE_FANCONTROL:
						wDevice = HIWORD(pRTE->GetParam1()); // device (HDD, CPU, Power Supp, board, case, system)
						break;
					default:
						bFoundNotifableError = FALSE;
						break;
					}
					DWORD dwRTE = MAKELONG((WORD)WKRTEError, wDevice);
					if (!bFoundNotifableError || dwMapRTE.Lookup(dwRTE, psDummy))
					{
						bGetErrorText = FALSE;
					}
					else
					{
						psDummy = NULL;
						bGetErrorText = TRUE;
						dwMapRTE.SetAt(dwRTE, psDummy);
					}

					if(bGetErrorText)
					{
						//build error message
						CString sMsg;
						sMsg = pRTE->GetErrorText()+_T(" ")+pRTE->GetFormattedTimeStamp()+_T("\r\n\r\n");
						sReturnErrorText += sMsg;
						bGetErrorText = FALSE;
					}
				}
			}
		}
	}

	if(!sReturnErrorText.IsEmpty())
	{
		CString sHeader;
		sHeader.LoadString(IDS_OEMGUI_HARDWARE_HEADER);
		sReturnErrorText = sHeader + sReturnErrorText + _T("\r\n\r\n");
	}
	return sReturnErrorText;	
}
/////////////////////////////////////////////////////////////////////////////////////
CString CIPCOutputServerClient::CheckMaintenance()
{
	CString sMaintenanceText;

	// is maintenance necessary ??
	// inform the client
	CWK_Profile wkp;
	BOOL bMaintenanceOn = wkp.GetInt(_T("Maintenance"),_T("IntervallOnOff"),FALSE);
	if (bMaintenanceOn)
	{
		int nMonth = wkp.GetInt(_T("Maintenance"),_T("Intervall"),0);
		if (nMonth>0)
		{
			CString sStartDate = wkp.GetString(_T("Maintenance"),_T("IntervallStartDate"),_T(""));
			CSystemTime stMaintenance;
			if (stMaintenance.SetDBDate(sStartDate))
			{
				CSystemTime stNow;
				stNow.GetLocalTime();
				stMaintenance.IncrementTime(30*nMonth,0,0,0,0);
				if (stMaintenance < stNow)
				{
					sMaintenanceText = GetMaintenanceText();
				}
			}
		}
	}
	return sMaintenanceText;
}
/////////////////////////////////////////////////////////////////////////////////////
CString CIPCOutputServerClient::GetMaintenanceText()
{	
	CString sText;
	CWK_Profile wkp;
	CString sNewLine(_T("\r\n"));
	CString sBlank(_T("  "));

	//create headers
	//-------------------------------------
	CString sHeader1, sHeader2, sHeader3, sStartDate;
   	sHeader1.LoadString(IDS_OEMGUI_HEADER1);
	//sHeader1 = _T("Wartungs-Hinweis!");
	sHeader2.LoadString(IDS_OEMGUI_HEADER2);
	sStartDate = wkp.GetString(_T("Maintenance"),_T("IntervallStartDate"),_T(""));
	if(!sStartDate.IsEmpty())
	{
		CSystemTime st;
		st.SetDBDate(sStartDate);
		sHeader2 += sBlank + st.GetDate() + _T(".");
	}
	sHeader3.LoadString(IDS_OEMGUI_HEADER3);
	
	//create serial number
	//-------------------------------------
	CString sSerialComputer, sSerialStr, sSerial;
	CWK_Profile prof(CWK_Profile::WKP_ABSOLUT,HKEY_LOCAL_MACHINE,_T(""), _T(""));
	sSerialComputer = prof.GetString(_T("System\\CurrentControlSet\\Control\\ComputerName\\ComputerName"),_T("ComputerName"),_T(""));
	sSerialStr.LoadString(IDS_OEMGUI_SERIAL);
	sSerial.Format(_T("%s %s"), sSerialStr, sSerialComputer);

	//create software version
	//-------------------------------------
	CString sVersionIdip, sVersionStr, sVersion;
	sVersionStr.LoadString(IDS_OEMGUI_VERSION);
	sVersionIdip = COemGuiApi::GetSoftwareVersion();
	sVersion.Format(_T("%s %s"), sVersionStr, sVersionIdip);
	
	//create installer header
	//-------------------------------------
	CString sInstStr;
	sInstStr.LoadString(IDS_OEMGUI_INSTALLER);

	//create installer data
	//-------------------------------------
	CString sCompany, sStreet, sPostcode, sCity, sEmail, sTelefon, sCountry;
	CString sCompanyStr, sStreetStr, sPostcodeStr, sCityStr, sEmailStr, sTelefonStr, sCountryStr;
	CString sCompanyReg, sStreetReg, sPostcodeReg, sCityReg, sEmailReg, sTelefonReg, sCountryReg;

	sCompanyReg = wkp.GetString(_T("Maintenance"),_T("Company"),_T(""));
	sStreetReg = wkp.GetString(_T("Maintenance"),_T("Street"),_T(""));
	sPostcodeReg = wkp.GetString(_T("Maintenance"),_T("Postcode"),_T(""));
	sCityReg = wkp.GetString(_T("Maintenance"),_T("City"),_T(""));
	sEmailReg = wkp.GetString(_T("Maintenance"),_T("Email"),_T(""));
	sTelefonReg = wkp.GetString(_T("Maintenance"),_T("Telefon"),_T(""));
	sCountryReg = wkp.GetString(_T("Maintenance"),_T("Country"),_T(""));

	sCompanyStr.LoadString(IDS_OEMGUI_COMPANY);
	sStreetStr.LoadString(IDS_OEMGUI_STREET);
	sPostcodeStr.LoadString(IDS_OEMGUI_POSTCODE);
	sCityStr.LoadString(IDS_OEMGUI_CITY);
	sEmailStr.LoadString(IDS_OEMGUI_EMAIL);
	sTelefonStr.LoadString(IDS_OEMGUI_TELEFON);
	sCountryStr.LoadString(IDS_OEMGUI_COUNTRY);

	sCompany = sCompanyStr + sBlank + sCompanyReg;
	sStreet	 = sStreetStr + sBlank + sStreetReg;
	sPostcode = sPostcodeStr + sBlank + sPostcodeReg;
	sCity = sCityStr + sBlank + sCityReg;
	sEmail = sEmailStr + sBlank + sEmailReg;
	sTelefon = sTelefonStr + sBlank + sTelefonReg;
	sCountry = sCountryStr + sBlank + sCountryReg;

	//create the whole nag screen text
	//-------------------------------------
	sText = sHeader1 + sNewLine;
	sText += sHeader2 + sNewLine;
	sText += sHeader3 + sNewLine + sNewLine;

	sText += sSerial + sNewLine;
	sText += sVersion + sNewLine + sNewLine;

	sText += sInstStr + sNewLine;
	sText += _T("--------------------") + sNewLine;
	sText += sCompany + sNewLine;
	sText += sStreet + sNewLine;
	sText += sPostcode + sNewLine;
	sText += sCity + sNewLine;
	sText += sEmail + sNewLine;
	sText += sTelefon + sNewLine;
	sText += sCountry + sNewLine;
	return sText;
}

/*@MHEAD{encoding:}*/
/*@MD 
@RELATED @MLINK{CIPCOutputServerClient::DoClientEncoding}

Erzeugt einen client Prozess oder stoppt bei @CW{iNumPictures}==0

@REMARK Limited 0<=@CW{iNumPictures}<1000
*/
void CIPCOutputServerClient::OnRequestNewJpegEncoding(CSecID camID, 
													Resolution	res, 
													Compression comp,
													int iNumPictures,
													DWORD dwTimeSlice
													)
{
	CHECK_ACCESS(PICT_REQ_JPEG_ENCODING);
	if (m_bTimeoutDisconnected) return;
	
	CIPCOutputServer *pEncoder = theApp.GetOutputGroups().GetGroupByID(camID);

	if (pEncoder) 
	{
		if (pEncoder->IsIP())
		{
			if (iNumPictures>0)
			{
				OnRequestStartVideo(camID,res,comp,COMPRESSION_JPEG,iNumPictures,1,0);
			}
			else
			{
				OnRequestStopVideo(camID,0);
			}

		}
		else
		{
			if (m_bTraceClientJpegRequests) 
			{
				WK_TRACE(_T("OnRequestNewJpegEncoding iNumPics %d slice %d\n"),iNumPictures, dwTimeSlice);
			}
			if (iNumPictures) 
			{
				// OOPS as default for all MiCo and CoCo ?
				if (iNumPictures>100) 
				{
					WK_TRACE_ERROR(_T("More than 100 pics (%d)'n"),iNumPictures);
					iNumPictures=0;
				}
			} 
			else 
			{
				// iNumPictures is zero, no modification
			}
			if (iNumPictures<0 || iNumPictures>1000) 
			{
				iNumPictures=0;
			}
			Compression c = comp;

			//	if (IsLocal())
			{
				c = pEncoder->GetProcessScheduler()->GetDefaultCompression(camID);
			}

			DoClientEncoding(camID,res,c,pEncoder->GetCompressionType(),iNumPictures,dwTimeSlice,GetBitrate(),FALSE);
		}
	} 
	else 
	{
		// encoder not found
			iNumPictures=0;
	}
}
//////////////////////////////////////////////////////////////////////////////////
void CIPCOutputServerClient::OnRequestStartVideo(CSecID id,			// camera nr.
												Resolution res,		// resolution
												Compression comp,		// image size or bitrate
												CompressionType ct,	// JPEG, YUV, MPEG4 ...
												int iFPS,
												int iIFrameInterval, 
												DWORD dwUserData		// unique server id
												)
{
	// Tasha Live
	CHECK_ACCESS(CIPC_OUTPUT_REQUEST_START_VIDEO);
	
	if (m_bTimeoutDisconnected) 
	{
		return;
	}
	
	if (m_bTraceClientJpegRequests) 
	{
		WK_TRACE(_T("OnRequestStartVideo %08lx %s,%s,%s,%d\n"),id.GetID(),
			NameOfEnum(res),NameOfEnum(comp),NameOfEnum(ct),iFPS);
	}

	Compression c = comp;
	Resolution r = res;
	int iFrames = max(1,iFPS);
	BOOL bTranscode = FALSE;

	CIPCOutputServer *pOutput = theApp.GetOutputGroups().GetGroupByID(id);
	if(pOutput)
	{
		c = pOutput->GetProcessScheduler()->GetDefaultCompression(id);

		if (   pOutput->IsTasha()
			|| pOutput->IsQ())
		{
			if (GetBitrate() <= 64*1024)
			{
				iFrames = min(iFPS,6);
				c = COMPRESSION_26;
				if (res == RESOLUTION_2CIF)
				{
					r = RESOLUTION_CIF;
				}
			}
			else if (GetBitrate() <= 128*1024)
			{
				iFrames = min(iFPS,12);
				c = COMPRESSION_25;
				if (res == RESOLUTION_2CIF)
				{
					r = RESOLUTION_CIF;
				}
			}
			else
			{
				iFrames = min(iFPS,25);
				if (res == RESOLUTION_QCIF)
				{
					r = RESOLUTION_CIF;
				}
			}
		}
		else if (   pOutput->IsSaVic()
				 || pOutput->IsJaCob())
		{
			if (GetBitrate() <= 64*1024)
			{
				iFrames = min(iFrames,6);
			}
			else
			{
				iFrames = min(iFrames,12);
			}
			if (r == RESOLUTION_CIF)
			{
				r = RESOLUTION_2CIF;
			}
		}
		if (   pOutput->IsSaVic()
			|| pOutput->IsJaCob()
			|| pOutput->IsQ()
			)
		{
			if (   ct == COMPRESSION_MPEG4
				|| ct == COMPRESSION_H263)
			{
				bTranscode = TRUE;
			}
		}
	}

	DoClientEncoding(id,r,c,ct,iFrames,1000,GetBitrate(),bTranscode);
}
//////////////////////////////////////////////////////////////////////////////////
void CIPCOutputServerClient::OnRequestStopVideo(CSecID id,DWORD dwUserData)
{
	if (m_bTraceClientJpegRequests) 
	{
		WK_TRACE(_T("OnRequestStopVideo %08x\n"),id.GetID());
	}
	DoClientEncoding(id,RESOLUTION_NONE,COMPRESSION_NONE,COMPRESSION_UNKNOWN,0,dwUserData,GetBitrate(),FALSE);
	DestroyTranscoder();
}
/*@MD 
Zentrale Hilfsfunktion, um client Prozesse zu starten. Bei
@CW{iNumPictures}==0 wird ein aktiver Prozess gestoppt.
Ist in der Lage von einer OutputGruppe zur anderen zu wechseln,
z.B. CoCo<<->MiCo, da der Prozess in den ProcessScheduler der jeweiligen
OutputGroup gehoert.

@RELATED @MLINK{CProcessScheduler::ClientJpegEncoding}
*/
void CIPCOutputServerClient::DoClientEncoding(CSecID camID, 
											Resolution	res, 
											Compression comp,
											CompressionType ct,
											int iNumPictures,
											DWORD dwTimeSlice,
											DWORD dwBitrate,
											BOOL bTranscode
											)
{
	CIPCOutputServer *pOutputGroup = theApp.GetOutputGroups().GetGroupByID(camID);

	// NOT YET possible drop OldEcnoder without newEncoder
	if (pOutputGroup) 
	{
		// modify process parameters or create a new process
		if (IsMultiCamera())
		{
			CProcess* pProcessLoop = NULL;
			CProcess* pProcess = NULL;
			BOOL bFound = FALSE;
			int iNumCams = 0;

			m_Processes.Lock(_T(__FUNCTION__));
			for (int i=0;i<m_Processes.GetSize();i++)
			{
				pProcessLoop = m_Processes.GetAtFast(i);
				if (   pProcessLoop
					&& pProcessLoop->IsClientEncodingProcess()
					)
				{
					if (pProcessLoop->GetActivation()->GetOutputID() == camID)
					{
						bFound = TRUE;
						pProcess = pProcessLoop;
					}
					iNumCams++;
				}
			}
			if (   bFound
				&& iNumPictures == 0)
			{
				m_Processes.Remove(pProcess);
			}

			if (   (   pOutputGroup->IsSaVic() 
				    || pOutputGroup->IsQ())
				&& (   IsLocal()
				    || UseH263()
					|| CanMpeg4())
					)
			{
				ct = COMPRESSION_YUV_422;
			}
			if (bTranscode && iNumCams>0)
			{
				// max number of picts
				int iMax = pOutputGroup->GetProcessSchedulerVideo()->GetMaxNumCameraSwitches()/iNumCams;
				iMax = max(iMax,1);
				iNumPictures = min(iNumPictures,iMax);
			}
			m_Processes.Unlock();

			//TRACE(_T("DoClientEncoding %08lx %s,%s,%d\n"),camID.GetID(),NameOfEnum(res),NameOfEnum(comp),iNumPictures);
			pOutputGroup->GetProcessSchedulerVideo()->DoClientEncoding(GetID(), 
																pProcess, // CAVEAT by ref, is set in call
																camID, 
																res, 
																comp, 
																ct,
																iNumPictures, 
																dwTimeSlice, 
																dwBitrate);
			if (   !bFound
				&& pProcess)
			{
				m_Processes.SafeAdd(pProcess);
			}

		}
		else
		{
			// no check on iNumPictures at this point, if iNumPictures is zero,
			// the process-scheduler will drop the process
			CProcess* pEncodeProcess = NULL;
			BOOL bFound = FALSE;
			m_Processes.Lock(_T(__FUNCTION__));
			for (int i=0;i<m_Processes.GetSize();i++)
			{
				pEncodeProcess = m_Processes.GetAtFast(i);
				if (   pEncodeProcess 
					&& pEncodeProcess->IsClientEncodingProcess()
					)
				{
					bFound = TRUE;
					if (pEncodeProcess->GetActivation()->GetOutputID().GetGroupID()!=pOutputGroup->GetGroupID()) 
					{
						// switch to other group / card
						CIPCOutputServer *pOldEncoder = theApp.GetOutputGroups().GetGroupByID(pEncodeProcess->GetActivation()->GetOutputID());
						if (pOldEncoder) 
						{
							m_Processes.Remove(pEncodeProcess);
							pOldEncoder->GetProcessSchedulerVideo()->DoClientEncoding(	GetID(), 
																				pEncodeProcess, // CAVEAT by ref, is set in call
																				camID, 
																				RESOLUTION_NONE,
																				COMPRESSION_NONE, 
																				COMPRESSION_UNKNOWN,
																				0, // numPictuires
																				0, 
																				dwBitrate
																				);
							pEncodeProcess = NULL;
							bFound = FALSE;
						}
					}
					break;
				}
			}
			m_Processes.Unlock();
			pOutputGroup->GetProcessSchedulerVideo()->DoClientEncoding(GetID(), 
																pEncodeProcess, // CAVEAT by ref, is set in call
																camID, 
																res, 
																comp,
																ct,
																iNumPictures, 
																dwTimeSlice, 
																dwBitrate);
			if (   !bFound
				&& pEncodeProcess)
			{
				m_Processes.SafeAdd(pEncodeProcess);
			}
		}
		// stop the savic MPEG4 encoder
		if (   pOutputGroup->IsSaVic()
			&& CanMpeg4()
			&& iNumPictures==0
			&& m_pEncoder)
		{
			m_pEncoder->RemoveClient(GetID().GetID(),camID);
		}
	} 
	else 
	{
		WK_TRACE_ERROR(_T("outputgroup not found %s for cam %08lx\n"),GetShmName(),camID.GetID());
		// NOT YET possible drop old for oldEncoder
	}
}

/*@MHEAD{decoding:}*/
/*@MD 
@ARGS
@CW{wGroupID} either SECID_NO_GROUPID, for the default, or a specific groupID.

*/
void CIPCOutputServerClient::OnRequestJpegDecoding(WORD wGroupID, Resolution res, 
									 const CIPCExtraMemory &data,
									 DWORD dwUserData
									 )
{
	CHECK_ACCESS(PICT_REQ_DECOMPRESS);

	CSecID tmpID;

	if (wGroupID!=SECID_NO_GROUPID) {	// decoder specified ?
		tmpID.Set(wGroupID,0);
	} else {
		tmpID = m_idDecoderBW;	// use default decoder
	}

	CIPCOutputServer *pDecoder = theApp.GetOutputGroups().GetGroupByID(tmpID);
	if (pDecoder==NULL) {
		// NOT YET error indication
		WK_TRACE_ERROR(_T("OnRequestJpegDecoding: no bw decoder found\n"));
	} else {
		// pDecoder found
		if (pDecoder->m_bCanBWDecompress==FALSE) {
			WK_TRACE_ERROR(_T("%s can't do BW decompress\n"),pDecoder->GetShmName());
		} else {
			CIPCExtraMemory *pTmp = new CIPCExtraMemory(data);
			pDecoder->DoRequestJpegDecodingExtra(wGroupID, res,pTmp, dwUserData);
			WK_DELETE(pTmp);
		}
	}
}
//
// H263
//
/*@MHEAD{overlay:}*/
/*@MD NYD*/

void CIPCOutputServerClient::OnRequestSetDisplayWindow(WORD wGroupID, const CRect &rect)
{
	CHECK_ACCESS(PICT_REQ_WND_DISPLAY);
	
	if (theApp.GetServerControl()->CanDoOverlay(GetID())==FALSE) 
	{
		// overlay access denied
		WK_STAT_LOG(_T("Client"),-1,_T("OverlayDenied"));
		return;		// EXIT
	}

	CSecID id(wGroupID,0);
	if (wGroupID==SECID_NO_GROUPID) {
		id = m_idDecoderColor;	// default color decoder
	}

	if (m_idDecoderColor==SECID_NO_ID) {
		// no need for a message
		// database client ALWAYS calls SetDisplayWindow
	} else {
		CIPCOutputServer *pDecoder = theApp.GetOutputGroups().GetGroupByID(id);
		if (pDecoder==NULL) {
			WK_TRACE_ERROR(_T("SetDisplayWindow: decoder not found\n"));
		} else {
			if (pDecoder->GetCompressionType()==COMPRESSION_H263) {
				// update the overlay client, might inform the current/old 
				theApp.GetServerControl()->SetOverlayClient(GetID(), id.GetGroupID());

				pDecoder->DoRequestSetDisplayWindow(wGroupID, rect);
			} else {
				WK_TRACE_WARNING(_T("Overlay for non H261 %s\n"),pDecoder->GetShmName());
			}
		}
	}
}
/*@MD NYD*/
void CIPCOutputServerClient::OnRequestSetOutputWindow (WORD wGroupID, 
													   const CRect &rect,
													   OverlayDisplayMode odm
													   )
{
	CHECK_ACCESS(PICT_REQ_WND_OUTP);

	if (!theApp.GetServerControl()->CanDoOverlay(GetID())) 
	{
		// overlay access denied
		WK_STAT_LOG(_T("Client"),-1,_T("OverlayDenied"));
		return;		// EXIT
	}

	if (m_idDecoderColor==SECID_NO_ID) {
		// no message, client might call it all the time
		return;	// <<< EXIT >>>
	}

	CSecID id(wGroupID,0);
	if (wGroupID==SECID_NO_GROUPID) {
		id = m_idDecoderColor;	// default color decoder
	}

	CIPCOutputServer *pDecoder = theApp.GetOutputGroups().GetGroupByID(id);
	if (pDecoder==NULL) {
		WK_TRACE_ERROR(_T("SetOutputWindow no decoder\n"));
	} else {
		// update the overlay client, might inform the current/old client
		theApp.GetServerControl()->SetOverlayClient(GetID(),id.GetGroupID());
		pDecoder->DoRequestSetOutputWindow(wGroupID, rect, odm);		

	}
}

/*@MHEAD{relay control:}*/
/*@MD NYD*/
void CIPCOutputServerClient::OnRequestSetRelay(CSecID relayID, BOOL bClosed)
{
	CHECK_ACCESS(OUTP_REQUEST_SET_RELAY);

	CIPCOutputServer *pOutput = theApp.GetOutputGroups().GetGroupByID(relayID);
	if (pOutput) 
	{
		CProcess* pProcess = NULL;
		pOutput->GetProcessSchedulerVideo()->ClientRelayProcess(GetID(),
															    pProcess,
															    relayID,
															    bClosed);
		if (pProcess)
		{
			m_Processes.Add(pProcess);
		}

	} 
	else 
	{
		WK_TRACE_ERROR(_T("SetRelay INVALID ID %x\n"),relayID.GetID());
	}
}

/*@MHEAD{mpeg encoding:}*/
/*@MD NYD
Erzeugt einen client Prozess unter Verwendung von
"CoCoClientPicsPerSecond"
@REMARK unused @CW{dwRecordTime}
@RELATED @MLINK{CIPCOutputServerClient::OnRequestStopH263Encoding}
@LINK{Server,ConfigOptions}
*/

void CIPCOutputServerClient::OnRequestStartH263Encoding(CSecID camID, 
														Resolution res, 
														Compression comp,
														DWORD dwBitrateScale,
														DWORD dwRecordTime)
{
	CHECK_ACCESS(PICT_REQ_START_H263);
	if (m_bTimeoutDisconnected) return;

	// OOPS unused recordtime ?
	int iNumPictures = m_iCoCoClientPicsPerSecond;
	BOOL bTranscode = FALSE;
	CIPCOutputServer *pEncoder = theApp.GetOutputGroups().GetGroupByID(camID);
	if (   pEncoder
		&& (   pEncoder->GetCompressionType() == COMPRESSION_JPEG
		    || pEncoder->GetCompressionType() == COMPRESSION_YUV_422)
		)
	{
		bTranscode = TRUE;
		//TRACE(_T("requested compression is %s\n"),NameOfEnum(comp));
		switch (comp)
		{
		// 512 kBit
		case COMPRESSION_7:	 iNumPictures = 12; break; 
		case COMPRESSION_8:	 iNumPictures = 9;  break; 
		case COMPRESSION_9:	 iNumPictures = 6;  break;
		case COMPRESSION_10: iNumPictures = 3;  break;
		case COMPRESSION_11: iNumPictures = 1; 	break;
		// 256 kBit
		case COMPRESSION_12: iNumPictures = 12; break; 
		case COMPRESSION_13: iNumPictures = 9;  break; 
		case COMPRESSION_14: iNumPictures = 6;  break;
		case COMPRESSION_15: iNumPictures = 3;  break;
		case COMPRESSION_16: iNumPictures = 1; 	break;
		// 128 kBit
		case COMPRESSION_17: iNumPictures = 12; break; 
		case COMPRESSION_18: iNumPictures = 9;  break; 
		case COMPRESSION_19: iNumPictures = 6;  break;
		case COMPRESSION_20: iNumPictures = 3;  break;
		case COMPRESSION_21: iNumPictures = 1; 	break;
		// 64 kBit
		case COMPRESSION_22: iNumPictures = 12; break; 
		case COMPRESSION_23: iNumPictures = 9;  break; 
		case COMPRESSION_24: iNumPictures = 6;  break;
		case COMPRESSION_25: iNumPictures = 3;  break;
		case COMPRESSION_26: iNumPictures = 1; 	break;
		default:
			iNumPictures = 1; 
			break;
		}
		int iMaxHigh = 3;
		int iMaxLow = 6;
		if (m_dwRemoteBitrate<=64*1024)
		{
			iMaxHigh = 3;
			iMaxLow = 6;
			if (IsMultiCamera())
			{
				iMaxHigh = 6;
				iMaxLow = 9;
			}
		}
		else if (m_dwRemoteBitrate<=128*1024)
		{
			iMaxHigh = 6;
			iMaxLow = 9;
			if (IsMultiCamera())
			{
				iMaxHigh = 9;
				iMaxLow = 12;
			}
		}
		if (res == RESOLUTION_2CIF)
		{
			if (iNumPictures>iMaxHigh)
			{
				iNumPictures = iMaxHigh;
			}
		}
		else if (res == RESOLUTION_QCIF)
		{
			if (iNumPictures>iMaxLow)
			{
				iNumPictures = iMaxLow;
			}
		}
		// always use default Compression 5 for JPEG to H.263 recoding
		comp = pEncoder->GetProcessScheduler()->GetDefaultCompression(camID);

		DWORD dwCalculatedBitrate = GetBitrate();
		if (IsStreaming())
		{
			dwCalculatedBitrate = (dwCalculatedBitrate * dwBitrateScale) / 1000;
		}
		if (!IsMultiCamera())
		{
			m_csTranscoder.Lock();
			if (m_pTranscoder)
			{
				m_pTranscoder->OnModified(iNumPictures,dwCalculatedBitrate);
			}
			else
			{
				CreateTranscoder(iNumPictures,dwCalculatedBitrate);
			}
			m_csTranscoder.Unlock();
		}
	}
	DoClientEncoding(camID,res,comp,COMPRESSION_H263,iNumPictures,0,GetBitrate(),bTranscode);
}
/*@MD NYD*/
void CIPCOutputServerClient::OnRequestStopH263Encoding(CSecID camID)
{
	DoClientEncoding(camID,RESOLUTION_NONE,COMPRESSION_NONE,COMPRESSION_UNKNOWN,0,0,0,FALSE);
	DestroyTranscoder();
}
/*@MHEAD{mpeg decoding:}*/
/*@MD NYD*/
void CIPCOutputServerClient::OnRequestH263Decoding(WORD wGroupID, 
										const CRect &rect,
										const CIPCPictureRecord &pict,
										DWORD dwUserData)
{
	if(    theApp.AllowClientAccess(this,PICT_REQ_DISPLAY_H263)==FALSE
		|| theApp.GetServerControl()->CanDoOverlay(GetID())==FALSE) 
	{
		DoConfirmH263Decoding(1,dwUserData);
		WK_STAT_LOG(_T("Client"),-1,_T("DecompressDenied"));
		return;
	}	

	CSecID id(wGroupID,0);
	if (wGroupID==SECID_NO_GROUPID) {
		id = m_idDecoderColor;	// default color decoder
	}

	CIPCOutputServer *pOutput = theApp.GetOutputGroups().GetGroupByID(id);
	if (pOutput) {
		// update the overlay client, might inform the current/old 
		theApp.GetServerControl()->SetDecompressClient(this);
		theApp.GetServerControl()->SetOverlayClient(GetID(),id.GetGroupID());

		// remember dwUserData to answer the client, when the unit
		// has send the confirm
		CClientLink *pUsage = new CClientLink(this,dwUserData);
		theApp.GetServerControl()->m_usageH263Decoding.SafeAdd(pUsage);
		pOutput->DoRequestH263Decoding(wGroupID, rect, pict, pUsage->m_dwID);
	} else {
		DoConfirmH263Decoding(0,dwUserData);
		WK_TRACE_ERROR(_T("OnRequestH263Decoding INVALID ID/no decoder\n"));
	}
}


/*@MHEAD{unsorted:} */
/*@MD NYD*/
void CIPCOutputServerClient::OnRequestGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						DWORD dwServerData
					)
{
	CHECK_ACCESS(CIPC_BASE_REQUEST_GET_VALUE);
	if (id != SECID_NO_ID)
	{
		CIPCOutputServer *pOutput = theApp.GetOutputGroups().GetGroupByID(id);
		if (pOutput) 
		{
			if (sKey==CSD_APPLICATION_NAME)		
			{
				WK_TRACE_ERROR(_T("NOT YET output get application name for %s\n"),(LPCTSTR)pOutput->GetShmName());
			} 
			else if (sKey==CSD_MODE)
			{
				// TODO
				if (pOutput->IsMDActive(id))
				{
					DoConfirmGetValue(id,sKey,CSD_MD,0);
				}
			}
			else 
			{
				pOutput->DoRequestGetValue(id,sKey,m_id.GetID());
			}
		}
		else {
			WK_TRACE_ERROR(_T("OnRequestGetValue INVALID ID %x\n"),id.GetID());
		}
	}
	else
	{
		// I have to answer
		if (0==sKey.CompareNoCase(_T("istcompression")))
		{
			CString sCompression(NameOfEnum(COMPRESSION_5));
			CProcessMacro* pMacro;
			for (int i=0;i<theApp.GetProcessMacros().GetSize();i++)
			{
				pMacro = theApp.GetProcessMacros().GetAtFast(i);
				if (pMacro && pMacro->GetType() == PMT_ACTUAL_IMAGE)
				{
					sCompression = NameOfEnum(pMacro->GetCompression());
					break;
				}
			}
			DoConfirmGetValue(id,sKey,sCompression,dwServerData);
		}
		else if (0==sKey.CompareNoCase(_T("istresolution")))
		{
			CString sResolution(NameOfEnum(RESOLUTION_2CIF));
			CProcessMacro* pMacro;
			for (int i=0;i<theApp.GetProcessMacros().GetSize();i++)
			{
				pMacro = theApp.GetProcessMacros().GetAtFast(i);
				if (pMacro && pMacro->GetType() == PMT_ACTUAL_IMAGE)
				{
					sResolution = NameOfEnum(pMacro->GetResolution());
					break;
				}
			}
			DoConfirmGetValue(id,sKey,sResolution,dwServerData);
		}
	}
}

/*@MD NYD*/
void CIPCOutputServerClient::OnRequestSetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						)
{
	CHECK_ACCESS(CIPC_BASE_REQUEST_SET_VALUE);
	CIPCOutputServer *pOutput = theApp.GetOutputGroups().GetGroupByID(id);
	if (pOutput) 
	{
		if (sKey == CSD_V_OUT)
		{
			pOutput->GetProcessSchedulerVideo()->DoClientVideoOut(id);
		}
		else
		{
			pOutput->DoRequestSetValue(id,sKey,sValue,m_id.GetID());
		}
	}
	else 
	{
		WK_TRACE_ERROR(_T("OnRequestSetValue INVALID ID %x\n"),id.GetID());
	}
}

/*@MD NYD*/
void CIPCOutputServerClient::OnRequestStopH263Decoding(WORD wGroupID, 
													   const CRect &rect)
{

	CHECK_ACCESS(OUTP_REQUEST_STOP_H263_DECODING);	// possible EXIT

	CSecID id(wGroupID,0);	// no specific cam
	if (wGroupID==SECID_NO_GROUPID) {
		id = m_idDecoderColor;
	}
	CIPCOutputServer *pOutput = theApp.GetOutputGroups().GetGroupByID(id);
	if (pOutput) {
		pOutput->DoRequestStopH263Decoding(wGroupID,rect);
	} else {
		WK_TRACE_ERROR(_T("OnRequestStopH263Decoding INVALID ID %x\n"),wGroupID);
	}
}


/*@MD NYD*/
void CIPCOutputServerClient::OnRequestSync(DWORD dwTickSend, DWORD dwType,DWORD dwUserID)
{
	CHECK_ACCESS(CIPC_REQUEST_SYNC);
	// NOT YET use dwType
	DoConfirmSync(WK_GetTickCount(), dwTickSend, dwType, dwUserID);
}
/*@MD NYD*/
void CIPCOutputServerClient::RemoveProcess(CProcess *pProcess)
{
	if (GetIPCState() == CIPC_STATE_CONNECTED)
	{
		m_Processes.Lock(_T(__FUNCTION__));
		if (m_Processes.GetSize())
		{
			m_Processes.Remove(pProcess);
		}
		m_Processes.Unlock();
	}
}


/*@MD NYD*/
void CIPCOutputServerClient::OnIndicateClientActive(BOOL bClientIsActive, WORD wGroupID)
{
	// NOT YET use groupID
	CHECK_ACCESS(OUTP_INDICATE_CLIENT_ACTIVE);		// possible EXIT

	if (theApp.GetServerControl()) {
		theApp.GetServerControl()->SetActiveClient(this,bClientIsActive);
	}

}
/*@MHEAD{util (internal):} */
//////////////////////////////////////////////////////////////////////
void CIPCOutputServerClient::OnRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
	CHECK_ACCESS(CIPC_OUTPUT_REQUEST_GET_MASK);		// possible EXIT

	CPermission *pPermission = theApp.GetPermissions().GetPermission(GetConnection().GetPermission());

	if (    pPermission 
		&& (pPermission->GetFlags() & WK_ALLOW_HARDWARE)
		)
	{
		CIPCOutputServer *pOutput = theApp.GetOutputGroups().GetGroupByID(camID);
		if (pOutput)
		{
			pOutput->DoRequestGetMask(camID,
				type,
				m_id.GetID());
		}
		else
		{
			WK_TRACE_ERROR(_T("OnRequestGetMask %08lx no camera\n"),camID.GetID());
		}
	}
	else
	{
		DoIndicateError(GetLastCmd(),camID,CIPC_ERROR_ACCESS_DENIED,type);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputServerClient::OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
	CHECK_ACCESS(CIPC_OUTPUT_REQUEST_SET_MASK);		// possible EXIT

	CPermission *pPermission = theApp.GetPermissions().GetPermission(GetConnection().GetPermission());

	if (    pPermission 
		&& (pPermission->GetFlags() & WK_ALLOW_HARDWARE)
		)
	{
		CIPCOutputServer *pOutput = theApp.GetOutputGroups().GetGroupByID(camID);

		if (pOutput)
		{
			pOutput->DoRequestSetMask(camID,m_id.GetID(),mask);
		}
	}
	else
	{
		DoIndicateError(GetLastCmd(),camID,CIPC_ERROR_ACCESS_DENIED,mask.GetType());
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputServerClient::SendPicture(const CIPCPictureRecord& pict,CompressionType ct, Compression comp,DWORD dwMDX, DWORD dwMDY)
{
	CSecID idArchive;
	COutput* pOutput = theApp.GetOutputGroups().GetOutputByID(pict.GetCamID());
	if (pOutput)
	{
		idArchive = pOutput->GetCurrentArchiveID();
	}
	
	if (pict.GetCompressionType() == COMPRESSION_JPEG)
	{
		if (   UseH263()
			&& !IsMultiCamera()
			&& ct==COMPRESSION_H263)
		{
			m_csTranscoder.Lock();
			if (m_pTranscoder)
			{
				m_pTranscoder->AddPictureRecordH263(pict);
			}
			m_csTranscoder.Unlock();
		}
		else
		{
			DoConfirmJpegEncoding(pict,pict.GetCamID().GetID(),idArchive);
		}
	}
	else if (pict.GetCompressionType() == COMPRESSION_H263)
	{
		DoIndicateH263Data(pict,pict.GetCamID().GetID(),idArchive);
	}
	else if (pict.GetCompressionType() == COMPRESSION_YUV_422)
	{
		if (IsLocal())
		{
			DoConfirmJpegEncoding(pict,pict.GetCamID().GetID(),idArchive);
		}
		else
		{
			if (m_pEncoder)
			{
				CIPCFields fields;
				m_pEncoder->AddPictureRecord(comp,pict,fields,0,GetID().GetID(),
					CanMpeg4() ? COMPRESSION_MPEG4 : COMPRESSION_JPEG);
			}
		}
	}
	else if (pict.GetCompressionType() == COMPRESSION_MPEG4)
	{
		if (   IsLowBandwidth()
			&& CProcessSchedulerVideo::m_iTraceH263Data)
		{
			WK_TRACE(_T("DoIndicateVideo %08lx %d %s,%03d %s-Frame %s %d\n"),pict.GetCamID().GetID(),pict.GetBlockNr(),
				pict.GetTimeStamp().GetTime(),pict.GetTimeStamp().GetMilliseconds(),
				(pict.GetPictureType() == SPT_FULL_PICTURE)?_T("I"):(pict.GetPictureType() == SPT_GOP_PICTURE)?_T("G"):_T("P"),
				NameOfEnum(pict.GetResolution()),
				pict.GetDataLength());
		}
		if (pict.GetPictureType() == SPT_GOP_PICTURE)
		{
			// send the whole gop
			if (CanGOP())
			{
				DoIndicateVideo(pict,dwMDX,dwMDY,pict.GetCamID().GetID(),idArchive);
			}
			else
			{
				// send the single images
				m_GOPSender.AddPictureRecord(pict,dwMDX,dwMDY);
			}
		}
		else
		{
			DoIndicateVideo(pict,dwMDX,dwMDY,pict.GetCamID().GetID(),idArchive);
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCOutputServerClient::SelectCamera(CSecID camID)
{
	BOOL bRet = FALSE;
	if (GetIPCState() == CIPC_STATE_CONNECTED)
	{
		DWORD dwTick = WK_GetTickCount();
		if (   (m_dwSelectCameraTick == 0)
			|| (GetTimeSpan(m_dwSelectCameraTick,dwTick)>1000)
			|| camID == SECID_NO_ID
			)
		{
			WK_TRACE(_T("switch camera for existing alarm connection %s %08lx\n"),GetRemoteHost(),camID.GetID());
			DoRequestSelectCamera(camID);
			if (camID == SECID_NO_ID)
			{
			//	TRACE(_T("switch camera SECID_NO_ID %08lx\n"),camID.GetID());
			}
			else
			{
				m_dwSelectCameraTick = dwTick;
			}
			bRet = TRUE;
		}
		else
		{
			TRACE(_T("switch camera too fast for alarm connection %08lx\n"),camID.GetID());
		}
	}
	else
	{
		WK_TRACE(_T("switch camera for not connected alarm connection %08lx\n"),camID.GetID());
	}
	return bRet;
}

