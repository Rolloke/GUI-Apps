// Settings.cpp: implementation of the CSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static _TCHAR szSection[] = _T("Process");
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSettings::CSettings()
{
	m_bTraceClientConfirms = FALSE;
	m_bTraceClientRequests = FALSE;
	m_bTraceUnitConfirms = FALSE;
	m_bTraceUnitRequests = FALSE;
	m_bTraceScan = FALSE;
	m_bTraceSync = FALSE;
	m_bTraceSave = FALSE;
	m_bTraceH263 = FALSE;
	m_bTraceVOUT = FALSE;
	m_bStatSwitch = FALSE;

	m_eTrackBehaviour = FIRST_POST_ALARM;
	m_iNrOfCameras = 0;
	m_iPreMD = 2;
	m_iPostMD = 3;
	m_iSuspect = 10;
	m_iNrAlarmSeconds = 60*15;
	m_bLocalHD = TRUE;
	m_bRemovable = FALSE;
	m_bAlarmList = FALSE;
	m_bRequestDIBs = TRUE;
	m_bIgnoreExternalDetectors = FALSE;

	m_iJPEGQuality = 80;
	m_iJPEGSize = 50*1024;

	m_Resolution = RESOLUTION_2CIF;
	
	m_bAudioIn = FALSE;
	m_bAudioOut = FALSE;
	m_dwAudioRecordingBits = 0;

	m_bMPEG4 = TRUE;
	m_bUseAVCodec = TRUE;
}

CSettings::~CSettings()
{

}
/////////////////////////////////////////////////////////////////////////////
void CSettings::ReadSettings(CWK_Profile& wkp)
{
	m_iNrOfCameras = wkp.GetInt(szSection,_T("NrOfCameras"),m_iNrOfCameras);
	m_iPreMD = wkp.GetInt(szSection,_T("PreMD"),m_iPreMD);
	if (m_iPreMD>10)
	{
		m_iPreMD = 10;
	}
	m_iPostMD = wkp.GetInt(szSection,_T("PostMD"),m_iPostMD);
	if (m_iPostMD>10)
	{
		m_iPostMD = 10;
	}
	m_iSuspect = wkp.GetInt(szSection,_T("Suspect"),m_iSuspect);
	if (m_iSuspect>20)
	{
		m_iSuspect = 20;
	}
	m_iNrAlarmSeconds = wkp.GetInt(szSection,_T("AlarmSeconds"),m_iNrAlarmSeconds);

	m_bLocalHD = wkp.GetInt(szSection,_T("LocalHD"),m_bLocalHD);
	m_bRemovable = wkp.GetInt(szSection,_T("Removable"),m_bRemovable);
	m_bAlarmList = TRUE;//wkp.GetInt(szSection,_T("AlarmList"),m_bAlarmList);
	m_eTrackBehaviour = (TrackBehaviour)wkp.GetInt(szSection,_T("Track"),m_eTrackBehaviour);

	m_bTraceClientConfirms = wkp.GetInt(szSection,_T("TraceClientConfirms"),m_bTraceClientConfirms);
	m_bTraceClientRequests = wkp.GetInt(szSection,_T("TraceClientRequests"),m_bTraceClientRequests);
	m_bTraceUnitConfirms = wkp.GetInt(szSection,_T("TraceUnitConfirms"),m_bTraceUnitConfirms);
	m_bTraceUnitRequests = wkp.GetInt(szSection,_T("TraceUnitRequests"),m_bTraceUnitRequests);
	m_bTraceSave = wkp.GetInt(szSection,_T("TraceSave"),m_bTraceSave);
	m_bTraceScan = wkp.GetInt(szSection,_T("TraceScan"),m_bTraceScan);
	m_bTraceSync = wkp.GetInt(szSection,_T("TraceSync"),m_bTraceSync);
	m_bTraceH263 = wkp.GetInt(szSection,_T("TraceH263"),m_bTraceH263);
	m_bTraceVOUT = wkp.GetInt(szSection,_T("TraceVOUT"),m_bTraceVOUT);
	m_bStatSwitch = wkp.GetInt(szSection,_T("StatSwitch"),m_bStatSwitch);

	m_bRequestDIBs = wkp.GetInt(szSection,_T("RequestDIBs"),m_bRequestDIBs);
	m_iJPEGQuality = wkp.GetInt(szSection,_T("JPEGQuality"),m_iJPEGQuality);
	m_iJPEGSize = wkp.GetInt(szSection,_T("JPEGSizeBytes"),m_iJPEGSize);
	m_Resolution = (Resolution)wkp.GetInt(szSection,_T("Resolution"),m_Resolution);

	m_bAudioIn = wkp.GetInt(szSection,_T("AudioIn"),m_bAudioIn);
	m_bAudioOut = wkp.GetInt(szSection,_T("AudioOut"),m_bAudioOut);
	m_idAudioCam = (DWORD)wkp.GetInt(szSection,_T("AudioCam"),m_idAudioCam.GetID());
	m_dwAudioRecordingBits = (DWORD)wkp.GetInt(szSection,_T("AudioRec"),m_dwAudioRecordingBits);

	m_bMPEG4 = wkp.GetInt(szSection,_T("MPEG4"),m_bMPEG4);
	m_bUseAVCodec = wkp.GetInt(szSection,_T("UseAVCodec"),m_bUseAVCodec);
	m_bIgnoreExternalDetectors = wkp.GetInt(szSection,_T("IgnoreExternalDetectors"),m_bIgnoreExternalDetectors);
}
/////////////////////////////////////////////////////////////////////////////
void CSettings::WriteSettings(CWK_Profile& wkp)
{
	wkp.WriteInt(szSection,_T("NrOfCameras"),m_iNrOfCameras);
	wkp.WriteInt(szSection,_T("PreMD"),m_iPreMD);
	wkp.WriteInt(szSection,_T("PostMD"),m_iPostMD);
	wkp.WriteInt(szSection,_T("Suspect"),m_iSuspect);
	wkp.WriteInt(szSection,_T("AlarmSeconds"),m_iNrAlarmSeconds);
	wkp.WriteInt(szSection,_T("LocalHD"),m_bLocalHD);
	wkp.WriteInt(szSection,_T("Removable"),m_bRemovable);
	wkp.WriteInt(szSection,_T("Track"),m_eTrackBehaviour);
	wkp.WriteInt(szSection,_T("AlarmList"),m_bAlarmList);
	wkp.WriteInt(szSection,_T("RequestDIBs"),m_bRequestDIBs);

	wkp.WriteInt(szSection,_T("TraceClientConfirms"),m_bTraceClientConfirms);
	wkp.WriteInt(szSection,_T("TraceClientRequests"),m_bTraceClientRequests);
	wkp.WriteInt(szSection,_T("TraceUnitConfirms"),m_bTraceUnitConfirms);
	wkp.WriteInt(szSection,_T("TraceUnitRequests"),m_bTraceUnitRequests);
	wkp.WriteInt(szSection,_T("TraceScan"),m_bTraceScan);
	wkp.WriteInt(szSection,_T("TraceScan"),m_bTraceScan);
	wkp.WriteInt(szSection,_T("TraceSave"),m_bTraceSave);
	wkp.WriteInt(szSection,_T("TraceH263"),m_bTraceH263);
	wkp.WriteInt(szSection,_T("TraceVOUT"),m_bTraceVOUT);
	wkp.WriteInt(szSection,_T("StatSwitch"),m_bStatSwitch);
	wkp.WriteInt(szSection,_T("JPEGQuality"),m_iJPEGQuality);
	wkp.WriteInt(szSection,_T("JPEGSizeBytes"),m_iJPEGSize);
	wkp.WriteInt(szSection,_T("Resolution"),m_Resolution);

	wkp.WriteInt(szSection,_T("AudioIn"),m_bAudioIn);
	wkp.WriteInt(szSection,_T("AudioOut"),m_bAudioOut);
	wkp.WriteInt(szSection,_T("AudioCam"),m_idAudioCam.GetID());
	wkp.WriteInt(szSection,_T("AudioRec"),m_dwAudioRecordingBits);
	
	wkp.WriteInt(szSection,_T("MPEG4"),m_bMPEG4);
	wkp.WriteInt(szSection,_T("UseAVCodec"),m_bUseAVCodec);

	wkp.WriteInt(szSection,_T("IgnoreExternalDetectors"),m_bIgnoreExternalDetectors);
}
/////////////////////////////////////////////////////////////////////////////
void CSettings::SetMPEG4(BOOL bMpeg4)
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	m_bMPEG4 = bMpeg4;
	wkp.WriteInt(szSection,_T("MPEG4"),m_bMPEG4);
}
