// Piezo.cpp: implementation of the CPiezo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvstarter.h"
#include "Piezo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
CPiezo::CPiezo()
{
	m_dwPiezoFlags = 0;
	m_hJaCobPiezo = NULL;
	m_hSaVicPiezo = NULL;
	m_hTashaPiezo = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CPiezo::~CPiezo()
{
	WK_CLOSE_HANDLE(m_hJaCobPiezo);
	WK_CLOSE_HANDLE(m_hSaVicPiezo);
	WK_CLOSE_HANDLE(m_hTashaPiezo);
}

/////////////////////////////////////////////////////////////////////////////
void CPiezo::InitPiezo()
{
	CWK_Profile wkp;

	WK_CLOSE_HANDLE(m_hJaCobPiezo);
	m_hJaCobPiezo = CreateEvent(NULL, FALSE, FALSE, EV_JACOB_PIEZO);
	wkp.WriteInt(_T("JaCobUnit\\Device1\\General"),_T("IgnoreRelais4"),TRUE);

	WK_CLOSE_HANDLE(m_hSaVicPiezo);
	m_hSaVicPiezo = CreateEvent(NULL, FALSE, FALSE, EV_SAVIC_PIEZO);
	wkp.WriteInt(_T("SaVicUnit\\Device1\\General"),_T("IgnoreRelais4"),TRUE);

	WK_CLOSE_HANDLE(m_hTashaPiezo);
	m_hTashaPiezo = CreateEvent(NULL, FALSE, FALSE, EV_TASHA_PIEZO);
	wkp.WriteInt(_T("TashaUnit\\Device1\\General"),_T("IgnoreRelais4"),TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CPiezo::ExitPiezo()
{
	WK_CLOSE_HANDLE(m_hJaCobPiezo);
	WK_CLOSE_HANDLE(m_hSaVicPiezo);
	WK_CLOSE_HANDLE(m_hTashaPiezo);
}
/////////////////////////////////////////////////////////////////////////////
void CPiezo::SetPiezo(BOOL bOn)
{
	if (m_hJaCobPiezo)
	{
		CWK_Profile wkp;
		wkp.WriteInt(_T("JaCobUnit\\Device1\\General"),_T("IgnoreRelais4"),TRUE);
		wkp.WriteInt(_T("JaCobUnit\\Device1\\General"),_T("PiezoState"),bOn);
		SetEvent(m_hJaCobPiezo);
//		WK_STAT_LOG(_T("Piezo JaCob"),bOn,_T("Piezo"));
	}

	if (m_hSaVicPiezo)
	{
		CWK_Profile wkp;
		wkp.WriteInt(_T("SaVicUnit\\Device1\\General"),_T("IgnoreRelais4"),TRUE);
		wkp.WriteInt(_T("SaVicUnit\\Device1\\General"),_T("PiezoState"),bOn);
		SetEvent(m_hSaVicPiezo);
//		WK_STAT_LOG(_T("Piezo SaVic"),bOn,_T("Piezo"));
	}

	if (m_hTashaPiezo)
	{
		CWK_Profile wkp;
		wkp.WriteInt(_T("TashaUnit\\Device1\\General"),_T("IgnoreRelais4"),TRUE);
		wkp.WriteInt(_T("TashaUnit\\Device1\\General"),_T("PiezoState"),bOn);
		SetEvent(m_hTashaPiezo);
		//		WK_STAT_LOG(_T("Piezo Tasha"),bOn,_T("Piezo"));
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPiezo::PiezoSetFlag(DWORD dwPiezoFlag)
{
	BOOL bOn;

	if (   (m_hJaCobPiezo == NULL) 
		|| (m_hSaVicPiezo == NULL)
		|| (m_hTashaPiezo == NULL)
		)
		return FALSE;

	if (dwPiezoFlag>0)
		m_dwPiezoFlags |= dwPiezoFlag;

	bOn = (m_dwPiezoFlags>0);

	CWK_Profile wkp;
	wkp.WriteInt(_T("JaCobUnit\\Device1\\General"),_T("PiezoState"),bOn);
	SetEvent(m_hJaCobPiezo);

	wkp.WriteInt(_T("SaViCUnit\\Device1\\General"),_T("PiezoState"),bOn);
	SetEvent(m_hSaVicPiezo);
	
	wkp.WriteInt(_T("TashaUnit\\Device1\\General"),_T("PiezoState"),bOn);
	SetEvent(m_hTashaPiezo);

	//	WK_STAT_LOG(_T("STM"),bOn,_T("JaCob STM"));

	return TRUE;	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPiezo::PiezoClearFlag(DWORD dwPiezoFlag)
{
	BOOL bOn;

	if (   (m_hJaCobPiezo == NULL) 
		|| (m_hSaVicPiezo == NULL)
		|| (m_hTashaPiezo == NULL)
		)
		return FALSE;

	if (dwPiezoFlag>0)
		m_dwPiezoFlags &= ~dwPiezoFlag;

	bOn = (m_dwPiezoFlags>0);

	CWK_Profile wkp;
	wkp.WriteInt(_T("JaCobUnit\\Device1\\General"),_T("PiezoState"),bOn);
	SetEvent(m_hJaCobPiezo);

	wkp.WriteInt(_T("SaVicUnit\\Device1\\General"),_T("PiezoState"),bOn);
	SetEvent(m_hSaVicPiezo);
	
	wkp.WriteInt(_T("TashaUnit\\Device1\\General"),_T("PiezoState"),bOn);
	SetEvent(m_hTashaPiezo);

	//	WK_STAT_LOG(_T("STM"),bOn,_T("JaCob STM"));

	return TRUE;	
}
