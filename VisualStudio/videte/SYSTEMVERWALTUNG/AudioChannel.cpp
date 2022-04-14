/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: AudioChannel.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/AudioChannel.cpp $
// CHECKIN:		$Date: 21.06.04 13:05 $
// VERSION:		$Revision: 9 $
// LAST CHANGE:	$Modtime: 21.06.04 10:49 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "AudioChannel.h"

//////////////////////////////////////////////////////////////////////////////
// CAudioChannel
//////////////////////////////////////////////////////////////////////////////
	
CAudioChannel::CAudioChannel(LPCTSTR sChannel, int nChannel, int nStereoChannel,
									  int nUnit, LPCTSTR sDeviceName, bool bInput)
{
	m_sChannel       = sChannel;
	m_nChannel       = nChannel;
	m_nUnit          = nUnit;
	m_sDeviceName    = sDeviceName;

	m_bActive        = FALSE;
	m_bDefault       = false;
	m_bStereo        = false;
	m_bInput         = bInput;
}
//////////////////////////////////////////////////////////////////////////////
const CAudioChannel& CAudioChannel::operator=(const CAudioChannel& inp)
{
	m_sChannel   = inp.m_sChannel;
	m_nChannel   = inp.m_nChannel;
	m_nUnit      = inp.m_nUnit;
	
	m_bActive    = inp.m_bActive;
	m_bDefault   = inp.m_bDefault;
	m_bStereo    = inp.m_bStereo;
	m_sNameLeft  = inp.m_sNameLeft;
	m_sNameRight = inp.m_sNameRight;

	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CAudioChannel::~CAudioChannel()
{
}
//////////////////////////////////////////////////////////////////////////////
void CAudioChannel::Load(CWK_Profile &prof)
{
	CString sSection, sName;
	sSection.Format(_T("%s\\%s\\%s\\%s"), AUDIO_UNIT, m_bInput ? SEC_MEDIA_TYPE_INPUT : SEC_MEDIA_TYPE_OUTPUT, m_sDeviceName, m_sChannel);

	m_bStereo  = prof.GetInt(sSection, AU_STEREO , 0) ? true : false;
	m_nChannel = prof.GetInt(sSection, AU_CHANNEL, 0);
	m_bActive  = prof.GetInt(sSection, AU_ACTIVE , 0);
	
	sName      = prof.GetString(sSection, m_bInput ? AU_INPUT_NAME : AU_OUTPUT_NAME, NULL);
	if (sName.IsEmpty() || !m_bActive)
	{
		if (m_bInput)
		{
			m_sNameLeft.Empty();
			m_sNameRight.Empty();
		}
		else
		{
			m_sNameLeft.LoadString(IDS_AUDIO_OUTPUTNAME);
		}
	}
	else
	{
		int nFind = sName.Find(_T(";"));
		if (nFind != -1)
		{
			m_sNameLeft  = sName.Left(nFind);
			m_sNameRight = sName.Mid(nFind+1);
		}
		else
		{
			m_sNameLeft = sName;
		}
	}

	if (m_bInput)
	{
		sSection.Format(AUDIO_UNIT_DEVICE _T("\\%s"), m_nUnit, AU_AUDIO_SETTINGS);
		m_nStereoChannel = prof.GetInt(sSection, AU_CHANNEL, 0);
		m_bDefault       = prof.GetInt(sSection, AU_INPUT_CHANNEL, -1) == m_nChannel ? true : false;
		if (!m_bStereo) 
		{
			m_nStereoChannel = 0;
		}
	}
	else
	{
		m_nStereoChannel = 0;
		m_bDefault = true;
	}
}
//////////////////////////////////////////////////////////////////////////////
void CAudioChannel::Save(CWK_Profile &prof)
{
	CString sSection, sName;
	sSection.Format(_T("%s\\%s\\%s\\%s"), AUDIO_UNIT, m_bInput ? SEC_MEDIA_TYPE_INPUT : SEC_MEDIA_TYPE_OUTPUT, m_sDeviceName, m_sChannel);
	prof.WriteInt(sSection, AU_ACTIVE, m_bActive);
	
	sName = m_sNameLeft;
	if (!m_sNameRight.IsEmpty())
	{
		sName = sName + _T(";") + m_sNameRight;
	}
	
	prof.WriteString(sSection, m_bInput ? AU_INPUT_NAME : AU_OUTPUT_NAME, sName);

	if (m_bInput)
	{
		if (m_bDefault)
		{
			sSection.Format(AUDIO_UNIT_DEVICE _T("\\%s"), m_nUnit, AU_AUDIO_SETTINGS);
			prof.WriteInt(sSection, AU_INPUT_CHANNEL, m_nChannel);
			prof.WriteInt(sSection, AU_CHANNEL      , m_nStereoChannel);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CAudioChannel::SetActive(int nC, bool bActive)
{
	if (bActive)
	{
		m_bActive |= (1 << nC);
	}
	else
	{
		m_bActive &= ~(1 << nC);
	}
}
//////////////////////////////////////////////////////////////////////////
void CAudioChannel::SetName(int nC, const CString &sName)
{
	if (nC == 0)
	{
		m_sNameLeft  = sName;
	}
	else
	{
		m_sNameRight = sName;
	}
}
//////////////////////////////////////////////////////////////////////////
const CString&	CAudioChannel::GetName(int nC)
{
	if (nC == 0)
	{
		return m_sNameLeft;
	}
	else
	{
		return m_sNameRight;
	}
}
//////////////////////////////////////////////////////////////////////////
bool CAudioChannel::IsActive(int nC)
{
	return m_bActive & (1 << nC) ? true : false;
}
