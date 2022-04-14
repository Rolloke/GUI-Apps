// InstanceCounter.cpp: implementation of the CInstanceCounter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CInstanceCounter.h"

CInstanceCounter::CInstanceCounter()
{
	if (!WK_IS_RUNNING(WK_APP_NAME_SAVICUNIT1))
	{
		m_nInstance = 0;
		m_AppID = WAI_SAVICUNIT_1;
	}
	else if (!WK_IS_RUNNING(WK_APP_NAME_SAVICUNIT2))
	{
		m_nInstance = 1;
		m_AppID = WAI_SAVICUNIT_2;
	}
	else if (!WK_IS_RUNNING(WK_APP_NAME_SAVICUNIT3))
	{
		m_nInstance = 2;
		m_AppID = WAI_SAVICUNIT_3;
	}
	else if (!WK_IS_RUNNING(WK_APP_NAME_SAVICUNIT4))
	{
		m_nInstance = 3;
		m_AppID = WAI_SAVICUNIT_4;
	}
}

//////////////////////////////////////////////////////////////////////
CInstanceCounter::~CInstanceCounter()
{
}

