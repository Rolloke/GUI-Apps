// Camera.cpp: implementation of the CCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"

#include "Camera.h"
#include "Input.h"
#include "CameraGroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
CCameraH26xEncoder::CCameraH26xEncoder(CCamera* pCamera)
:CH26xEncoder(TRUE)
{
	m_pCamera = pCamera;
}
//////////////////////////////////////////////////////////////////////
CCameraH26xEncoder::~CCameraH26xEncoder()
{
	m_pCamera = NULL;
}
//////////////////////////////////////////////////////////////////////
void CCameraH26xEncoder::OnEncodedFrame()
{
	if (m_pCamera)
	{
		m_pCamera->OnEncodedFrame(COMPRESSION_H263);
	}
}
//////////////////////////////////////////////////////////////////////
CCameraMPEGEncoder::CCameraMPEGEncoder(CCamera* pCamera) : CMPEG4Encoder(TRUE)
{
	m_pCamera = pCamera;
}
//////////////////////////////////////////////////////////////////////
CCameraMPEGEncoder::~CCameraMPEGEncoder()
{
	m_pCamera = NULL;
}
//////////////////////////////////////////////////////////////////////
void CCameraMPEGEncoder::OnEncodedFrame()
{
	if (m_pCamera)
	{
		m_pCamera->OnEncodedFrame(COMPRESSION_MPEG4);
	}
}
//////////////////////////////////////////////////////////////////////
CPreAlarmPictures::CPreAlarmPictures(int iSize)
{
	m_iNrOfIFrames = 0;
	m_iPreAlarmSize = iSize;
}
CPreAlarmPictures::~CPreAlarmPictures()
{
	SafeDeleteAll();
	m_iNrOfIFrames = 0;
}
/////////////////////////////////////////////////////////////////////
void CPreAlarmPictures::SetPreAlarmSize(int iSize)
{
	m_iPreAlarmSize = iSize;
}
/////////////////////////////////////////////////////////////////////
void CPreAlarmPictures::AddNewPicture(const CIPCPictureRecord& pict)
{
	Lock();
	
	if (   pict.GetPictureType() == SPT_FULL_PICTURE
		|| pict.GetPictureType() == SPT_GOP_PICTURE
		|| GetSize() > 0)
	{
		CIPCPictureRecord* pPict = new CIPCPictureRecord(pict);
		if (pPict)
		{
			SafeAdd(pPict);
		}
		if (   pict.GetPictureType() == SPT_FULL_PICTURE
			|| pict.GetPictureType() == SPT_GOP_PICTURE)
		{
			m_iNrOfIFrames++;
			if (m_iNrOfIFrames>m_iPreAlarmSize)
			{
				// the first one should be I-Frame
				pPict = GetAtFast(0);
				if (pPict)
				{
					if (   pPict->GetPictureType() == SPT_FULL_PICTURE
						|| pPict->GetPictureType() == SPT_GOP_PICTURE)
					{
						m_iNrOfIFrames--;
					}
					else
					{
						TRACE("first image in pre alarm not I-Frame\n");
					}
					WK_DELETE(pPict);
				}
				RemoveAt(0);
				while (   GetSize()
					   && (pPict = GetAtFast(0))->GetPictureType() == SPT_DIFF_PICTURE)
				{
					WK_DELETE(pPict);
					RemoveAt(0);
				}
			}
		}
	}
	else
	{
		// P-Frame and Buffer empty
	}
	Unlock();
}
/////////////////////////////////////////////////////////////////////
void CPreAlarmPictures::FlushPictures(CCamera* pCamera, WORD wArchivNr)
{
	Lock();

	// kein Track Bild
	int iTrack = -1;

	if (theApp.GetSettings().TrackFirstPreAlarm())
	{
		// erstes Voralarm Bild
		iTrack = 0;
	}
	else if (theApp.GetSettings().TrackLastPreAlarm())
	{
		// letztes Voralarm Bild
		iTrack = GetSize()-1;
	}

	for (int i=0; i<GetSize();i++)
	{
		if (i == iTrack)
		{
			// Copy Constructor (!!)
			CString s;
			s.Format(_T("%d"),NAVIGATION_TRACK);
			pCamera->GetMotionDetector()->GetFields().Lock();
			CIPCFields fields(pCamera->GetMotionDetector()->GetFields());
			pCamera->GetMotionDetector()->GetFields().Unlock();
			fields.SafeAdd(new CIPCField(_T("DVD_MD_S"),s,'C'));
			pCamera->SavePicture(*GetAtFast(i),fields,wArchivNr);
		}
		else
		{
			pCamera->GetMotionDetector()->GetFields().Lock();
			CIPCFields fields(pCamera->GetMotionDetector()->GetFields());
			pCamera->GetMotionDetector()->GetFields().Unlock();
			pCamera->SavePicture(*GetAtFast(i),fields,wArchivNr);
		}
	}
	DeleteAll();
	m_iNrOfIFrames = 0;
	Unlock();
}
//////////////////////////////////////////////////////////////////////
LONG CClientCounters::GetMinCounter()
{
	LONG lMinCounter = 0;

	Lock();
	if (GetSize() == 1)
	{
		lMinCounter = GetAtFast(0)->GetCounter();
	}
	else if (GetSize()>1)
	{
		lMinCounter = GetAtFast(0)->GetCounter();
		for (int i=1;i<GetSize();i++)
		{
			if (GetAtFast(i)->GetCounter()<lMinCounter)
			{
				lMinCounter = GetAtFast(i)->GetCounter();
			}
		}
	}

	Unlock();

	return lMinCounter;
}
//////////////////////////////////////////////////////////////////////
void CClientCounters::Increment(DWORD dwClientID)
{
	Lock();
	for (int i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->GetClientID()<dwClientID)
		{
			GetAtFast(i)->Increment();
			break;
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CClientCounters::Reset()
{
	Lock();
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->Reset();
	}
	Unlock();}

//////////////////////////////////////////////////////////////////////
void CClientCounters::Decrement(DWORD dwClientID)
{
	Lock();
	for (int i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->GetClientID()<dwClientID)
		{
			GetAtFast(i)->Decrement();
			break;
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCamera::CCamera(COutputGroup* pGroup, CSecID id)
 : COutput(pGroup,id),m_PreAlarmPictures(theApp.GetSettings().GetNrPreMD())
{
	m_bActive = FALSE;
	m_Mode = OM_MD;
	m_bMDTrackFlag = FALSE;
	m_pExternDetector = NULL;
	m_pSuspectDetector = NULL;
	m_pMotionDetector = NULL;
	m_pAudioDetector = NULL;
	m_pMedia = NULL;

	m_wAlarmNr = (WORD)-1;
	m_wPreAlarmNr = (WORD)-1;
	m_wMDAlarmNr = (WORD)-1;
	m_wAlarmListNr = (WORD)-1;

	m_pLivePicture = NULL;
	
	m_iRecordedFramesPerDay = 0;
	m_iRecordedFramesPerWeek = 0;
	m_iCurrentRecordedFramesPerDay = 0;
	m_iCurrentRecordedFramesPerWeek = 0;
	m_iGrabbedFramesPerDay = 0;
	m_iGrabbedFramesPerWeek = 0;
	m_iCurrentGrabbedFramesPerDay = 0;
	m_iCurrentGrabbedFramesPerWeek = 0;

	m_bTerminateVideoSignal = TRUE;

	m_dwDwellTime = 5;
	m_byAlarmCallBits = 0;
	m_Compression = COMPRESSION_1;
	m_dwFPS = 2;
	if (GetGroup()->IsTasha())
	{
		m_dwFPS = 25;
	}
	else if (GetGroup()->IsQ())
	{
		m_dwFPS = 100/GetGroup()->GetSize();
		m_dwFPS = min(25,m_dwFPS);
	}


	// PTZ camera
	m_bPTZ = FALSE;
	m_eType	= CCT_UNKNOWN;
	m_dwPTZID = 0;
	m_iComPort	= 1;

	m_pJpeg = NULL;

	// JPEG to H.263
	m_pH26xEncoderQCIF = NULL;
	// m_dwH263ClientsQCIF
	InterlockedExchange(&m_iH263ClientRequestsQCIF,0);

	m_pMPEG4EncoderQCIF = NULL;
	// m_MPEG4ClientsQCIF;

	// Tasha low bandwidth stream
	// m_TashaClients
	m_cTashaLowbandwidthCompression = COMPRESSION_25; 
	// 128kBit = COMPRESSION_25
	//  64kBit = COMPRESSION_26
	m_rTashaLowbandwidthResolution = RESOLUTION_CIF; // CIF

	m_dwTickConfirm = 0;
	m_iTickDifference = 0;
	m_dwSwitchTimes.SetSize(100);
	m_iCurrentSwitchTimeIndex = 0;
	m_dwTashaLowBitrateConfirm = 0;

	m_stLastSave.GetLocalTime();

	m_iJPEGQuality = theApp.GetSettings().GetJPEGQuality();
	m_dwSum = 0;
	m_iCurrentLengthIndex = 0;
}
//////////////////////////////////////////////////////////////////////
CCamera::~CCamera()
{
	if (IsUVV())
	{
		WK_TRACE(_T("UVV %s %d ms Streuung %d\n"),GetName(),GetAverageSwitchtime(),GetSwitchtimeVarianz());
	}
	WK_TRACE(_T("camera %s JPEG Quality %d\n"),GetName(),GetJPEGQuality());
	WK_DELETE(m_pH26xEncoderQCIF);
	WK_DELETE(m_pMPEG4EncoderQCIF);
	WK_DELETE(m_pJpeg);
	m_MPEG4ClientsQCIF.SafeDeleteAll();
	m_TashaClients.SafeDeleteAll();
	DeletePictures();
}
//////////////////////////////////////////////////////////////////////
void CCamera::DeletePictures()
{
	Lock();
	WK_DELETE(m_pLivePicture);
	m_PreAlarmPictures.SafeDeleteAll();
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CCamera::SetMDFlag()
{
	m_bMDTrackFlag = TRUE;
}
//////////////////////////////////////////////////////////////////////
void CCamera::SetActive(BOOL bActive)
{
	COutput::SetActive(bActive);
	if (m_bActive)
	{
		if (IsUVV())
		{
			if (m_pMotionDetector && 
				m_pMotionDetector->GetOutstandingPicts() > 0)
			{
				theApp.GetInputClients().DoIndicateAlarmNr(m_pMotionDetector->GetClientID(),TRUE);
			}
			ResetUVVStatistics();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CCamera::SetTimerActive(BOOL bTimerActive)
{
	if (m_bTimerActive != bTimerActive)
	{
		COutput::SetTimerActive(bTimerActive);

		// inform Clients about change
		theApp.GetOutputClients().DoConfirmGetValue(GetClientID(),
			CSD_IS_TIMER_ACTIVE,
			m_bTimerActive ? CSD_ON : CSD_OFF);


		if (!m_bTimerActive)
		{
			// deaktiviere einen möglichen MD Alarm
			if (m_pMotionDetector)
			{
				m_pMotionDetector->ResetOutstandingPicts();
				theApp.GetInputClients().DoIndicateAlarmNr(m_pMotionDetector->GetClientID(),FALSE);
			}
			if (m_pExternDetector)
			{
				m_pExternDetector->ResetOutstandingPicts();
				theApp.GetInputClients().DoIndicateAlarmNr(m_pExternDetector->GetClientID(),FALSE);
			}
			if (m_pSuspectDetector)
			{
				m_pSuspectDetector->ResetOutstandingPicts();
				theApp.GetInputClients().DoIndicateAlarmNr(m_pSuspectDetector->GetClientID(),FALSE);
			}
			if (m_pAudioDetector)
			{
				m_pAudioDetector->ResetOutstandingPicts();
				theApp.GetInputClients().DoIndicateAlarmNr(m_pAudioDetector->GetClientID(),FALSE);
			}
		}
		else
		{
			// aktiviere die Melder neu
			if (m_pExternDetector)
			{
				m_pExternDetector->CheckActive();
			}
			if (m_pSuspectDetector)
			{
				m_pSuspectDetector->CheckActive();
			}
			if (m_pAudioDetector)
			{
				m_pAudioDetector->CheckActive();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CCamera::SetFPS(DWORD dwFPS)
{
	if (m_dwFPS != dwFPS)
	{
		m_dwFPS = dwFPS;
		if (   GetGroup()->IsTasha()
			|| GetGroup()->IsQ())
		{
			CCameraGroup* pCameraGroup = (CCameraGroup*)GetGroup();
			pCameraGroup->StartRequest(this,IsInputActive());
		}
		if (GetGroup()->IsQ())
		{
			m_PreAlarmPictures.SetPreAlarmSize(theApp.GetSettings().GetNrPreMD()*m_dwFPS);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CCamera::SetMode(OutputMode mode)
{
	if (m_Mode != mode)
	{
		COutput::SetMode(mode);
		if (IsUVV())
		{
			WK_TRACE(_T("camera %s changed new mode is UVV\n"),GetName());
			DeletePictures();
			ResetFramesPerDay();
			ResetFramesPerWeek();
			GetGroup()->DoRequestSetValue(GetUnitID(),CSD_MD,CSD_OFF);
			if (m_pMotionDetector)
			{
				m_pMotionDetector->ResetOutstandingPicts();
			}
			if (m_pAudioDetector)
			{
				m_pAudioDetector->ResetOutstandingPicts();
			}
			m_byAlarmCallBits &= ~ALARMCALL_ACTIVITY;
		}
		else if (IsMD())
		{
			WK_TRACE(_T("camera %s changed new mode is MD\n"),GetName());
			GetGroup()->DoRequestSetValue(GetUnitID(),CSD_MD,CSD_ON);
            if (m_pExternDetector)
			{
				m_pExternDetector->ResetOutstandingPicts();
			}
            if (m_pSuspectDetector)
			{
				m_pSuspectDetector->ResetOutstandingPicts();
			}
		}
		if (   GetGroup()->IsTasha()
			|| GetGroup()->IsQ())
		{
			CCameraGroup* pCameraGroup = (CCameraGroup*)GetGroup();
			pCameraGroup->StartRequest(this,IsInputActive());
		}

		ResetUVVStatistics();

		theApp.SetReset(FALSE,TRUE,FALSE);
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CCamera::IsRecording() const
{
	BOOL r = FALSE;

	if (m_pMotionDetector)
	{
		r |= (m_pMotionDetector->GetOutstandingPicts()>0);
	}
	if (m_pExternDetector)
	{
		r |= m_pExternDetector->IsActive();
		r |= (m_pExternDetector->GetOutstandingPicts()>0);
	}
	if (m_pSuspectDetector)
	{
		r |= m_pSuspectDetector->IsActive();
		r |= (m_pSuspectDetector->GetOutstandingPicts()>0);
	}
	if (m_pAudioDetector)
	{
		r |= m_pAudioDetector->IsActive();
		r |= (m_pAudioDetector->GetOutstandingPicts()>0);
	}

	return r;
}
//////////////////////////////////////////////////////////////////////
BOOL CCamera::IsInputActive() const
{
	if (IsMD())
	{
		if (m_pMotionDetector
			&& m_pMotionDetector->GetOutstandingPicts()>0)
		{
			return TRUE;
		}
		if (m_pExternDetector
			&& m_pExternDetector->GetOutstandingPicts()>0)
		{
			return TRUE;
		}
	}
	else if (IsUVV())
	{
		if (   m_pExternDetector
			&& m_pExternDetector->GetOutstandingPicts()>0)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CCamera::GetMDCoordinates(DWORD& dwX,DWORD& dwY)
{
	if (   m_pMotionDetector
		&& m_pMotionDetector->GetOutstandingPicts())
	{
		dwX = m_pMotionDetector->GetX();
		dwY = m_pMotionDetector->GetY();
	}
	else
	{
		dwX = 0;
		dwY = 0;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CCamera::IsLastSaveOld()
{
	CSystemTime st,diff,empty;

	st.GetLocalTime();
	if (st>m_stLastSave)
	{
		diff = st - m_stLastSave;

		if (diff.GetYear()>empty.GetYear())
		{
			return TRUE;
		}
		if (diff.GetMonth()>empty.GetMonth())
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CCamera::FromString(const CString& s)
{
	COutput::FromString(s);
	CString sTemp;
	DWORD i;
	sTemp = CWK_Profile::GetStringFromString(s,_T("FPD"),_T("0"));
	i = 0;
	if (1 == _stscanf(sTemp,_T("%d"),&i))
	{
		m_iRecordedFramesPerDay = i;
	}
	sTemp = CWK_Profile::GetStringFromString(s,_T("CFPD"),_T("0"));
	i = 0;
	if (1 == _stscanf(sTemp,_T("%d"),&i))
	{
		m_iCurrentRecordedFramesPerDay = i;
	}
	sTemp = CWK_Profile::GetStringFromString(s,_T("FPW"),_T("0"));
	i = 0;
	if (1 == _stscanf(sTemp,_T("%d"),&i))
	{
		m_iRecordedFramesPerWeek = i;
	}
	sTemp = CWK_Profile::GetStringFromString(s,_T("CFPW"),_T("0"));
	i = 0;
	if (1 == _stscanf(sTemp,_T("%d"),&i))
	{
		m_iCurrentRecordedFramesPerWeek = i;
	}

	sTemp = CWK_Profile::GetStringFromString(s,_T("GFPD"),_T("0"));
	i = 0;
	if (1 == _stscanf(sTemp,_T("%d"),&i))
	{
		m_iGrabbedFramesPerDay = i;
	}
	sTemp = CWK_Profile::GetStringFromString(s,_T("GFPW"),_T("0"));
	i = 0;
	if (1 == _stscanf(sTemp,_T("%d"),&i))
	{
		m_iGrabbedFramesPerWeek = i;
	}
	sTemp = CWK_Profile::GetStringFromString(s,_T("CGFPD"),_T("0"));
	i = 0;
	if (1 == _stscanf(sTemp,_T("%d"),&i))
	{
		m_iCurrentGrabbedFramesPerDay = i;
	}
	sTemp = CWK_Profile::GetStringFromString(s,_T("CGFPW"),_T("0"));
	i = 0;
	if (1 == _stscanf(sTemp,_T("%d"),&i))
	{
		m_iCurrentGrabbedFramesPerWeek = i;
	}

	sTemp = CWK_Profile::GetStringFromString(s,_T("LSP"),_T(""));
	if (!sTemp.IsEmpty())
	{
		m_stLastSave.FromString(sTemp);
	}

	m_dwDwellTime = CWK_Profile::GetNrFromString(s,_T("DWELL"),m_dwDwellTime);
	m_dwFPS = CWK_Profile::GetNrFromString(s,_T("FPS"),m_dwFPS);
	m_PreAlarmPictures.SetPreAlarmSize(theApp.GetSettings().GetNrPreMD()*m_dwFPS);
	m_byAlarmCallBits = (BYTE)CWK_Profile::GetNrFromString(s,_T("Alarm"),m_byAlarmCallBits);
	m_iJPEGQuality = CWK_Profile::GetNrFromString(s,_T("JPEGQuality"),m_iJPEGQuality);
	m_Compression = (Compression)CWK_Profile::GetNrFromString(s,_T("Compression"),m_Compression);
	m_bPTZ = (BOOL)CWK_Profile::GetNrFromString(s, INI_SCHWENK_NEIGER, m_bPTZ);
	m_bTerminateVideoSignal = (BOOL)CWK_Profile::GetNrFromString(s,INI_TERMINATION,m_bTerminateVideoSignal);

	if (m_bPTZ)
	{
		// PTZ load under CommUnit
		// searching for my camid
		CString sSection, sKey, sCamID, sTemp;

		CWK_Profile wkpDVRT;
		DWORD dwComMask = wkpDVRT.GetInt(_T("Common"),_T("CommPortInfo"),0);
		sCamID.Format(_T("%08x"), GetUnitID().GetID());
		int iNum = 0;

		int iCom=1;
		for (DWORD dwBit=1 ; dwBit ; dwBit<<=1, iCom++)
		{
			if (dwComMask & dwBit)
			{
				BOOL bFound = FALSE;
				sSection.Format(_T("CommUnit\\COM%d"), iCom);
				iNum = wkpDVRT.GetInt(sSection, _T("Num"), 0);
				for (int k=0 ; k<iNum ; k++)
				{
					sKey.Format(_T("SID%d"), k);
					sTemp = wkpDVRT.GetString(sSection, sKey, _T(""));
					if (sTemp == sCamID)
					{
						// bingo found
						m_eType = (CameraControlType)wkpDVRT.GetInt(sSection, _T("Typ"), 0);
						m_iComPort = iCom;
						sKey.Format(_T("CID%d"), k);
						m_dwPTZID = wkpDVRT.GetInt(sSection, sKey, 0);
						bFound = TRUE;
						break;
					}
				}
				if (bFound)
				{
					break;
				}
			}
		}
	} // PTZ
}
//////////////////////////////////////////////////////////////////////
CString CCamera::ToString()
{
	CString s1 = COutput::ToString();
	CString s2,s3,s4,s5;

	s2.Format(_T("\\FPD%d\\FPW%d\\CFPD%d\\CFPW%d\\LSP%s"),
		m_iRecordedFramesPerDay,
		m_iRecordedFramesPerWeek,
		m_iCurrentRecordedFramesPerDay,
		m_iCurrentRecordedFramesPerWeek,
		m_stLastSave.ToString());
	s3.Format(_T("\\GFPD%d\\GFPW%d\\CGFPD%d\\CGFPW%d"),
		m_iGrabbedFramesPerDay,
		m_iGrabbedFramesPerWeek,
		m_iCurrentGrabbedFramesPerDay,
		m_iCurrentGrabbedFramesPerWeek);
	s4.Format(_T("\\DWELL%d\\Alarm%d\\JPEGQuality%d\\Compression%d\\SN%d"),
		m_dwDwellTime,
		m_byAlarmCallBits,
		m_iJPEGQuality,
		(int)m_Compression,
		m_bPTZ);
	s5.Format(_T("\\TERM%d\\FPS%d"),m_bTerminateVideoSignal,m_dwFPS);

	// PTZ data at output group

	return s1 + s2 + s3 + s4 + s5;
}
//////////////////////////////////////////////////////////////////////
CString CCamera::Format()
{
	CString s = COutput::Format();

	s = _T("C ") + s;

	if (IsMD())
	{
		s += _T(" MD ");
	}
	else if (IsUVV())
	{
		s += _T(" UVV");
	}

	if (GetGroup()->HasCameras())
	{
		if (IsTimerActive())
		{
			s += _T(" Timer ON");
		}
		else
		{
			s += _T(" Timer OFF");
		}

		double fMovementDayPercent = 0.0;
		double fMovementWeekPercent = 0.0;
		DWORD iRFPD = m_iCurrentRecordedFramesPerDay + m_iRecordedFramesPerDay;
		DWORD iGFPD = m_iCurrentGrabbedFramesPerDay + m_iGrabbedFramesPerDay;
		if (iGFPD>0)
		{
			fMovementDayPercent = (((double)iRFPD*100.0)/(double)iGFPD);
		}
		DWORD iRFPW = m_iCurrentRecordedFramesPerWeek + m_iRecordedFramesPerWeek;
		DWORD iGFPW = m_iCurrentGrabbedFramesPerWeek + m_iGrabbedFramesPerWeek;
		if (iGFPW>0)
		{
			fMovementWeekPercent = (((double)iRFPW*100.0)/(double)iGFPW);
		}
		CString sPercent;
		sPercent.Format(_T(" %02.2f%% Day %02.2f%% Week"),fMovementDayPercent,fMovementWeekPercent);
		s += sPercent;

		CString sFPS;
		sFPS.Format(_T(" %d fps"),GetFPS());
		s+= sFPS;

	}


	return s;
}
//////////////////////////////////////////////////////////////////////
void CCamera::ResetFramesPerDay()
{
	m_iRecordedFramesPerDay = m_iCurrentRecordedFramesPerDay;
	m_iCurrentRecordedFramesPerDay = 0;

	m_iGrabbedFramesPerDay = m_iCurrentGrabbedFramesPerDay;
	m_iCurrentGrabbedFramesPerDay = 0;
}
//////////////////////////////////////////////////////////////////////
void CCamera::ResetFramesPerWeek()
{
	m_iRecordedFramesPerWeek = m_iCurrentRecordedFramesPerWeek;
	m_iCurrentRecordedFramesPerWeek = 0;

	m_iGrabbedFramesPerWeek = m_iCurrentGrabbedFramesPerWeek;
	m_iCurrentGrabbedFramesPerWeek = 0;
}
//////////////////////////////////////////////////////////////////////
void CCamera::SaveReferenceImage()
{
	if (IsMD())
	{
		if ((m_wMDAlarmNr!=(WORD)-1) && IsTimerActive())
		{
			Lock();
			if (m_pLivePicture)
			{
				CString s;
				s.Format(_T("%d"),NAVIGATION_FULL_HOUR);

				m_pMotionDetector->GetFields().Lock();
				CIPCFields fields(m_pMotionDetector->GetFields());
				m_pMotionDetector->GetFields().Unlock();

				fields.SafeAdd(new CIPCField(_T("DVD_MD_S"),s,'C'));
				SavePicture(*m_pLivePicture,fields,m_wMDAlarmNr);
			}
			Unlock();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CCamera::ResetUVVStatistics()
{
	m_dwTickConfirm = 0;
	m_iTickDifference = 0;
	for (int i=0;i<m_dwSwitchTimes.GetSize();i++)
	{
		m_dwSwitchTimes.SetAt(i,0);
	}
}
//////////////////////////////////////////////////////////////////////
int	CCamera::GetIntervall()
{
	int iIntervall = 0;
	if (IsUVV())
	{
		iIntervall = IsInputActive() ? 500 : 1000;
	}
	return iIntervall;
}
//////////////////////////////////////////////////////////////////////
void CCamera::NewTashaClientPicture(const CIPCPictureRecord& pict,DWORD x, DWORD y)
{
	m_TashaClients.Lock();
	for (int i=0;i<m_TashaClients.GetSize();i++)
	{
		COutputClient* pOC = theApp.GetOutputClients().GetOutputClient(m_TashaClients[i]->GetClientID());
		if (   pOC
			&& pOC->IsConnected())
		{
			pOC->GetCIPC()->SendPicture(this,pict,x,y,FALSE);
			m_TashaClients[i]->Decrement();
		}
	}
	m_dwTashaLowBitrateConfirm = GetTickCount();
	m_TashaClients.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CCamera::NewPicture(const CIPCPictureRecord& pict,DWORD x, DWORD y)
{
	DWORD dwTick = WK_GetTickCount();
	BOOL bNoCamPicture = FALSE;
	Lock();
	if (IsUVV())
	{
		if (m_dwTickConfirm!=0)
		{
			DWORD dwTickDifference = WK_GetTimeSpan(m_dwTickConfirm,dwTick);
			int iIntervall = GetIntervall();
			m_iTickDifference += ((int)dwTickDifference)-iIntervall;
			m_dwSwitchTimes.SetAt(m_iCurrentSwitchTimeIndex++,dwTickDifference);
			if (m_iCurrentSwitchTimeIndex>=m_dwSwitchTimes.GetSize())
			{
				m_iCurrentSwitchTimeIndex = 0;
			}
//			TRACE(_T("%s UVV Time diff %d ms, sum %d\n"),GetName(),dwTickDifference,m_iTickDifference);
		}
	}
	m_dwTickConfirm = dwTick;
	if (pict.GetJobTime() == 0)
	{
		// detect the state change
		if (m_pLivePicture && m_pLivePicture->GetJobTime() != 0)
		{
			bNoCamPicture = TRUE;
		}
	}
	else
	{
		m_iCurrentGrabbedFramesPerDay++;
		m_iCurrentGrabbedFramesPerWeek++;
	}

	if (  pict.GetPictureType() == SPT_FULL_PICTURE
		||pict.GetPictureType() == SPT_GOP_PICTURE)
	{
		WK_DELETE(m_pLivePicture);
		m_pLivePicture = new CIPCPictureRecord(pict);
		// indicate live picture to all clients
	}
	Unlock();
	theApp.ConfirmLivePicture(this,pict,x,y);


	if (IsMD())
	{
		// MD Detector
		BOOL bSaved = FALSE;
		if (m_pMotionDetector)
		{
			BOOL bTrack = m_pMotionDetector->IsTrackPict();
			if (m_pMotionDetector->GetAndDecreaseOutstandingPicts()>0)
			{
				FlushMDPictures();
				if (   (m_wMDAlarmNr!=(WORD)-1)
					&& IsTimerActive())
				{
					if (bTrack && m_bMDTrackFlag)
					{
						CString s;
						s.Format(_T("%d"),NAVIGATION_TRACK);
						m_pMotionDetector->GetFields().Lock();
						CIPCFields fields(m_pMotionDetector->GetFields());
						m_pMotionDetector->GetFields().Unlock();
						fields.SafeAdd(new CIPCField(_T("DVD_MD_S"),s,'C'));
						SavePicture(pict,fields,m_wMDAlarmNr);
						m_bMDTrackFlag = FALSE;
					}
					else
					{
						SavePicture(pict,m_pMotionDetector->GetFields(),m_wMDAlarmNr);
					}
					bSaved = TRUE;
				}
			}
		}
		if (   m_pExternDetector 
			&& m_pExternDetector->GetAndDecreaseOutstandingPicts()>0)
		{
			FlushMDPictures();
			if (((m_wMDAlarmNr!=(WORD)-1)) && !bSaved)
			{
				SavePicture(pict,m_pExternDetector->GetFields(),m_wMDAlarmNr);
				bSaved = TRUE;
			}
		}
		if (   m_pSuspectDetector 
			&& m_pSuspectDetector->GetAndDecreaseOutstandingPicts()>0)
		{
			FlushMDPictures();
			if (((m_wMDAlarmNr!=(WORD)-1)) && !bSaved)
			{
				SavePicture(pict,m_pSuspectDetector->GetFields(),m_wMDAlarmNr);
				bSaved = TRUE;
			}
		}
		if (   m_pAudioDetector 
			&& m_pAudioDetector->GetAndDecreaseOutstandingPicts()>0)
		{
			FlushMDPictures();
			if (((m_wMDAlarmNr!=(WORD)-1)) && !bSaved)
			{
				SavePicture(pict,m_pAudioDetector->GetFields(),m_wMDAlarmNr);
				bSaved = TRUE;
			}
		}
		if (!bSaved)
		{
			m_PreAlarmPictures.AddNewPicture(pict);
			m_bMDTrackFlag = TRUE;
		}
	}
	else if (IsUVV())
	{
		// Alarm Detector
		BOOL bSaved = FALSE;
		if (m_pExternDetector)
		{
			if (m_pExternDetector->GetAndDecreaseOutstandingPicts()>0)
			{
				if (m_wAlarmNr!=(WORD)-1)
				{
					SavePicture(pict,m_pExternDetector->GetFields(),m_wAlarmNr);
					bSaved = TRUE;
				}
			}
			else 
			{
				if (m_wPreAlarmNr!=(WORD)-1)
				{
					SavePicture(pict,m_pExternDetector->GetFields(),m_wPreAlarmNr);
				}
			}
		}

		if (!bSaved)
		{
			if (   m_pSuspectDetector 
				&& m_pSuspectDetector->GetAndDecreaseOutstandingPicts() > 0)
			{
				if (m_wAlarmNr!=(WORD)-1)
				{
					// save suspicious image
					CString s;
					s.Format(_T("%d"),NAVIGATION_SUSPECT);
					m_pMotionDetector->GetFields().Lock();
					CIPCFields fields(m_pMotionDetector->GetFields());
					m_pMotionDetector->GetFields().Unlock();
					fields.SafeAdd(new CIPCField(_T("DVD_MD_S"),s,'C'));
					SavePicture(pict,fields,m_wAlarmNr);
					bSaved = TRUE;
				}
			}
		}
	}

	if (   (   IsUVV()	
		    || GetGroup()->IsSavic())
		&& m_dwH263ClientsQCIF.GetSize())
	{
		EncodeH263(pict);
	}
	if (   (   IsUVV()	
		|| GetGroup()->IsSavic())
		&& m_MPEG4ClientsQCIF.GetSize())
	{
		EncodeMPEG4(pict);
	}

	if (bNoCamPicture)
	{
		Disable();
	}
}
//////////////////////////////////////////////////////////////////////////
void CCamera::EncodeH263(const CIPCPictureRecord& pict)
{
	const BYTE* pData = NULL;
	int iWidth,iHeight;
	int iH263Width,iH263Height;
	CompressionType ct = COMPRESSION_UNKNOWN;
	iWidth = iHeight = 0;
	iH263Width = iH263Height = 0;
	if (pict.GetCompressionType() == COMPRESSION_JPEG)
	{
		// JPEG Bilder von Jacob oder Savic
		if (m_pJpeg == NULL)
		{
			m_pJpeg = new CJpeg();
			m_pJpeg->SetProperties(768,576,IJL_YCBCR);
		}
		m_pJpeg->DecodeJpegFromMemory((BYTE*)pict.GetData(),
			pict.GetDataLength(),
			TRUE,
			CJ_NOTHING,TRUE);
		pData = m_pJpeg->GetDibBuffer();
		iWidth = m_pJpeg->GetImageDims().cx;
		iHeight = m_pJpeg->GetImageDims().cy;
		ct = COMPRESSION_YUV_422;
		if (iHeight*2<iWidth)
		{
			iH263Width = iWidth / 4;
			iH263Height = iHeight / 2;
		}
		else
		{
			iH263Width = iWidth / 2;
			iH263Height = iHeight / 2;
		}
	}
	else if (pict.GetCompressionType() == COMPRESSION_YUV_422)
	{
		// YUV Bilder von Savic
		BYTE* pPictData = (BYTE*)pict.GetData();
		LPBITMAPINFOHEADER pBmiHeader = (LPBITMAPINFOHEADER)pPictData;
		pData = pPictData+pBmiHeader->biSize;
		iWidth = pBmiHeader->biWidth;
		iHeight = pBmiHeader->biHeight;
		ct = COMPRESSION_YUV_422;
		if (iHeight*2<iWidth)
		{
			iH263Width = iWidth / 4;
			iH263Height = iHeight / 2;
		}
		else
		{
			iH263Width = iWidth / 2;
			iH263Height = iHeight / 2;
		}
	}
	else if (pict.GetCompressionType() == COMPRESSION_RGB_24)
	{
		// kleine MD RGB Bilder von Jacob
		BYTE* pPictData = (BYTE*)pict.GetData();
		LPBITMAPINFOHEADER pBmiHeader = (LPBITMAPINFOHEADER)pPictData;
		pData = pPictData+pBmiHeader->biSize;
		iWidth = pBmiHeader->biWidth;
		iHeight = pBmiHeader->biHeight;
		ct = COMPRESSION_RGB_24;
		iH263Width = iWidth;
		iH263Height = iHeight;
	}

	if (m_pH26xEncoderQCIF == NULL)
	{
		// Encoder anlegen, falls noch nicht vorhanden
		m_pH26xEncoderQCIF = new CCameraH26xEncoder(this);
		if (theApp.GetSettings().TraceH263())
		{
			WK_TRACE(_T("Init H.263 %d,%d\n"),iH263Width,iH263Height);
		}
		m_pH26xEncoderQCIF->Init(iH263Width,iH263Height,0,0,FALSE);
	}
	if (   m_pH26xEncoderQCIF
		&& pData)
	{
		if (m_iH263ClientRequestsQCIF >= m_dwH263ClientsQCIF.GetSize())
		{
			if (ct == COMPRESSION_YUV_422)
			{
				m_pH26xEncoderQCIF->SetYUV422Data(pict.GetCamID(),iWidth,iHeight,pData);
			}
			else if(ct == COMPRESSION_RGB_24)
			{
				m_pH26xEncoderQCIF->SetRGBData(pict.GetCamID(),iWidth,iHeight,pData);
			}
		}
		else
		{
			//	TRACE(_T("new QCIF Picture not enough client requests %08lx, %d\n"),GetUnitID().GetID(),m_iH263ClientRequestsQCIF);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CCamera::EncodeMPEG4(const CIPCPictureRecord& pict)
{
	const BYTE* pData = NULL;
	int iWidth,iHeight;
	int iMPEG4Width,iMPEG4Height;
	CompressionType ct = COMPRESSION_UNKNOWN;
	iWidth = iHeight = 0;
	iMPEG4Width = iMPEG4Height = 0;
	if (pict.GetCompressionType() == COMPRESSION_JPEG)
	{
		// JPEG Bilder von Jacob oder Savic
		if (m_pJpeg == NULL)
		{
			m_pJpeg = new CJpeg();
			m_pJpeg->SetProperties(768,288,IJL_YCBCR);
		}
		m_pJpeg->DecodeJpegFromMemory((BYTE*)pict.GetData(),pict.GetDataLength(),FALSE,CJ_NOTHING,TRUE);
		pData = m_pJpeg->GetDibBuffer();
		iWidth = m_pJpeg->GetImageDims().cx;
		iHeight = m_pJpeg->GetImageDims().cy;
		ct = COMPRESSION_YUV_422;
		if (iHeight*2<iWidth)
		{
			iMPEG4Width = iWidth / 4;
			iMPEG4Height = iHeight / 2;
		}
		else
		{
			iMPEG4Width = iWidth / 2;
			iMPEG4Height = iHeight / 2;
		}
	}
	else if (pict.GetCompressionType() == COMPRESSION_YUV_422)
	{
		// YUV Bilder von Savic
		BYTE* pPictData = (BYTE*)pict.GetData();
		LPBITMAPINFOHEADER pBmiHeader = (LPBITMAPINFOHEADER)pPictData;
		pData = pPictData+pBmiHeader->biSize;
		iWidth = pBmiHeader->biWidth;
		iHeight = pBmiHeader->biHeight;
		ct = COMPRESSION_YUV_422;
		if (iHeight*2<iWidth)
		{
			iMPEG4Width = iWidth / 4;
			iMPEG4Height = iHeight / 2;
		}
		else
		{
			iMPEG4Width = iWidth / 2;
			iMPEG4Height = iHeight / 2;
		}
	}
	else if (pict.GetCompressionType() == COMPRESSION_RGB_24)
	{
		// kleine MD RGB Bilder von Jacob
		BYTE* pPictData = (BYTE*)pict.GetData();
		LPBITMAPINFOHEADER pBmiHeader = (LPBITMAPINFOHEADER)pPictData;
		pData = pPictData+pBmiHeader->biSize;
		iWidth = pBmiHeader->biWidth;
		iHeight = pBmiHeader->biHeight;
		ct = COMPRESSION_RGB_24;
		iMPEG4Width = iWidth;
		iMPEG4Height = iHeight;
	}

	if (m_pMPEG4EncoderQCIF == NULL)
	{
		// Encoder anlegen, falls noch nicht vorhanden
		m_pMPEG4EncoderQCIF = new CCameraMPEGEncoder(this);
		if (theApp.GetSettings().TraceH263())
		{
			WK_TRACE(_T("Init MPEG4 %d,%d\n"),iMPEG4Width,iMPEG4Height);
		}
		m_pMPEG4EncoderQCIF->Init(176,144,16000,5,50);
	}
	if (   m_pMPEG4EncoderQCIF
		&& pData)
	{
		if (m_MPEG4ClientsQCIF.GetMinCounter()>0)
		{
			TRACE(_T("Encode MPEG4 QCIF %08lx\n"),GetClientID().GetID());
			if (ct == COMPRESSION_YUV_422)
			{
				m_pMPEG4EncoderQCIF->SetYUV422Data(pict.GetCamID(),iWidth,iHeight,pData);
			}
			else if(ct == COMPRESSION_RGB_24)
			{
				m_pMPEG4EncoderQCIF->SetRGBData(pict.GetCamID(),iWidth,iHeight,pData);
			}
			else if(ct == COMPRESSION_JPEG)
			{
				m_pMPEG4EncoderQCIF->SetYUV422Data(pict.GetCamID(),iWidth,iHeight,pData);
			}
		}
		else
		{
			//	TRACE(_T("new QCIF Picture not enough client requests %08lx, %d\n"),GetUnitID().GetID(),m_iH263ClientRequestsQCIF);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CCamera::OnEncodedFrame(CompressionType ct)
{
	CIPCPictureRecord* pPict = NULL;

	if (ct == COMPRESSION_H263)
	{
		for (int i=0;i<m_dwH263ClientsQCIF.GetSize();i++)
		{
			COutputClient* pOC = theApp.GetOutputClients().GetOutputClient(m_dwH263ClientsQCIF[i]);
			if (   pOC
				&& pOC->IsConnected())
			{
				if (pPict == NULL)
				{
					CSystemTime t;
					t.GetLocalTime();
					pPict = m_pH26xEncoderQCIF->GetEncodedPicture(pOC->GetCIPC(),GetUnitID(),t);
				}
				if (pPict)
				{
					if (theApp.GetSettings().TraceH263())
					{
						WK_TRACE(_T("sending H.263 %s size %dB %dx%d %08lx\n"),
							(pPict->GetPictureType()==SPT_FULL_PICTURE) ? _T("I") : _T("P"),
							pPict->GetDataLength(),
							m_pH26xEncoderQCIF->GetWidth(),
							m_pH26xEncoderQCIF->GetHeight(),
							pPict->GetCamID().GetID());
					}
					pOC->GetCIPC()->SendPicture(this,*pPict,0,0,FALSE);
					InterlockedDecrement(&m_iH263ClientRequestsQCIF);
				}
			}
		}
	}
	else if (ct == COMPRESSION_MPEG4)
	{
		TRACE(_T("OnEncodedFrame %08lx\n"),GetClientID().GetID());
		for (int i=0;i<m_MPEG4ClientsQCIF.GetSize();i++)
		{
			COutputClient* pOC = theApp.GetOutputClients().GetOutputClient(m_MPEG4ClientsQCIF[i]->GetClientID());
			if (   pOC
				&& pOC->IsConnected())
			{
				if (pPict == NULL)
				{
					CSystemTime t;
					t.GetLocalTime();
					pPict = m_pMPEG4EncoderQCIF->GetEncodedPicture(pOC->GetCIPC(),GetUnitID(),t);
				}
				if (pPict)
				{
					if (theApp.GetSettings().TraceH263())
					{
						WK_TRACE(_T("sending MPEG4 %s size %dB %dx%d %08lx\n"),
							(pPict->GetPictureType()==SPT_FULL_PICTURE) ? _T("I") : _T("P"),
							pPict->GetDataLength(),
							m_pMPEG4EncoderQCIF->GetWidth(),
							m_pMPEG4EncoderQCIF->GetHeight(),
							pPict->GetCamID().GetID());
					}
					pOC->GetCIPC()->SendPicture(this,*pPict,0,0,FALSE);
					m_MPEG4ClientsQCIF[i]->Decrement();
				}
			}
		}
	}
	WK_DELETE(pPict);
}
//////////////////////////////////////////////////////////////////////////
void CCamera::NewDIBPicture(WORD wXRes, WORD wYRes, const CIPCPictureRecord& pict)
{
	TRACE(_T("NewDIBPicture %d,%d,%08lx\n"),wXRes,wYRes,pict.GetCamID().GetID());
	if (   m_pH26xEncoderQCIF == NULL
		&& m_dwH263ClientsQCIF.GetSize()>0)
	{
		// Encoder anlegen, falls noch nicht vorhanden

		m_pH26xEncoderQCIF = new CCameraH26xEncoder(this);
		m_pH26xEncoderQCIF->Init(wXRes,wYRes,0,0,FALSE);
	}
	if (m_pH26xEncoderQCIF)
	{
		if (m_iH263ClientRequestsQCIF >= m_dwH263ClientsQCIF.GetSize())
		{
			BYTE* pPictData = (BYTE*)pict.GetData();
			LPBITMAPINFOHEADER pBmiHeader = (LPBITMAPINFOHEADER)pPictData;
			BYTE* pData = pPictData+pBmiHeader->biSize;
			if (pBmiHeader->biCompression == BI_RGB)
			{
				m_pH26xEncoderQCIF->SetRGBData(pict.GetCamID(),wXRes,wYRes,pData);
			}
			else if (pBmiHeader->biCompression == mmioFOURCC('Y','4','2','2'))
			{
				m_pH26xEncoderQCIF->SetYUV422Data(pict.GetCamID(),wXRes,wYRes,pData);
			}
		}
		else
		{
//			TRACE(_T("new QCIF Picture not enough client requests %08lx, %d\n"),GetUnitID().GetID(),m_iH263ClientRequestsQCIF);
		}
	}
	if (   m_pMPEG4EncoderQCIF == NULL
		&& m_MPEG4ClientsQCIF.GetSize()>0)
	{
		// neuen MPEG4 encoder anlegen
		m_pMPEG4EncoderQCIF = new CCameraMPEGEncoder(this);
		m_pMPEG4EncoderQCIF->Init(176,144,16000,5,50);
	}
	if (m_pMPEG4EncoderQCIF)
	{
		if (m_MPEG4ClientsQCIF.GetMinCounter()>0)
		{
			BYTE* pPictData = (BYTE*)pict.GetData();
			LPBITMAPINFOHEADER pBmiHeader = (LPBITMAPINFOHEADER)pPictData;
			BYTE* pData = pPictData+pBmiHeader->biSize;
			if (pBmiHeader->biCompression == BI_RGB)
			{
				// OOPS even RGB as BGR ?
				m_pMPEG4EncoderQCIF->SetBGRData(pict.GetCamID(),wXRes,wYRes,pData);
			}
			else if (pBmiHeader->biCompression == mmioFOURCC('Y','4','2','2'))
			{
				m_pMPEG4EncoderQCIF->SetYUV422Data(pict.GetCamID(),wXRes,wYRes,pData);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CCamera::Disable()
{
	((CCameraGroup*)GetGroup())->ClearActiveCam(GetUnitID().GetSubID());

	DeletePictures();

	COutput::Disable();
}
//////////////////////////////////////////////////////////////////////////
void CCamera::Enable()
{
	COutput::Enable();

	((CCameraGroup*)GetGroup())->SetActiveCam(GetUnitID().GetSubID());
}
//////////////////////////////////////////////////////////////////////
void CCamera::FlushMDPictures()
{
	if (	(IsMD())
		&&	(m_wMDAlarmNr!=(WORD)-1)
		)
	{
		m_PreAlarmPictures.FlushPictures(this,m_wMDAlarmNr);
	}
}
//////////////////////////////////////////////////////////////////////
void CCamera::SavePicture(const CIPCPictureRecord& pict,								
						 CIPCFields& fields,
						 WORD wArchiveNr)
{
	fields.Lock();
	CIPCFields copy(fields);
	fields.Unlock();
/*
	if (IsUVV())
	{
		TRACE(_T("%s save UVV %dBytes %s-Frame\n"),
			GetName(),pict.GetDataLength(),(pict.GetPictureType() == SPT_FULL_PICTURE) ? _T("I"):_T("P"));
	}*/

	if (theApp.SavePicture(this,pict,copy,wArchiveNr))
	{
		m_stLastSave = pict.GetTimeStamp();
		m_idCurrentArchive = CSecID(SECID_GROUP_ARCHIVE,wArchiveNr);
		if (	(IsMD())
			&&	(m_wMDAlarmNr == wArchiveNr)
			)
		{
			m_iCurrentRecordedFramesPerDay++;
			m_iCurrentRecordedFramesPerWeek++;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CCamera::AddH263ClientQCIF(DWORD dwClientID)
{
//	TRACE(_T("AddH263ClientQCIF %08lx %d\n"),GetUnitID().GetID(),dwClientID);
	if (m_dwH263ClientsQCIF.GetSize() == 0)
	{
		// Client ID im Array Merken
		m_dwH263ClientsQCIF.Add(dwClientID);
		// DIB Daten der Unit einschalten!
		if (IsMD()
			&& GetGroup()->IsJacob())
		{
			GetGroup()->DoRequestSetValue(GetUnitID(),CSD_DIB_TRANSFER,CSD_ON);
		}
		// der Encoder wird erst mit dem ersten DIB Bild
		// in NewDIBPicture angelegt
	}
	else
	{
		BOOL bFound = FALSE;
		for (int i=0;i<m_dwH263ClientsQCIF.GetSize() && !bFound;i++)
		{
			bFound = m_dwH263ClientsQCIF[i] == dwClientID;
		}
		if (!bFound)
		{
			// Client ID im Array Merken
			m_dwH263ClientsQCIF.Add(dwClientID);
			// I Bild anfordern, da neuer Client
//			TRACE(_T("new Client ForceIntraFrame\n"));
			m_pH26xEncoderQCIF->ForceIntraFrame();
		}
	}
	InterlockedIncrement(&m_iH263ClientRequestsQCIF);
}
//////////////////////////////////////////////////////////////////////
void CCamera::DeleteH263ClientQCIF(DWORD dwClientID)
{
	BOOL bFound = FALSE;
	for (int i=0;i<m_dwH263ClientsQCIF.GetSize() && !bFound;i++)
	{
		bFound = m_dwH263ClientsQCIF[i] == dwClientID;
	}
	if (bFound)
	{
		m_dwH263ClientsQCIF.RemoveAt(i-1);
		if (m_dwH263ClientsQCIF.GetSize() == 0)
		{
			// DIB Daten der Unit ausschalten!
			if (   IsMD()
				&& GetGroup()->IsJacob())
			{
				GetGroup()->DoRequestSetValue(GetUnitID(),CSD_DIB_TRANSFER,CSD_OFF);
			}
			// Encoder entsorgen
			WK_DELETE(m_pH26xEncoderQCIF);
			if (m_MPEG4ClientsQCIF.GetSize() == 0)
			{
				WK_DELETE(m_pJpeg);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CCamera::DoTashaClientRequest()
{
	int iFps = 12;
	int iFrameIntervall = 24;
	if (m_cTashaLowbandwidthCompression == COMPRESSION_26)
	{
		iFps = 6;
	}

	if (m_dwTashaLowBitrateConfirm!=0)
	{
		if (GetTimeSpan(m_dwTashaLowBitrateConfirm)>5*1000)
		{
			WK_TRACE(_T("time out tasha stream 1 restart %08lx\n"));
			GetGroup()->DoRequestStopVideo(GetUnitID(),1);
		}
	}
	// start the request
	GetGroup()->DoRequestStartVideo(GetUnitID(),
									m_rTashaLowbandwidthResolution,
									m_cTashaLowbandwidthCompression,
									COMPRESSION_MPEG4,
									iFps,
									iFrameIntervall, // IFrame
									1 // ID
									);	
}
//////////////////////////////////////////////////////////////////////
void CCamera::AddMPEG4Client(COutputClient* pClient,Resolution res)
{
	if (GetGroup()->IsTasha())
	{
		m_TashaClients.Lock();
		BOOL bFound = FALSE;
		CClientCounter* pClientCounter = NULL; 
		for (int i=0;i<m_TashaClients.GetSize() && !bFound;i++)
		{
			bFound = m_TashaClients[i]->GetClientID() == pClient->GetID();
			if (bFound)
			{
				pClientCounter = m_TashaClients[i];
			}
		}
		if (pClientCounter == NULL)
		{
			// the 1st one start encoding
			if (pClient->GetBitrate() <= 64*1024)
			{
				m_cTashaLowbandwidthCompression = COMPRESSION_26;
			}
			if (res>m_rTashaLowbandwidthResolution)
			{
				m_rTashaLowbandwidthResolution = res;
			}
			pClientCounter = new CClientCounter(pClient->GetID());
			m_TashaClients.Add(pClientCounter);
			pClientCounter->Increment();

			if (m_TashaClients.GetSize() == 1)
			{
				// the first call and client
				CCameraGroup* pCameraGroup = (CCameraGroup*)GetGroup();
				// reduce storage to 12 fps
				pCameraGroup->StartRequest(this,IsInputActive());
			}
		}
		else
		{
			pClientCounter->Increment();
		}

		//
		TRACE(_T("TashaClients.GetMinCounter %d\n"),m_TashaClients.GetMinCounter());
		if (m_TashaClients.GetMinCounter()>0)
		{
			if (!bFound && m_TashaClients.GetSize()>1)
			{
				// stop to get a new I-Frame
				GetGroup()->DoRequestStopVideo(GetUnitID(),1);
			}
			DoTashaClientRequest();
		}
		m_TashaClients.Unlock();
	}
	else
	{
		DWORD dwClientID = pClient->GetID();
		CClientCounter* pClientCounter = NULL; 

		m_MPEG4ClientsQCIF.Lock();

		// JPEG/YUV to Mpeg4
		if (m_MPEG4ClientsQCIF.GetSize() == 0)
		{
			// Client ID im Array Merken
			pClientCounter = new CClientCounter(dwClientID);
			m_MPEG4ClientsQCIF.Add(pClientCounter);
			// DIB Daten der Unit einschalten!
			if (   IsMD()
				&& GetGroup()->IsJacob())
			{
				GetGroup()->DoRequestSetValue(GetUnitID(),CSD_DIB_TRANSFER,CSD_ON);
			}
			// der Encoder wird erst mit dem ersten DIB Bild
			// in NewDIBPicture angelegt
		}
		else
		{
			BOOL bFound = FALSE;
			for (int i=0;i<m_MPEG4ClientsQCIF.GetSize() && !bFound;i++)
			{
				bFound = m_MPEG4ClientsQCIF[i]->GetClientID() == dwClientID;
			}
			if (!bFound)
			{
				// Client ID im Array Merken
				pClientCounter = new CClientCounter(dwClientID);
				m_MPEG4ClientsQCIF.Add(pClientCounter);
				// I Bild anfordern, da neuer Client
				//			TRACE(_T("new Client ForceIntraFrame\n"));
				m_pMPEG4EncoderQCIF->ForceIntraFrame();
			}
			else
			{
				pClientCounter = m_MPEG4ClientsQCIF[i-1];
			}
		}
		if (pClientCounter)
		{
			pClientCounter->Increment();
		}
		m_MPEG4ClientsQCIF.Unlock();
		TRACE(_T("TashaClients.GetMinCounter %d\n"),m_MPEG4ClientsQCIF.GetMinCounter());
	}
}
//////////////////////////////////////////////////////////////////////
void CCamera::DeleteMPEG4Client(DWORD dwClientID)
{
	if (GetGroup()->IsTasha())
	{
		BOOL bFound = FALSE;
		for (int i=0;i<m_TashaClients.GetSize() && !bFound;i++)
		{
			bFound = m_TashaClients[i]->GetClientID() == dwClientID;
		}
		if (bFound)
		{
			WK_DELETE(m_TashaClients[i-1]);
			m_TashaClients.RemoveAt(i-1);
			if (m_TashaClients.GetSize() == 0)
			{
				// the last one
				GetGroup()->DoRequestStopVideo(GetUnitID(),1);
				// the first call and client
				CCameraGroup* pCameraGroup = (CCameraGroup*)GetGroup();
				// restore storage to 25 fps
				pCameraGroup->StartRequest(this,IsInputActive());
				m_cTashaLowbandwidthCompression = COMPRESSION_25;
				m_rTashaLowbandwidthResolution = RESOLUTION_CIF;
			}
		}
	}
	else
	{
		// JPEG/YUV2 to MPEG4
		BOOL bFound = FALSE;
		for (int i=0;i<m_MPEG4ClientsQCIF.GetSize() && !bFound;i++)
		{
			bFound = m_MPEG4ClientsQCIF[i]->GetClientID() == dwClientID;
		}
		if (bFound)
		{
			WK_DELETE(m_MPEG4ClientsQCIF[i-1]);
			m_MPEG4ClientsQCIF.RemoveAt(i-1);

			if (m_MPEG4ClientsQCIF.GetSize() == 0)
			{
				WK_DELETE(m_pMPEG4EncoderQCIF);
				if (m_dwH263ClientsQCIF.GetSize() == 0)
				{
					WK_DELETE(m_pJpeg);
				}
				// DIB Daten der Unit ausschalten!
				if (   IsMD()
					&& GetGroup()->IsJacob())
				{
					GetGroup()->DoRequestSetValue(GetUnitID(),CSD_DIB_TRANSFER,CSD_OFF);
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
DWORD CCamera::GetAverageSwitchtime()
{
	DWORD dwSum = 0;
	DWORD dwCount = 0;
	for (int i=0;i<m_dwSwitchTimes.GetSize();i++)
	{
		DWORD dwValue = m_dwSwitchTimes[i];
		if (dwValue>0)
		{
			dwSum += dwValue;
			dwCount++;
		}
	}
	if (dwCount>0)
	{
		return dwSum/ dwCount;
	}
	else
	{
		return 0;
	}
}
//////////////////////////////////////////////////////////////////////
DWORD CCamera::GetSwitchtimeVarianz()
{
	DWORD dwAverage = GetAverageSwitchtime();
	DWORD dwSum = 0;
	DWORD dwCount = 0;
	for (int i=0;i<m_dwSwitchTimes.GetSize();i++)
	{
		DWORD dwValue = m_dwSwitchTimes[i];
		if (dwValue>0)
		{
			if (dwValue > dwAverage)
				dwSum += dwValue-dwAverage;
			else
				dwSum += dwAverage-dwValue;
			dwCount++;
		}
	}
	if (dwCount>0)
	{
		return dwSum/ dwCount;
	}
	else
	{
		return 0;
	}
}
//////////////////////////////////////////////////////////////////////
int CCamera::GetJPEGQuality()
{
	return m_iJPEGQuality;
}
#define _ARRAY_LENGTH 20
//////////////////////////////////////////////////////////////////////
void CCamera::AddCompressedJPEGSize(DWORD dwSize)
{
	if (m_dwaLengths.GetSize()<_ARRAY_LENGTH)
	{
		m_dwSum += dwSize;
		m_dwaLengths.Add(dwSize);
	}
	else
	{
		m_dwSum -= m_dwaLengths.GetAt(m_iCurrentLengthIndex);
		m_dwSum += dwSize;
		m_dwaLengths[m_iCurrentLengthIndex] = dwSize;
		m_iCurrentLengthIndex++;
		if (m_iCurrentLengthIndex==m_dwaLengths.GetSize())
		{
			m_iCurrentLengthIndex = 0;
		}
	}
	int iAverageSize = m_dwSum / m_dwaLengths.GetSize();
	int iJpegSize = theApp.GetSettings().GetJPEGSize();

	switch (m_Compression)
	{
	case COMPRESSION_1:
		iJpegSize = iJpegSize;
		break;
	case COMPRESSION_2:
		iJpegSize = (iJpegSize*75)/100;
		break;
	case COMPRESSION_3:
		iJpegSize = iJpegSize/2;
		break;
	default:
		break;
	}
	switch (theApp.GetSettings().GetResolution())
	{
	case RESOLUTION_2CIF:
		iJpegSize = iJpegSize;
		break;
	case RESOLUTION_QCIF:
		iJpegSize = iJpegSize/2;
		break;
	}

	if (iAverageSize > ((iJpegSize*105)/100))
	{
		m_iJPEGQuality--;
		if (m_iJPEGQuality<50)
		{
			m_iJPEGQuality = 50;
		}
	}
	else if (iAverageSize < ((iJpegSize*95)/100))
	{
		m_iJPEGQuality++;
		if (m_iJPEGQuality>100)
		{
			m_iJPEGQuality = 100;
		}
	}
}
