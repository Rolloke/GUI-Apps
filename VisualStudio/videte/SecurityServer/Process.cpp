/* GlobalReplace: GetTickCount --> WK_GetTickCount */
/* GlobalReplace: CTimerClass --> CWK_Timer */
/* GlobalReplace: pApp\-\> --> theApp. */
/* GlobalReplace: Relais --> Relay */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: Process.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/Process.cpp $
// CHECKIN:		$Date: 16.08.06 17:04 $
// VERSION:		$Revision: 195 $
// LAST CHANGE:	$Modtime: 16.08.06 16:59 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"

#include "Process.h"
#include "ProcessMacro.h"
#include "ProcessListView.h"

#include "sec3.h"
#include "outputlist.h"
#include "inputlist.h"
#include "ProcessScheduler.h"

// for DoAlarm
#include "CIPCServerControlClientSide.h"
#include "CIPCServerControlServerSide.h"
#include "CIPCInputServerClient.h"
#include "CIPCOutputServerClient.h"
#include "input.h"
#include "CipcDatabaseClientServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


LPCTSTR NameOfEnum(SecProcessType type)
{
	switch (type)
	{
	case SPT_NONE:
		return _T("SPT_NONE");
		break;
	case SPT_ENCODE:
		return _T("SPT_ENCODE");
		break;
	case SPT_DECODE:
		return _T("SPT_DECODE");
		break;
	case SPT_VIDEO:
		return _T("SPT_VIDEO");
		break;
	case SPT_RELAY:
		return _T("SPT_RELAY");
		break;
	case SPT_ALARM_CALL:
		return _T("SPT_ALARM_CALL");
		break;
	case SPT_BACKUP:
		return _T("SPT_BACKUP");
		break;
	case SPT_ACTUAL_IMAGE:
		return _T("SPT_ACTUAL_IMAGE");
		break;
	case SPT_IMAGE_CALL:
		return _T("SPT_IMAGE_CALL");
		break;
	case SPT_CHECK_CALL:
		return _T("SPT_CHECK_CALL");
		break;
	case SPT_GUARD_TOUR:
		return _T("SPT_GUARD_TOUR");
		break;
	case SPT_VIDEO_OUT:
		return _T("SPT_VIDEO_OUT");
		break;
	case SPT_AUDIO:
		return _T("SPT_AUDIO");
		break;
	case SPT_PTZ_PRESET:
		return _T("SPT_PTZ_PRESET");
		break;
	case SPT_ALARM_LIST:
		return _T("SPT_ALARM_LIST");
		break;
	case SPT_UPLOAD:
		return _T("SPT_UPLOAD");
		break;
	default:
		return _T("UNKNOWN");
		break;
	}
}
LPCTSTR NameOfEnum(SecProcessState state)
{
	switch (state)
	{
	case SPS_INITIALIZED:
		return _T("SPS_INITIALIZED");
		break;
	case SPS_ACTIVE_RECORDING:
		return _T("SPS_ACTIVE_RECORDING");
		break;
	case SPS_ACTIVE_IDLE:
		return _T("SPS_ACTIVE_IDLE");
		break;
	case SPS_PAUSED:
		return _T("SPS_PAUSED");
		break;
	case SPS_TERMINATED:
		return _T("SPS_TERMINATED");
		break;
	}
	return _T("");
}

/////////////////////////////////////////////////////////////////////////////
// OOPS only used local in this file
#define SECID_GROUP_TMP_PROCESSES (SECID_GROUP_PROCESS+124)

WORD CProcess::m_tmpProcessCounter=0;
int CProcess::m_iTraceLevel=0;	// 0 normal few messages
								// 1 detail messages
								// 99 nothing at all

BOOL CProcess::m_bTraceProcessTermination=FALSE;
BOOL CProcess::m_bAdjustStartTime = FALSE;
BOOL CProcess::m_bTraceProcessRun = FALSE;
BOOL CProcess::m_bTraceProcesses = FALSE;
/////////////////////////////////////////////////////////////////////////////
void CProcess::InitTimer()
{
	if (m_pActivation->m_idTimer!=SECID_NO_ID) 
	{
		// GetTimerByID is thread-safe
		m_pTimer = theApp.GetTimers().GetTimerByID(m_pActivation->m_idTimer);

		if (m_pTimer==NULL) 
		{
			WK_TRACE_ERROR(_T("Timer %x not found\n"),m_pActivation->m_idTimer.GetID());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CProcess::CProcess(CProcessScheduler* pScheduler)
{
	m_pProcessScheduler = pScheduler;
	m_id.Set(SECID_GROUP_TMP_PROCESSES,++m_tmpProcessCounter);
//	WK_TRACE(_T("PID %08lx\n"),m_id.GetID());
	m_bHadStatLog=FALSE;

	m_tickInit=0;	// set when initialized
	m_tickIntervalStart=0;
	m_tickSliceStart=0;
	m_tickTerminated=0;
	m_bMarkedForTermination = FALSE;


	m_state = SPS_INITIALIZED;

	m_dwDropCounter = 0;

	m_pTimer = NULL;
	m_pActivation = NULL;
	//
	m_CompressionType=COMPRESSION_UNKNOWN;
	
	m_type=SPT_NONE;


	m_iNumVirtualSeconds=0;
	
	m_csPics.Lock();
	m_iNumPicturesReceived = 0;
	m_iNumSlicePicturesReceived=0;
	m_csPics.Unlock();

	m_pRelay = NULL;

	m_iNumPicturesRequested = 0;
}
/////////////////////////////////////////////////////////////////////////////
CProcess::~CProcess()
{
	if (m_state!=SPS_TERMINATED) 
	{
		WK_TRACE_ERROR(_T("process not terminated in destructor\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CProcess::Terminate(BOOL bShutDown)
{
	if (m_state!=SPS_TERMINATED) 
	{
		SetState(SPS_TERMINATED,WK_GetTickCount(),bShutDown);
	}

	if (m_bTraceProcessTermination) 
	{
		DWORD dwRealTime = m_tickTerminated-m_tickInit;
		if (m_iNumVirtualSeconds==0) 
		{
			dwRealTime=0;
		}
		WK_TRACE(_T("Process [%s] terminated after %d VirtualSeconds within %d realtime ms, %d pictures received (expected %d) pics/slice %d/%d (requested %d)\n"),
			(LPCTSTR)GetMacro().GetName(),
			m_iNumVirtualSeconds, 
			dwRealTime,
			m_iNumPicturesReceived,
			GetMacro().GetNumExpectedPictures(),
			GetMacro().GetPicsPerSlice(),
			GetMacro().GetRecordSlice(),
			m_iNumPicturesRequested
			);
	
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CSecID CProcess::GetClientID() const
{
	return SECID_NO_ID;
}
/////////////////////////////////////////////////////////////////////////////
CString	CProcess::GetStateName() const
{
	if (GetActivation()->GetTypeOfActivation()==TOA_INPUT_ALARM) 
	{
		return _T("ALARM");
	} 
	else 
	{
		return _T("OK");
	}
}
/////////////////////////////////////////////////////////////////////////////
CString	CProcess::Format()
{
	CString sRet;
	CSecID camID;
	if (GetActivation())
	{
		camID = GetActivation()->GetOutputID();
	}

	sRet.Format(_T("Cam ID=%08lx,TYP=%s,STATE=%s,PICRec=%d,PICSliceRec=%d"),
		camID.GetID(),NameOfEnum(m_type),NameOfEnum(m_state),m_iNumPicturesReceived,m_iNumSlicePicturesReceived);

	return sRet;
}
/////////////////////////////////////////////////////////////////////////////
CString	CProcess::GetDescription(BOOL bActive)
{
	CString sText;

	if (   m_state != SPS_TERMINATED
		&& m_pActivation)
	{
		CString sState  = _T("XX");
		CString sInput  = _T("XX");
		CString sArchiv = _T("XX");
		
		CSystemTime st;
		st.GetLocalTime();
		CString sTime;
		sTime = st.GetDateTime();
		
		
		if (bActive && GetAlarmInput()) 
		{
			sInput = GetAlarmInput()->GetName();
		}
		
		sState = GetStateName();
		
		CString sOutput;
		const COutput* pOutput = NULL;
		
		// get the camera/relais name
		if (bActive) 
		{
			CSecID id = GetCamID();
			if (id.IsOutputID())
			{
				pOutput = theApp.GetOutputGroups().GetOutputByID(id);
				if (pOutput) 
				{
					sOutput.Format(_T("%s"),pOutput->GetName());
				}
			}
			else if (id.IsMediaID())
			{
/*				const CMedia*  pMedia = NULL;
				pMedia = theApp.GetAudioGroups().GetAudioByID(id);
				if (pMedia) 
				{
					sOutput.Format(_T("%s"),pMedia->GetName());
				}
*/
				sOutput.Format(_T("%s"),theApp.GetAudioGroups().GetAudioNameByID(id));
			}
		}
		
		if (GetActivation()) 
		{
			if (GetMacro().GetType()==PMT_RECORD) 
			{
				WORD wArchiveID = GetActivation()->GetArchiveID().GetSubID();
				if (wArchiveID!=SECID_NO_SUBID) 
				{
					sArchiv.Format(_T("ar. %d"),wArchiveID);
					if (theApp.GetDatabase())
					{
						CIPCArchivRecord* pArchiv = theApp.GetDatabase()->GetRecords().GetArchiv(wArchiveID);
						if (pArchiv)
						{
							sArchiv = pArchiv->GetName();
						}
					}
					sText.Format(_T("%s (%s): %s , %s --> %s [%s] pr. %d {%d pics/%d ms,%s,%s}"),
						sState,
						sTime,
						sInput,
						sOutput,
						GetMacro().GetName(),
						sArchiv,
						GetActivation()->GetPriority()+1,
						GetMacro().GetPicsPerSlice(),
						GetMacro().GetRecordSlice(),
						NameOfEnum(GetMacro().GetCompression()),
						NameOfEnum(GetMacro().GetResolution())
						);
				}
				else 
				{
					// no sspecific archive given
					sText.Format(_T("%s (%s) : %s , %s --> %s pr. %d {%d pics/ %d ms,%s,%s}"),
						sState,
						sTime,
						sInput,
						sOutput,
						GetMacro().GetName(),
						GetActivation()->GetPriority()+1,
						GetMacro().GetPicsPerSlice(),
						GetMacro().GetRecordSlice(),
						NameOfEnum(GetMacro().GetCompression()),
						NameOfEnum(GetMacro().GetResolution())
						);
				}
			}
			else if (GetMacro().GetType()==PMT_RELAY) 
			{
				// NOT YET
				sText.Format(_T("%s (%s):%s,%s-->%s "),
					sState,
					sTime,
					sInput,
					sOutput,
					GetMacro().GetName()
					);
				// wenn es die Standard FlankenSteuerung ist
				// oder wenn der client EDGE hat
				// zeige die EdgeZeit an.
				if (pOutput) 
				{
					if (	GetMacro().GetID()==SECID_PROCESS_RELAY_EDGE_CONTROL
						||  (   IsClientProcess() 
						     && pOutput->GetRelayControlType()==RCT_EDGE) 
						) 
					{
						CString sClosingTime;
						sClosingTime.Format(_T("[%d s]"),pOutput->GetRelayClosingTime());
						sText += sClosingTime;
					}
				}
				
			} 
			else if (GetMacro().GetType()==PMT_ALARM_CALL) 
			{
				sText.Format(_T("%s (%s):%s,%s-->%s "),
					sState,
					sTime,
					sInput,
					sOutput,
					GetMacro().GetName()
					);
			}
			else if (GetMacro().GetType()==PMT_BACKUP) 
			{
				if (theApp.GetDatabase())
				{
					WORD wArchivNr = GetMacro().GetBackupArchiveID().GetSubID();
					CIPCArchivRecord* pArchiv = theApp.GetDatabase()->GetRecords().GetArchiv(wArchivNr);
					if (pArchiv)
					{
						sArchiv = pArchiv->GetName();
					}
				}
				sText.Format(_T("%s (%s):%s, %s, %s->%s, "),
					sState,
					sTime,
					sInput,
					GetMacro().GetName(),
					sArchiv,
					GetMacro().GetBackupDestination()
					);
			}
			else if (GetMacro().GetType()==PMT_ACTUAL_IMAGE) 
			{
				sText.Format(_T("%s (%s):%s, %s, Actual Image"),
					sState,
					sTime,
					sInput,
					GetMacro().GetName());
			}
			else if (GetMacro().GetType()==PMT_IMAGE_CALL) 
			{
				sText.Format(_T("%s (%s):%s, %s, Image Call"),
					sState,
					sTime,
					sInput,
					GetMacro().GetName());
			}
			else if (GetMacro().GetType()==PMT_CHECK_CALL) 
			{
				sText.Format(_T("%s (%s):%s, %s, Check Call"),
					sState,
					sTime,
					sInput,
					GetMacro().GetName());
			}
			else if (GetMacro().GetType()==PMT_GUARD_TOUR) 
			{
				sText.Format(_T("%s (%s):%s, %s, Guard Tour"),
					sState,
					sTime,
					sInput,
					GetMacro().GetName());
			}
			else if (GetMacro().GetType() == PMT_VIDEO_OUT)
			{
				CString sType,sCard;
				switch (GetMacro().GetVideoOutType())
				{
				case 0:
					sType = _T("client");
					break;
				case 1:
					sType = _T("sequence");
					break;
				case 2:
					sType = _T("activity");
					break;
				}

				CIPCOutputServer* pCIPC = theApp.GetOutputGroups().GetGroupByID(CSecID(GetMacro().GetVideoOutGroupID(),0));
				if (pCIPC)
				{
					sCard = pCIPC->GetName();
				}

				sText.Format(_T("%s (%s):%s, %s, VOUT %s %s %d -> Port %d"),
					sState,
					sTime,
					sInput,
					GetMacro().GetName(),
					sType,
					sCard,
					(WORD)(GetActivation()->GetOutputID().GetSubID()+1),
					GetMacro().GetVideoOutPort());
			}
			else if (GetMacro().GetType() == PMT_AUDIO)
			{
				// TODO AUDIO
				if (IsClientProcess())
				{
				}
				else
				{
					WORD wArchiveID = GetActivation()->GetArchiveID().GetSubID();
					if (wArchiveID!=SECID_NO_SUBID) 
					{
						sArchiv.Format(_T("ar. %d"),wArchiveID);
						if (theApp.GetDatabase())
						{
							CIPCArchivRecord* pArchiv = theApp.GetDatabase()->GetRecords().GetArchiv(wArchiveID);
							if (pArchiv)
							{
								sArchiv = pArchiv->GetName();
							}
						}
					}
					sText.Format(_T("Audio %s (%s): %s , %s --> %s [%s] pr. %d"),
							sState,
							sTime,
							sInput,
							sOutput,
							GetMacro().GetName(),
							sArchiv,
							GetActivation()->GetPriority()+1);
				}
			}
			else if (GetMacro().GetType() == PMT_UPLOAD)
			{
				sText.Format(_T("%s (%s): %s , %s --> %s pr. %d {%d pics/%d ms %d Pause,%s,%s}"),
					sState,
					sTime,
					sInput,
					sOutput,
					GetMacro().GetName(),
					GetActivation()->GetPriority()+1,
					GetMacro().GetPicsPerSlice(),
					GetMacro().GetRecordSlice(),
					GetMacro().GetPauseTime()/1000,
					NameOfEnum(GetMacro().GetCompression()),
					NameOfEnum(GetMacro().GetResolution()));
			}
			else if (GetMacro().GetType() == PMT_PTZ_PRESET)
			{
				CameraControlCmd ccc = GetActivation()->GetCameraControlCmd();
				sText.Format(_T("PTZ Preset %s (%s): %s , %s --> %s"),
					sState,
					sTime,
					sInput,
					sOutput,
					NameOfEnum(ccc));
			}
			else 
			{
				sText = _T("unknown process");
			}
			
			if (GetTimer()) 
			{
				sText += _T(" Timer: ");
				sText += GetTimer()->GetName();
			}
		}
	}

	return sText;
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::Restart()
{
//	TRACE(_T("CProcess::Restart %s %s\n"),GetDescription(TRUE),NameOfEnum(GetState()));
	SetState(SPS_INITIALIZED,WK_GetTickCount());
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::Run()
{
	// is called every second from scheduler
	WK_TRACE(_T("CProcess::Run() not overwritten\n"));
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::OnPause()
{
	WK_TRACE(_T("CProcess::OnPause() not overwritten\n"));
}
//////////////////////////////////////////////////////////////////////
void CProcess::OnModified(BOOL bNewOutputID)
{
}
//////////////////////////////////////////////////////////////////////
void CProcess::OnUnitDisconnected()
{
}
//////////////////////////////////////////////////////////////////////
void CProcess::PictureData(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY,DWORD dwUserID)
{
}
//////////////////////////////////////////////////////////////////////
void CProcess::MediaData(const CIPCMediaSampleRecord& rec, CSecID idMedia)
{
}
//////////////////////////////////////////////////////////////////////
void CProcess::InitProtocolFile()
{
	CWK_Profile prof;

	CString sDir = prof.GetString(SECTION_LOG, LOG_PROTOCOLPATH, DEFAULT_PROTOCOLPATH);
	WK_CreateDirectory(sDir);

	CString sFile,sTime;
	CSystemTime st;
	st.GetLocalTime();
	sTime = st.GetDBDate() + st.GetDBTime();

	sFile.Format(_T("%s\\%s%s.htm"),sDir,GetMacro().GetName(),sTime);
	sFile.Replace(_T("?"), _T("_"));

	CString sTitle, sMetaCodePage, sMetaGenerator;

	sTitle.LoadString(IDS_PROTOCOL);
	sTitle += _T(": ");
	sTitle += GetMacro().GetName();
	sTitle += _T(" ");
	sTitle += st.GetDateTime();

	UINT nCodePage = CWK_String::GetCodePage();
	m_sfProtocolFile.SetFileMCBS((WORD)CWK_String::GetCodePage());
	if (nCodePage != CP_ACP)
	{
		sMetaCodePage.Format(HTML_CODEPAGE_METATAG, theApp.GetHTMLCodePage(nCodePage));
	}
	sMetaGenerator.Format(HTML_GENERATOR_METATAG, theApp.m_pszAppName);

	if (m_sfProtocolFile.Open(sFile,CFile::modeCreate|CFile::modeWrite))
	{
		WriteProtocol(_T("<html>\n"));
		WriteProtocol(_T(" <head>\n"));
		if (nCodePage != CP_ACP)
		{
			WriteProtocol(sMetaCodePage);
		}
		WriteProtocol(sMetaGenerator);
		WriteProtocol(_T("  <title>\n"));
		WriteProtocol(sTitle);
		WriteProtocol(_T("  </title>\n"));
		WriteProtocol(_T(" </head>\n"));
		WriteProtocol(_T("<body>\n"));
		WriteProtocol(_T("<h2>")+sTitle+_T("</h2>\n"));
	}
	else 
	{
		WK_TRACE_ERROR(_T("cannot create protocol file %s\n"),
						GetMacro().GetName());
	}
}
//////////////////////////////////////////////////////////////////////
void CProcess::ExitProtocolFile()
{
	WriteProtocol(_T("</body>\n"));
	WriteProtocol(_T("</html>\n"));
	if (m_sfProtocolFile.m_pStream)
	{
		TRY
		{
			m_sfProtocolFile.Flush();
			m_sfProtocolFile.Close();
		}
		WK_CATCH (_T(""));
		CString sPath = m_sfProtocolFile.GetFilePath();
		int p;
		p = sPath.ReverseFind(_T('\\'));
		sPath = sPath.Left(p);
		CheckDiskSpaceAndCleanUp(5*1024,sPath,_T("*.htm"));
	}
}
//////////////////////////////////////////////////////////////////////
void CProcess::WriteProtocol(const CString& sLine)
{
	if (m_sfProtocolFile.m_pStream)
	{
		TRY
		{
			m_sfProtocolFile.WriteString(sLine);
		}
		WK_CATCH (_T("cannot write html protocol"));
	}
	else
	{
		WK_TRACE_ERROR(_T("WriteProtocol file not opened\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::DoStatLog(int iValue)
{
	// mark if the process did some trace
	// so created and directly destroyed should not do 0 stats
	m_bHadStatLog = TRUE;
	if (m_bTraceProcesses)
	{
		if (GetActivation()) 
		{
			CString sName = GetMacro().GetName();
			
			if (GetActivation()->GetInput()) 
			{
				sName += _T("|") + GetActivation()->GetInput()->GetName();
			}
			if (GetActivation()->GetOutputID() != SECID_NO_ID)
			{
				const COutput *pOutput = theApp.GetOutputGroups().GetOutputByID(
										GetActivation()->GetOutputID()
										);
				if (pOutput) 
				{
					sName += _T("|") + pOutput->GetName();
				}
			}

			WK_STAT_LOG(_T("Process"),iValue,sName);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::CalcMode(DWORD cti)
{
	m_iNumVirtualSeconds++;

	if (   m_bTraceProcessRun 
		&& !IsClientProcess()
		&& GetMacro().GetNumExpectedPictures()) 
	{
		WK_TRACE(_T("Process [%s] running %d VirtualSeconds, %d pictures received [in slice %d](expected %d)(%d requested)\n"),
			(LPCTSTR)GetMacro().GetName(),
			m_iNumVirtualSeconds, 
			m_iNumPicturesReceived,
			m_iNumSlicePicturesReceived,
			GetMacro().GetNumExpectedPictures(),
			m_iNumPicturesRequested
			);
	}

	if (m_state==SPS_TERMINATED) 
	{	// pre-check, won't change any more
		return;	// <<< EXIT >>>
	}

	if (m_state==SPS_INITIALIZED) 
	{
		if (m_iTraceLevel==0) 
		{
			StatLog(1);	// special for on/off tracing
		}
		SetState(SPS_ACTIVE_RECORDING,cti);
		m_tickInit = cti;
		m_tickIntervalStart=cti;
		m_iNumVirtualSeconds=1;
	}

	// NEW 240797 never exceed the expected picture count
	// same condition already handle in the ProcessScheduler, 
	// when a picture is received. Thus this code here should be rarely used.
	DWORD dwExpectedPictures = GetMacro().GetNumExpectedPictures();
	
	if (   dwExpectedPictures 
		&& !IsClientProcess() 
		&& GetPictureCount() >= (int) dwExpectedPictures 
		) 
	{
		WK_TRACE(_T("Process[%s] already got all the (%d) pictures\n"),
			(LPCTSTR)GetMacro().GetName(),
			dwExpectedPictures 
			);	// OBSOLETE
		SetState(SPS_TERMINATED,cti);
	}
	
	// Time Conditions:
	// a) an IntervallProcess can be limited by its OverallTime
	// b) a Non-Intervall is limited by its RecordTime
	DWORD dwElapsedTime = cti-m_tickInit;
	if (IsRelayProcess()) 
	{
		// the standard edge-macro is used for different relays
		// each with it's own time settings
		// thus GetOverallTime has to be lookep up in the related relay
		if (   GetMacro().GetID()==SECID_PROCESS_RELAY_EDGE_CONTROL
			|| (   IsClientProcess() 
			    && GetRelay()->GetRelayControlType()==RCT_EDGE)
			) 
		{
			DWORD dwOverallTime = GetRelay()->GetRelayClosingTime()*1000;
			if (dwElapsedTime >= dwOverallTime) 
			{
				TRACE(_T("relay %s with edge control, hold %u elapsed %u, terminate\n"),
							GetRelay()->GetName(), dwOverallTime, dwElapsedTime);
				SetState(SPS_TERMINATED,cti);
			}
		}
	} 
	else if (   IsBackupProcess()
			 || IsImageCallProcess()
			 || IsActualImageProcess()
			 || IsCheckCallProcess()
			 || IsGuardTourProcess()
			 || IsVideoOutProcess()
			 )
	{
		// still staying recording
	}
	else 
	{
		// non relay processes
		// check overall time
		if (GetMacro().IsIntervall()) 
		{
			 if (GetMacro().GetOverallTime() != CPM_TIME_INFINITY
				&& dwElapsedTime >= GetMacro().GetOverallTime()) 
			 {
				 // limited intervall elapsed
				 SetState(SPS_TERMINATED,cti);
			 }
		} 
		else if (dwElapsedTime  >= GetMacro().GetRecordTime()) 
		{
			// non-intervall elapsed
			SetState(SPS_TERMINATED,cti);
		}
	}

	// limited by Timer ?
	if (   m_state != SPS_TERMINATED 
		&& m_pTimer 
		&& !m_pTimer->IsIncluded(CTime::GetCurrentTime())
		) 
	{
		if (   !IsBackupProcess()
			&& !IsCheckCallProcess()
			&& (   !IsGuardTourProcess()
			    || !GetMacro().IsKeepAlive())
			)
		{
			// outside of timer
			SetState(SPS_TERMINATED,cti);
		}
	}

	if (m_state==SPS_TERMINATED) 
	{
		return;	// <<< EXIT >>>
	}

	// record/pause check
	if (   m_state == SPS_ACTIVE_RECORDING 
		|| m_state == SPS_ACTIVE_IDLE) 
	{
		// record time done ?
		// OOPS +-100
		if (cti-m_tickIntervalStart+100 >= GetMacro().GetRecordTime()) 
		{
			// increment the host between the pause times
			if (GetMacro().GetPauseTime()) 
			{
				SetState(SPS_PAUSED,cti);
			} 
			else 
			{	// stay active but force peak
				StatLog(3);		// force back peak
				SetState(SPS_ACTIVE_RECORDING,cti);
				m_tickIntervalStart=cti;	// it's a new intervall
			}
		} 
		else if (cti-m_tickSliceStart >= GetMacro().GetRecordSlice()) 
		{
			// one slice done
			StatLog(3);		// force back peak
			SetState(SPS_ACTIVE_RECORDING,cti);
		} 
		else 
		{
			// stay in current mode

			// NEW 041797, path
			// special condition
			// during a small virtual second
			// a slice might be activated twice
			// if all picture are already received, switch to idle mode
#if 0
			// cant't do that, pictures arrive asynchron
			if (m_state==SPS_ACTIVE_RECORDING
				&& GetClient()==NULL
				&& GetMacro().IsIntervall()
				&& (DWORD) m_iNumSlicePicturesReceived >= GetMacro().GetPicsPerSlice()
				) {
				m_iNumSlicePicturesReceived = 0;
				SetState(SPS_ACTIVE_IDLE,cti);

				if (m_bTraceProcessTermination) {	// OOPS no own debugFlag
					WK_TRACE(_T("Process[%s] already got all (%d) pictures for this slice delta %d\n"),
									(LPCTSTR)GetMacro().GetComment(),
									GetMacro().GetPicsPerSlice(),
									cti - m_tickIntervalStart
									);	// OBSOLETE
					WK_STAT_PEAK(_T("Process"),1,_T("AllPicturesDone"));
				}

			}
#endif
		}
	} 
	else if (m_state==SPS_PAUSED) 
	{
		ASSERT(GetMacro().IsIntervall());
		// OOPS +- 100 ms
		if ((cti - m_tickIntervalStart)+100 >= 
				(GetMacro().GetPauseTime()+GetMacro().GetRecordTime()) ) {
			// one interval done
			SetState(SPS_ACTIVE_RECORDING,cti);
			m_tickIntervalStart=cti;
		}
	}
	// mode changes from ACTIVE_RECORDING to ACTIVE_IDLE are
	// done by the ProcessScheduler
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::CleanUp()
{
	if (!IsClientProcess())
	{
		Lock(_T(__FUNCTION__));
		if (m_pActivation) 
		{
			if (m_pActivation->GetActiveProcess()==this) 
			{
				m_pActivation->SetActiveProcess(NULL);
				// RKE: hier ist das Inputobject manchmal ungültig
				m_pActivation->GetInput()->ProcessTerminated(this);
			} 
			else 
			{
				// OOPS some other process ?
				if (   theApp.IsUpAndRunning()
					&& m_pActivation->GetActiveProcess()!=NULL)
				{
					WK_TRACE_ERROR(_T("IBL %s %08lx!=%08lx\n"),NameOfEnum(m_type),m_pActivation->GetActiveProcess(),this);
					WK_TRACE_ERROR(_T("IBL process %s\n"),m_pActivation->GetActiveProcess()->GetDescription(FALSE));
					WK_TRACE_ERROR(_T("IBL this %s\n"),GetDescription(FALSE));
				}
			}
			// OOPS keep back link, for incoming picture data
			// m_pActivation = NULL;
		}
		Unlock();
	}
}
/////////////////////////////////////////////////////////////////////////////
// set new state and according tick values
void CProcess::SetState(SecProcessState s, DWORD cti, BOOL bShutDown/*=FALSE*/)
{
	Lock(_T(__FUNCTION__));
	if (m_state != s)
	{
		OnStateChanging(s,bShutDown);
	}

	switch (s) 
	{
		case SPS_INITIALIZED:	// to restart keep alive
			m_state=SPS_INITIALIZED;
			m_csPics.Lock();
			m_iNumSlicePicturesReceived = 0;
			m_iNumPicturesReceived = 0;
			m_csPics.Unlock();
			StatLog(1);
			// NOT YET other values ?
			break;
		case SPS_PAUSED:
			m_state=SPS_PAUSED;
			m_dwDropCounter = 0;
			StatLog(2);
			break;
		case SPS_ACTIVE_RECORDING:
			m_tickSliceStart = cti;
			m_state=SPS_ACTIVE_RECORDING;
			m_iNumSlicePicturesReceived = 0;
			StatLog(4);
			break;
		case SPS_ACTIVE_IDLE:
			// m_tickSliceStart is still used to check for pauses
			m_state=SPS_ACTIVE_IDLE;
			StatLog(3);
			break;
		case SPS_TERMINATED:
			m_state=SPS_TERMINATED;
			m_tickTerminated = cti;

			UpdateProcessListView(FALSE,this);
			if (m_bHadStatLog) {
				DoStatLog(0);
			}
			CleanUp();
			break;
		default:
			WK_TRACE_ERROR(_T("invalid process state\n"));
			ASSERT(0);
	}
	Unlock();
}
/////////////////////////////////////////////////////////////////////
void CProcess::OnStateChanging(SecProcessState newState, BOOL bShutDown)
{
	// to be overridden in derived classes
}
/////////////////////////////////////////////////////////////////////
BOOL CProcess::Modify(CSecID camID, Resolution res, Compression comp, 
					  int iNumPictures, DWORD dwClientTimeSlice, DWORD dwBitrateScale)
{
	Lock(_T(__FUNCTION__));

	BOOL bNewOutputID = m_pActivation->GetOutputID() != camID;
	BOOL bNewNumPictures = m_pActivation->GetMacro().GetNumPictures() != (DWORD)iNumPictures;
	BOOL bNewTimeSlice = m_pActivation->GetMacro().GetRecordTime() != dwClientTimeSlice;
	BOOL bNewResolution = m_pActivation->GetMacro().GetResolution() != res;
	BOOL bNewCompression = m_pActivation->GetMacro().GetCompression() != comp;

	CProcessMacro &macro = GetActivation()->GetTmpMacro();
	m_pActivation->SetOutputID(camID);
	macro.SetPictureParameter(comp,res);

	// force seconds
	if (dwClientTimeSlice) {
		dwClientTimeSlice = (dwClientTimeSlice / 1000)*1000;
	} else {
		dwClientTimeSlice=1000;
	}

	// bound check
	if (dwClientTimeSlice==0) {
		iNumPictures=1;
		dwClientTimeSlice=1000;
	} else if (dwClientTimeSlice > 1000*60) {
		WK_TRACE_ERROR(_T("ClientTimeSlice overflow %d\n"),dwClientTimeSlice);
		iNumPictures=1;
		dwClientTimeSlice=1000;
	}

	macro.SetRecordValues(dwClientTimeSlice,iNumPictures);

	OnModified(bNewOutputID);

	Unlock();

	BOOL bNew = bNewNumPictures || bNewOutputID || bNewTimeSlice || bNewResolution || bNewCompression;

	if (bNew)
	{
		UpdateProcessListView(TRUE,this);
	}

	return bNew;
	
}
CProcesses::CProcesses()
{
#ifdef TRACE_LOCKS
	m_nCurrentThread = theApp.m_bTraceLocks ? 0 : -1;
#endif
}
/////////////////////////////////////////////////////////////////////////////
CProcess* CProcesses::GetByID(CSecID id)
{
	Lock(_T(__FUNCTION__));
	for (int i=0;i<GetSize();i++) 
	{
		CProcess *p = GetAtFast(i);
		if (p->GetID()==id) 
		{
			Unlock();	// before EXIT
			return p;	// EXIT
		}
	}
	Unlock();
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::AddAlarmFields(CIPCFields& fields, int iGOPLength)
{
	CInput* pInput = GetAlarmInput();
	if (pInput)
	{
		// Daten nur bei Alarmprozessen
		if (   GetActivation() 
			&& GetActivation()->GetTypeOfActivation()==TOA_INPUT_ALARM)
		{
			pInput->CopyFields(fields);
			if (pInput->IsSDI())
			{
				pInput->DeleteFirstFields(iGOPLength,FIELD_DBD_POS);
			}
		}
	} 
}
/////////////////////////////////////////////////////////////////////////////
void CProcess::AddDefaultFields(CIPCFields& fields)
{
	if (GetAlarmInput())
	{
		// now add input name
		fields.SafeAdd(new CIPCField(CIPCField::m_sfInNm,
									 GetAlarmInput()->GetName(),
									 32,
									 'C')
					   );
	}
	// now add camera  name
	CSecID cam = GetActivation()->GetOutputID();
	const COutput* pOutput = theApp.GetOutputGroups().GetOutputByID(cam);
	if (pOutput)
	{
		fields.SafeAdd(new CIPCField(CIPCField::m_sfCaNm,
									 pOutput->GetName(),
									 32,
									 'C')
					   );
	}
	// now add station name
	fields.SafeAdd(new CIPCField(CIPCField::m_sfStNm,
								 theApp.GetHosts().GetLocalHostName(),
								 32,
								 'C')
				   );
}

