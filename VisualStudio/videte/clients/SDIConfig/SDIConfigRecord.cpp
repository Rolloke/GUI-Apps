/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIConfigRecord.cpp $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIConfigRecord.cpp $
// CHECKIN:		$Date: 16.01.98 9:57 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 14.01.98 14:24 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "SDIConfigRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CSDIConfigRecord::CSDIConfigRecord(CSecID id, CString& sComment)
{
	m_id = id;
	m_sComment = sComment;
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigRecord::~CSDIConfigRecord()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigRecord::SetComment(CString& sComment)
{
	m_sComment = sComment;
}
