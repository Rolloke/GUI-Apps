/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: media.cpp $
// ARCHIVE:		$Archive: /DV/DVProcess/media.cpp $
// CHECKIN:		$Date: 26.02.04 12:39 $
// VERSION:		$Revision: 9 $
// LAST CHANGE:	$Modtime: 26.02.04 12:37 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "dvprocess.h"
#include "Media.h"
#include "Camera.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////////
// CMedia
//////////////////////////////////////////////////////////////////////////////
CMedia::CMedia(CIPCAudioServer* pGroup,WORD wGroupID, WORD wSubID)
{
	m_id = CSecID(wGroupID,wSubID);
	m_pGroup = pGroup;
	m_pCamera = NULL;
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
//////////////////////////////////////////////////////////////////////
CString CMedia::Format()
{
	CString s;
	s.Format(_T("%08lx %s %s "),
		GetID().GetID(),
		GetName(),
		GetType()
		);
	return s;
}
//////////////////////////////////////////////////////////////////////
void CMedia::SetCamera(CCamera* pCamera)
{
	m_pCamera = pCamera;
	if (pCamera)
	{
		m_sName = pCamera->GetName();
	}
}
