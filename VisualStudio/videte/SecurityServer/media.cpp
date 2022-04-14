/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: media.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/media.cpp $
// CHECKIN:		$Date: 12.08.04 14:23 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 12.08.04 8:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "sec3.h"
#include "Media.h"
#include "IOGroup.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
// CMedia
//////////////////////////////////////////////////////////////////////////////
CMedia::CMedia(WORD wGroupID, WORD wSubID)
{
	m_id = CSecID(wGroupID,wSubID);

}
//////////////////////////////////////////////////////////////////////////////
const CMedia& CMedia::operator=(const CMedia& inp)
{
	m_id = inp.m_id;
	m_sName = inp.m_sName;
	m_sType = inp.m_sType;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CMedia::~CMedia()
{
}
//////////////////////////////////////////////////////////////////////////////
BOOL CMedia::Load(CWK_Profile& wkp)
{
	CString sSection;
	CString sMediaDefine;

	sSection.Format(_T("%s\\Group%04X"),SEC_NAME_MEDIAGROUPS, m_id.GetGroupID());
	sMediaDefine = wkp.GetString(sSection,m_id.GetSubID()+1,_T(""));

	if (sMediaDefine.IsEmpty())
	{
		return FALSE;
	}
	m_bIsAudio = false;
	m_bIsVideo = false;
	// name
	m_sName = wkp.GetStringFromString(sMediaDefine, INI_COMMENT, _T(""));
	m_sType = wkp.GetStringFromString(sMediaDefine, INI_TYPE, _T(""));
	CString sSpecInfo = wkp.GetStringFromString(sMediaDefine, INI_SPECIFIC_INFO, _T(""));
	if      (sSpecInfo == MEDIA_IS_AUDIO) m_bIsAudio = true;
	else if (sSpecInfo == MEDIA_IS_VIDEO) m_bIsVideo = true;
	m_bIsDefault = wkp.GetNrFromString(sMediaDefine, INI_DEFAULT , 0) ? true : false; 
	m_bIsEnabled = wkp.GetNrFromString(sMediaDefine, INI_DISABLED, 0) ? false : true; 

	return TRUE;
}
