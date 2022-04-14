// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "Input.h"

#include "Camera.h"
#include "CameraGroup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CInput::CInput(CInputGroup* pGroup, CSecID id, InputMode mode)
{
	m_pGroup = pGroup;
	m_pCamera = NULL;
	m_pMedia = NULL;
	m_UnitID = id;
	m_bActive = FALSE;
	
	m_iOutstandingPicts = 0;
	m_dwUVVAlarmStart = 0;
	m_dwUVVKeyFlushed = 0;
	
	m_Mode = mode;
	m_bOn = TRUE;
	m_bInformClients = FALSE;
	m_dwX = 0;
	m_dwY = 0;
	m_sKey.Format(_T("%d"),id.GetSubID());

    BYTE bMax = (BYTE)(pGroup->GetSize()*2);
	WORD wNr = (WORD)((id.GetGroupID() & (pGroup->GetSize()-1)) * bMax + id.GetSubID());
	m_sName.Format(_T("%02d"),wNr+1);
	if (IsSuspect())
	{
		wNr += 0x40;
		wNr -= 0x08;
	}
	m_ClientID = CSecID(SECID_GROUP_INPUT,wNr);
	m_byAlarmCallHandling = 0;

	if (IsMD())
	{
		m_byAlarmCallHandling |= ALARMCALL_ACTIVITY;
	}
	if (IsExtern())
	{
		m_byAlarmCallHandling |= ALARMCALL_EXTERN;
	}
	if (IsSuspect())
	{
		m_byAlarmCallHandling |= ALARMCALL_SUSPECT;
	}
}
//////////////////////////////////////////////////////////////////////
CInput::~CInput()
{
	m_Fields.SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CInput::SetClientNr(BYTE bNr)
{
	if (!IsPTZ())
	{
		WORD wNr = MAKEWORD(bNr,(BYTE)m_pGroup->GetID());
		if (IsSuspect())
		{
			wNr += 0x80;
		}
		m_ClientID = CSecID(SECID_GROUP_INPUT,wNr);
		m_sName.Format(_T("%02d"),m_ClientID.GetSubID()+1);
	}
}
//////////////////////////////////////////////////////////////////////
CString CInput::GetName() const
{
	return m_pCamera?m_pCamera->GetName():m_sName;
}
//////////////////////////////////////////////////////////////////////
CString	CInput::NameOfEnum(InputMode mode)
{
	switch (mode)
	{
	case IM_INVALID: return _T("INVALID");
	case IM_EXTERN: return _T("EXTERN");
	case IM_MD: return _T("MD");
	case IM_SUSPECT: return _T("SUSPECT");
	case IM_PTZ: return _T("PTZ");
	case IM_AUDIO: return _T("AUDIO");
	default: return _T("INVALID");
	}
}
//////////////////////////////////////////////////////////////////////
void CInput::FromString(const CString& s)
{
	m_sName = CWK_Profile::GetStringFromString(s,_T("CO"),m_sName);
	m_bOn = CWK_Profile::GetNrFromString(s,_T("ST"),m_bOn);
	int iOutstandingPicts;
	
	iOutstandingPicts = CWK_Profile::GetNrFromString(s,_T("OP"),m_iOutstandingPicts);
	if (m_iOutstandingPicts != iOutstandingPicts)
	{
		m_iOutstandingPicts = iOutstandingPicts;
	}
	if (m_iOutstandingPicts>0)
	{
		m_bInformClients = TRUE;
	}
	DWORD dwUVVAlreadyDone;
	dwUVVAlreadyDone = CWK_Profile::GetNrFromString(s,_T("UVV"),m_dwUVVAlarmStart);
	if (   m_dwUVVAlarmStart == 0
		&& dwUVVAlreadyDone > 0)
	{
		m_dwUVVAlarmStart = GetTickCount()- dwUVVAlreadyDone;
		m_bInformClients = TRUE;
	}
}
//////////////////////////////////////////////////////////////////////
CString CInput::ToString(BOOL bShutdown)
{
	CString s;
	DWORD dwUVVAlreadyDone = 0;
	int iOutstandingPicts = 0;

	if (!bShutdown)
	{
		if (m_dwUVVAlarmStart !=0)
		{
			dwUVVAlreadyDone = GetTimeSpan(m_dwUVVAlarmStart);
		}
		if (m_iOutstandingPicts>0)
		{
			iOutstandingPicts = m_iOutstandingPicts; 
		}
	}

	s.Format(_T("\\CO%s\\ST%d\\OP%d\\UVV%d"),
			 m_sName,
			 m_bOn,
			 iOutstandingPicts,
			 dwUVVAlreadyDone);

	return s;
}
//////////////////////////////////////////////////////////////////////
CString CInput::Format()
{
	CString s;
	int i = m_iOutstandingPicts;

	if (m_Mode == IM_AUDIO)
	{
		i = m_bActive;
	}
	else
	{

		if (   GetCamera()
			&& GetCamera()->IsUVV())
		{
			if (   GetCamera()->GetGroup()->IsTasha()
				|| GetCamera()->GetGroup()->IsQ())
			{
				if (m_dwUVVAlarmStart != 0)
				{
					i = theApp.GetSettings().GetNrAlarmSeconds()-GetTimeSpan(m_dwUVVAlarmStart)/1000;
				}
			}
		}
	}



	s.Format(_T("UID%08lx CID%08lx %s %s %d "),
		GetUnitID().GetID(),
		GetClientID().GetID(),
		NameOfEnum(m_Mode), 
		GetName(),
		i
		);
	return s;
}
//////////////////////////////////////////////////////////////////////
void CInput::SetActive(BOOL bActive, const CIPCFields& fields)
{
	if (bActive && IsMD())
	{
		m_Fields.AddNoDuplicates(fields);
		m_bInformClients = FALSE;
	}

	SetActive(bActive);

	DWORD dwData1 = 0;
	CIPCField* pMDX = fields.GetCIPCField(_T("DVD_MD_X"));
	CIPCField* pMDY = fields.GetCIPCField(_T("DVD_MD_Y"));

	if (pMDX && pMDY)
	{
		DWORD x,y;
		x = y = 0;
		_stscanf(pMDX->GetValue(),_T("%04hx"),&x);
		_stscanf(pMDY->GetValue(),_T("%04hx"),&y);
		dwData1 = MAKELONG((WORD)x, (WORD)y);
		m_dwX = x;
		m_dwY = y;
	}
	else
	{
		m_dwX = 0;
		m_dwY = 0;
	}

	if (   IsActive()
		&& (GetOutstandingPicts()>0) 
		&& IsMD() 
		&& (dwData1 != 0)
		&& !GetCamera()->IsUVV()) //TKR: UVV cameras don`t need to know when a MD is active
	{
		// inform all clients
		theApp.GetInputClients().DoIndicateAlarmNr(GetClientID(),
			bActive,
			dwData1,
			0);
		m_bInformClients = TRUE;
	}
}
//////////////////////////////////////////////////////////////////////
void CInput::SetActive(BOOL bActive, DWORD dwX, DWORD dwY)
{
	if (   bActive 
		&& IsMD())
	{
		CIPCFields fields;
		
		if (dwX != 0)
		{
			CString sx,sy;
			sx.Format(_T("%04x"),LOWORD(dwX));
			sy.Format(_T("%04x"),HIWORD(dwY));

			fields.Add(new CIPCField(_T("DVD_MD_X"),sx,'C'));
			fields.Add(new CIPCField(_T("DVD_MD_Y"),sy,'C'));
		}
		m_Fields.AddNoDuplicates(fields);
		m_bInformClients = FALSE;
		m_dwX = dwX;
		m_dwY = dwY;
	}
	else
	{
		m_dwX = 0;
		m_dwY = 0;
	}

	SetActive(bActive);

	DWORD dwData1 = 0;

	if (dwX!=0)
	{
		dwData1 = MAKELONG((WORD)dwX,(WORD)dwY);
	}

	if (   IsActive()
		&& (GetOutstandingPicts()>0) 
		&& IsMD() 
		&& (dwData1 != 0)
		&& !GetCamera()->IsUVV()) //TKR: UVV cameras don`t need to know when a MD is active
	{
		// inform all clients
		theApp.GetInputClients().DoIndicateAlarmNr(GetClientID(),
												bActive,
												dwData1,
												0);
		m_bInformClients = TRUE;
	}
}
//////////////////////////////////////////////////////////////////////
void CInput::SetName(const CString& sName)
{
	m_sName = sName;
}
//////////////////////////////////////////////////////////////////////
void CInput::ClearActive()
{
	ResetOutstandingPicts();
}
//////////////////////////////////////////////////////////////////////
void CInput::CheckActive()
{
	BOOL bActive = GetGroup()->IsInputActive(GetUnitID());
	if (m_pCamera)
	{
		BOOL bTimer = m_pCamera->IsTimerActive();
		if (!bTimer && theApp.GetDetectorIgnoreTimer())
		{
			bTimer = TRUE;
		}
		if (bTimer)
		{
			SetActive(bActive);
		}
		else
		{
			ResetOutstandingPicts();
		}
	}
	else
	{
		SetActive(bActive);
	}
}
//////////////////////////////////////////////////////////////////////
void CInput::SetOutstandingPicts()
{
	if (m_pCamera)
	{
		if (m_pCamera->GetGroup()->IsTasha())
		{
			SetOutstandingPicts(theApp.GetSettings().GetNrPostMD()*5);
		}
		else if (m_pCamera->GetGroup()->IsQ())
		{
			SetOutstandingPicts(theApp.GetSettings().GetNrPostMD()*m_pCamera->GetFPS());
		}
		else
		{
			SetOutstandingPicts(theApp.GetSettings().GetNrPostMD());
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CInput::SetActive(BOOL bActive)
{
	if (m_pCamera)
	{
		if (IsAudio())
		{
			TRACE(_T("audio detector %d\n"),bActive);
		}
		BOOL bTimer = m_pCamera->IsTimerActive();

		if (   IsExtern() 
			|| IsSuspect()
			|| IsAudio())
		{
			if (!bTimer)
			{
				// timer is off
				if (theApp.GetDetectorIgnoreTimer())
				{
					bTimer = TRUE;
				}
			}
		}

		if (   !theApp.IsShutDown() 
			&& bTimer 
			&& bActive
			&& (   !IsAudio() 
			    || theApp.GetSettings().GetAudioRecordingLevel())
		   )
		{
			if (m_pCamera->IsMD())
			{
				if (m_pCamera->GetDwellTime()>0)
				{
					theApp.SetVideoOut(m_pCamera->GetCard(),m_pCamera->GetUnitID());
				}

				// flush MD PreAlarm to Storage
				if (m_iOutstandingPicts == 0)
				{
					if (m_pCamera->IsRecording())
					{
						if (IsMD())
						{
							m_pCamera->SetMDFlag();
						}
					}
/*
					else
					{
						m_pCamera->FlushMDPictures(this);
					}*/
					if (theApp.GetSettings().GetAlarmList())
					{
						SaveAlarm();
					}
				}
				SetOutstandingPicts();
				if (!IsAudio())
				{
					theApp.GetInputClients().DoIndicateAlarmNr(GetClientID(),TRUE);
					m_bInformClients = TRUE;
				}

				if (m_pCamera->GetMedia())
				{
					if (   IsAudio()
						&& theApp.GetSettings().GetAudioRecordingLevel())
					{
						m_pMedia->GetGroup()->SetRecording(AUDIO_RECORDING_LEVEL,bActive);
					}
					if (   IsExtern()
					    && theApp.GetSettings().GetAudioRecordingExtern())
					{
						m_pCamera->GetMedia()->GetGroup()->SetRecording(AUDIO_RECORDING_EXTERN,bActive);
					}
					if (   IsMD()
					    && theApp.GetSettings().GetAudioRecordingActivity())
					{
						m_pCamera->GetMedia()->GetGroup()->SetRecording(AUDIO_RECORDING_ACTIVITY,bActive);
					}
				}
			}
			else if (m_pCamera->IsUVV()) 
			{
				// 15 Min x 60 Sekunden x 2 Bilder
				if (IsExtern())
				{
					// active or not new tasha fps
					CCameraGroup* pGroup = (CCameraGroup*)m_pCamera->GetGroup();
					if (pGroup)
					{
						if (pGroup->IsTasha())
						{
							// renew the request with new parameters
							WK_TRACE(_T("UVV Alarm Tasha %08lx\n"),GetUnitID().GetID());
							SetUVVAlarmStart();
							pGroup->StartRequest(m_pCamera,bActive);
						}
						else if (pGroup->IsQ())
						{
							// renew the request with new parameters
							WK_TRACE(_T("UVV Alarm Q %08lx\n"),GetUnitID().GetID());
							SetUVVAlarmStart();
							pGroup->StartRequest(m_pCamera,bActive);
						}
						else
						{
							WK_TRACE(_T("UVV Alarm %08lx\n"),GetUnitID().GetID());
							SetOutstandingPicts(theApp.GetSettings().GetNrAlarmSeconds()*2);
						}
					}
					theApp.GetInputClients().DoIndicateAlarmNr(GetClientID(),TRUE);
					m_bInformClients = TRUE;
				}
				else if (IsSuspect())
				{
					int iNumPics = theApp.GetSettings().GetNrSuspect();
					if (   m_pCamera->GetGroup()->IsTasha()
						|| m_pCamera->GetGroup()->IsQ())
					{
						iNumPics *= m_pCamera->GetFPS();
					}
					SetOutstandingPicts(iNumPics);
					WK_TRACE(_T("UVV Suspect %08lx %d Frames\n"),GetUnitID().GetID(),iNumPics);
					theApp.GetInputClients().DoIndicateAlarmNr(GetClientID(),TRUE);
					m_bInformClients = TRUE;
				}
			}

			// raise the alarm call
			if ((m_byAlarmCallHandling & m_pCamera->GetAlarmCallHandling()) !=0)
			{
				theApp.GetCallThread().AddAlarmOutputID(m_pCamera->GetClientID().GetID());

				if (theApp.GetAlarmOutput())
				{
					theApp.ActivateAlarmOutput();
				}
			}
		}

		if (   !bActive
			&& m_pCamera->GetMedia()
			&& m_pCamera->IsMD()
			)
		{
			if (   IsAudio()
				&& theApp.GetSettings().GetAudioRecordingLevel())
			{
				m_pCamera->GetMedia()->GetGroup()->SetRecording(AUDIO_RECORDING_LEVEL,bActive);
			}
		}
	}
			
	// no camera ???
	
	m_bActive = bActive;
}
//////////////////////////////////////////////////////////////////////
int CInput::IsTrackPict() const
{
	if (theApp.GetSettings().TrackFirstPostAlarm())
	{
		if (m_dwUVVAlarmStart==0)
		{
			return m_iOutstandingPicts == theApp.GetSettings().GetNrPostMD();
		}
		else
		{
			return GetTimeSpan(m_dwUVVAlarmStart)<250;
		}

	}
	else if (theApp.GetSettings().TrackLastPostAlarm())
	{
		if (m_dwUVVAlarmStart==0)
		{
			return m_iOutstandingPicts == 1;
		}
		else
		{
			return GetTimeSpan(m_dwUVVAlarmStart)>1000*899;
		}
	}
	else
	{
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////
void CInput::ResetOutstandingPicts()
{
	SetOutstandingPicts(0);
	m_bActive = FALSE;
	m_Fields.SafeDeleteAll();
	InformClientsNotActive();
	if (   IsAudio()
		&& m_pCamera
		&& m_pCamera->GetMedia()
		&& theApp.GetSettings().GetAudioRecordingLevel())
	{
		m_pCamera->GetMedia()->GetGroup()->SetRecording(AUDIO_RECORDING_LEVEL,FALSE);
	}
}
/////////////////////////////////////////////////////////////////////
int CInput::GetOutstandingPicts() const
{
	if (m_dwUVVAlarmStart==0)
	{
		return m_iOutstandingPicts;
	}
	else
	{
		if (GetTimeSpan(m_dwUVVAlarmStart)<=(DWORD)(theApp.GetSettings().GetNrAlarmSeconds()*1000))
		{
			int iFps = 2;
			if (   GetCamera()->GetGroup()->IsTasha()
				|| GetCamera()->GetGroup()->IsQ())
			{
				iFps = GetCamera()->GetFPS();
			}
			return ((theApp.GetSettings().GetNrAlarmSeconds()) - GetTimeSpan(m_dwUVVAlarmStart)/1000)*iFps;
		}
		else
		{
			return 0;
		}
	}
}
//////////////////////////////////////////////////////////////////////
int CInput::GetAndDecreaseOutstandingPicts()
{
	BOOL bAlarmRunning = FALSE;

	if (   m_dwUVVAlarmStart!=0
		&& IsActive())
	{
		// UVV still active prolong
		m_dwUVVAlarmStart = GetTickCount();
	}

	if (m_iOutstandingPicts>0 || m_dwUVVAlarmStart !=0)
	{
		if (!IsActive())
		{
			int iOutstandingPicts = m_iOutstandingPicts;
			if (m_iOutstandingPicts>0)
			{
				SetOutstandingPicts(m_iOutstandingPicts-1);
			}

			if (   IsExtern() 
				&& m_pCamera
				&& m_pCamera->IsUVV())
			{
				if (   m_dwUVVKeyFlushed == 0
					|| GetTimeSpan(m_dwUVVKeyFlushed)>30*1000)
				{
					TRACE(_T("flush UVV key %s %08lx\n"),m_pCamera->GetName(),this);
					CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
					wkp.WriteString(m_pGroup->GetSection(),
									GetKey(),
									ToString(FALSE));
					wkp.FlushKey(m_pGroup->GetSection());
					m_dwUVVKeyFlushed = GetTickCount();
				}
			}

			if (iOutstandingPicts>0)
			{
				bAlarmRunning = m_iOutstandingPicts >0;
			}
			else if (m_dwUVVAlarmStart!=0)
			{
				bAlarmRunning = GetTimeSpan(m_dwUVVAlarmStart)<=(DWORD)(1000*theApp.GetSettings().GetNrAlarmSeconds());
			}

			if (!bAlarmRunning)
			{
				if (IsMD())
				{
					m_Fields.SafeDeleteAll();
				}
				InformClientsNotActive();

				if (   m_pCamera
					&& m_pCamera->GetMedia())
				{
					if (IsMD())
					{
						m_pCamera->GetMedia()->GetGroup()->SetRecording(AUDIO_RECORDING_ACTIVITY,FALSE);
					}
					if (IsExtern())
					{
						m_pCamera->GetMedia()->GetGroup()->SetRecording(AUDIO_RECORDING_EXTERN,FALSE);
					}
				}

				if (    m_pCamera
					&&  m_pCamera->IsUVV()
					&& IsExtern())
				{
					// deactive new tasha fps because alarm is over, all pictures are saved
					CCameraGroup* pGroup = (CCameraGroup*)m_pCamera->GetGroup();
					if (   pGroup 
						&& (   pGroup->IsTasha() 
						    || pGroup->IsQ())
							)
					{
						pGroup->StartRequest(m_pCamera,FALSE);
					}
				}
				m_dwUVVAlarmStart = 0;
			}
		}
		else //!IsActive()
		{
			if(m_iOutstandingPicts>0)
			{
				bAlarmRunning = TRUE;
			}
			else if(m_dwUVVAlarmStart !=0)
			{
				bAlarmRunning = FALSE;
			}
		}
	}
	return bAlarmRunning;
}
///////////////////////////////////////////////////////////////////////
void CInput::InformClientsNotActive()
{
	if (m_bInformClients)
	{
		m_bInformClients = FALSE;
		// inform all clients
		if (IsMD())
		{
			if (   m_pCamera
				&& m_pCamera->IsRecording()
				)
			{
				theApp.GetInputClients().DoIndicateAlarmNr(GetClientID(),TRUE);
			}
			else
			{
				theApp.GetInputClients().DoIndicateAlarmNr(GetClientID(),FALSE);
			}
		}
		else
		{
			if (   m_pCamera
				&& m_pCamera->IsMD()
				&& m_pCamera->GetMotionDetector()
				&& m_pCamera->GetMotionDetector()->GetOutstandingPicts()>0
				)
			{
				// wenn die MD noch aufnimmt alarm nicht löschen
			}
			else
			{
				theApp.GetInputClients().DoIndicateAlarmNr(GetClientID(),FALSE);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////
void CInput::SaveAlarm()
{
	CSystemTime st;
	st.GetLocalTime();

	CIPCFields fields;
	fields.Add(new CIPCField(_T("DVR_TIME"),st.GetDBTime(),_T('C')));
	fields.Add(new CIPCField(_T("DVR_DATE"),st.GetDBDate(),_T('C')));

	CString sTyp;

	if (IsExtern())
	{
		sTyp = _T("1");
	}
	else if (IsSuspect())
	{
		sTyp = _T("2");
	}
	else if (IsMD())
	{
		sTyp = _T("A");
	}
	else if (IsAudio())
	{
		sTyp = _T("V");
	}

	fields.Add(new CIPCField(_T("DVR_TYP"),sTyp,_T('C')));

	if (m_pCamera)
	{
		if (m_pCamera->GetAlarmListNr() != (WORD)-1)
		{
			theApp.SaveAlarmData(fields,
								m_pCamera->GetClientID(),
								m_pCamera->GetAlarmListNr());
		}
	}
}
/////////////////////////////////////////////////////////////////////////
void CInput::SetOutstandingPicts(int iOutstandingPicts)
{
	if (   m_pCamera
		&& m_pCamera->IsActive())
	{
		if (m_iOutstandingPicts != iOutstandingPicts)
		{
			m_iOutstandingPicts = iOutstandingPicts;
		}
	}
}
/////////////////////////////////////////////////////////////////////////
void CInput::SetUVVAlarmStart()
{
	if (   m_pCamera
		&& m_pCamera->IsActive())
	{
		m_dwUVVAlarmStart = GetTickCount();
	}
}
