// Settings.h: interface for the CSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGS_H__9CC6F908_3BFB_48C1_A052_3C9443115940__INCLUDED_)
#define AFX_SETTINGS_H__9CC6F908_3BFB_48C1_A052_3C9443115940__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "media.h"

typedef enum
{
	FIRST_PRE_ALARM  =	1,
	LAST_PRE_ALARM   =	2,
	FIRST_POST_ALARM =	3,
	LAST_POST_ALARM  =	4,
} TrackBehaviour;

class CSettings  
{
public:
	CSettings();
	virtual ~CSettings();

	// attributes
public:
	inline BOOL TraceClientRequests() const;
	inline BOOL TraceClientConfirms() const;
	inline BOOL TraceUnitRequests() const;
	inline BOOL TraceUnitConfirms() const;
	inline BOOL TraceSave() const;
	inline BOOL TraceScan() const;
	inline BOOL TraceSync() const;
	inline BOOL TraceH263() const;
	inline BOOL TraceVOUT() const;
	inline BOOL StatSwitch() const;

	inline int  GetNrOfCameras() const;
	inline int  GetNrPreMD() const;
	inline int  GetNrPostMD() const;
	inline int  GetNrSuspect() const;
	inline int  GetNrAlarmSeconds() const;
	inline BOOL GetAlarmList() const;
	inline BOOL GetRequestDIBs() const;
	inline BOOL IgnoreExternalDetectors() const;

	inline BOOL TrackFirstPreAlarm() const;
	inline BOOL TrackLastPreAlarm() const;
	inline BOOL TrackFirstPostAlarm() const;
	inline BOOL TrackLastPostAlarm() const;

	inline int  GetJPEGQuality() const;
	inline int  GetJPEGSize() const;
	inline BOOL GetMPEG4() const;
	inline BOOL UseAVCodec() const;

	inline Resolution GetResolution() const;

	inline BOOL GetAudioIn() const;
	inline BOOL GetAudioOut() const;
	inline CSecID GetAudioCam() const;

	inline BOOL GetAudioRecordingLevel() const;
	inline BOOL GetAudioRecordingActivity() const;
	inline BOOL GetAudioRecordingExtern() const;
	inline DWORD GetAudioRecordingBits() const;

	// set
public:
	inline void SetNrOfCameras(int iNrOfCameras);
	inline void SetResolution(Resolution res);
	
	inline void SetAudioIn(BOOL bAudioIn);
	inline void SetAudioOut(BOOL bAudioOut);
	inline void SetAudioCam(CSecID idAudioCam);
	inline void SetAudioRecordingBits(DWORD dwAudioRecordingBits);


	// operations
public:
	void ReadSettings(CWK_Profile& wkp);
	void WriteSettings(CWK_Profile& wkp);
	void SetMPEG4(BOOL bMpeg4);

private:
	// Traces
	BOOL		m_bTraceClientConfirms;
	BOOL		m_bTraceClientRequests;
	BOOL		m_bTraceUnitConfirms;
	BOOL		m_bTraceUnitRequests;
	BOOL		m_bTraceSave;
	BOOL		m_bTraceScan;
	BOOL		m_bTraceSync;
	BOOL		m_bTraceH263;
	BOOL		m_bTraceVOUT;
	BOOL		m_bStatSwitch;
	// Features
	TrackBehaviour	m_eTrackBehaviour;
	int			m_iNrOfCameras;
	int			m_iPreMD;		// Anzahl der Vor MD Bilder
	int			m_iPostMD;		// Anzahl der Nach MD Bilder
	int			m_iSuspect;
	int			m_iNrAlarmSeconds;
	BOOL		m_bLocalHD;		// Lokale Festplatten nutzen
	BOOL		m_bRemovable;	// Entfernbare Medien (JAZ/DVD) nutzen
	BOOL		m_bAlarmList;
	BOOL		m_bRequestDIBs;
	BOOL		m_bIgnoreExternalDetectors;
	
	int			m_iJPEGQuality;
	int			m_iJPEGSize;
	Resolution  m_Resolution;

	BOOL		m_bAudioIn;
	BOOL		m_bAudioOut;
	CSecID		m_idAudioCam;
	DWORD		m_dwAudioRecordingBits;
	
	BOOL	    m_bMPEG4;
	BOOL		m_bUseAVCodec;

	friend class CDVProcessApp;
};
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::TraceClientRequests() const
{
	return m_bTraceClientRequests;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::TraceClientConfirms() const
{
	return m_bTraceClientConfirms;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::TraceUnitRequests() const
{
	return m_bTraceUnitRequests;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::TraceUnitConfirms() const
{
	return m_bTraceUnitConfirms;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::TraceSave() const
{
	return m_bTraceSave;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::TraceScan() const
{
	return m_bTraceScan;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::TraceSync() const
{
	return m_bTraceSync;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::TraceH263() const
{
	return m_bTraceH263;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::TraceVOUT() const
{
	return m_bTraceVOUT;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::StatSwitch() const
{
	return m_bStatSwitch;
}
/////////////////////////////////////////////////////////////////////////////
inline int CSettings::GetNrOfCameras() const
{
	return m_iNrOfCameras;
}
/////////////////////////////////////////////////////////////////////////////
inline void CSettings::SetNrOfCameras(int iNrOfCameras)
{
	m_iNrOfCameras = iNrOfCameras;
}
/////////////////////////////////////////////////////////////////////////////
inline int CSettings::GetNrPreMD() const
{
	return m_iPreMD;
}
/////////////////////////////////////////////////////////////////////////////
inline int CSettings::GetNrPostMD() const
{
	return m_iPostMD;
}
/////////////////////////////////////////////////////////////////////////////
inline int CSettings::GetNrSuspect() const
{
	return m_iSuspect;
}
/////////////////////////////////////////////////////////////////////////////
inline int CSettings::GetNrAlarmSeconds() const
{
	return m_iNrAlarmSeconds;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::TrackFirstPreAlarm() const
{
	return m_eTrackBehaviour == FIRST_PRE_ALARM;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::TrackLastPreAlarm() const
{
	return m_eTrackBehaviour == LAST_PRE_ALARM;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::TrackFirstPostAlarm() const
{
	return m_eTrackBehaviour == FIRST_POST_ALARM;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::TrackLastPostAlarm() const
{
	return m_eTrackBehaviour == LAST_POST_ALARM;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::GetAlarmList() const
{
	return m_bAlarmList;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::GetRequestDIBs() const
{
	return m_bRequestDIBs;
}
/////////////////////////////////////////////////////////////////////////////
inline int CSettings::GetJPEGQuality() const
{
	return m_iJPEGQuality;
}
/////////////////////////////////////////////////////////////////////////////
inline int CSettings::GetJPEGSize() const
{
	return m_iJPEGSize;
}
/////////////////////////////////////////////////////////////////////////////
inline Resolution CSettings::GetResolution() const
{
	return m_Resolution;
}
/////////////////////////////////////////////////////////////////////////////
inline void CSettings::SetResolution(Resolution res)
{
	m_Resolution = res;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::GetAudioIn() const
{
	return m_bAudioIn;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::GetAudioOut() const
{
	return m_bAudioOut;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CSettings::GetAudioCam() const
{
	return m_idAudioCam;
}
/////////////////////////////////////////////////////////////////////////////
inline void CSettings::SetAudioIn(BOOL bAudioIn)
{
	m_bAudioIn = bAudioIn;
}
/////////////////////////////////////////////////////////////////////////////
inline void CSettings::SetAudioOut(BOOL bAudioOut)
{
	m_bAudioOut = bAudioOut;
}
/////////////////////////////////////////////////////////////////////////////
inline void CSettings::SetAudioCam(CSecID idAudioCam)
{
	m_idAudioCam = idAudioCam;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CSettings::GetAudioRecordingBits() const
{
	return m_dwAudioRecordingBits;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::GetAudioRecordingLevel() const
{
	return (m_dwAudioRecordingBits & AUDIO_RECORDING_LEVEL)>0;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::GetAudioRecordingActivity() const
{
	return (m_dwAudioRecordingBits & AUDIO_RECORDING_ACTIVITY)>0;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::GetAudioRecordingExtern() const
{
	return (m_dwAudioRecordingBits & AUDIO_RECORDING_EXTERN)>0;
}
/////////////////////////////////////////////////////////////////////////////
inline void CSettings::SetAudioRecordingBits(DWORD dwAudioRecordingBits)
{
	m_dwAudioRecordingBits = dwAudioRecordingBits;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::GetMPEG4() const
{
	return m_bMPEG4;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::IgnoreExternalDetectors() const
{
	return m_bIgnoreExternalDetectors;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSettings::UseAVCodec() const
{
	return m_bUseAVCodec;
}

#endif // !defined(AFX_SETTINGS_H__9CC6F908_3BFB_48C1_A052_3C9443115940__INCLUDED_)
