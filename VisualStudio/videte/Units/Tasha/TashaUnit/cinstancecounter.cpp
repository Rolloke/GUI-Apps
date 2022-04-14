/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: cinstancecounter.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/cinstancecounter.cpp $
// CHECKIN:		$Date: 13.04.04 10:06 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 13.04.04 8:40 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CInstanceCounter.h"

CInstanceCounter::CInstanceCounter()
{
	if (!WK_IS_RUNNING(WK_APP_NAME_TASHAUNIT1))
	{
		WK_ALONE(WK_APP_NAME_TASHAUNIT1);
		m_nInstance = 0;
		m_AppID= WAI_TASHAUNIT_1;
	}
	else if (!WK_IS_RUNNING(WK_APP_NAME_TASHAUNIT2))
	{
		WK_ALONE(WK_APP_NAME_TASHAUNIT2);
		m_nInstance = 1;
		m_AppID = WAI_TASHAUNIT_2;
	}
	else if (!WK_IS_RUNNING(WK_APP_NAME_TASHAUNIT3))
	{
		WK_ALONE(WK_APP_NAME_TASHAUNIT3);
		m_nInstance = 2;
		m_AppID = WAI_TASHAUNIT_3;
	}
	else if (!WK_IS_RUNNING(WK_APP_NAME_TASHAUNIT4))
	{
		WK_ALONE(WK_APP_NAME_TASHAUNIT4);
		m_nInstance = 3;
		m_AppID = WAI_TASHAUNIT_4;
	}
	else
		WK_TRACE_ERROR(_T("CInstanceCounter::CInstanceCounter. Too many instance\n"));
}

//////////////////////////////////////////////////////////////////////
CInstanceCounter::~CInstanceCounter()
{
}

