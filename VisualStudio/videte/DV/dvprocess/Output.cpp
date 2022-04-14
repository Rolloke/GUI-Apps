// Output.cpp: implementation of the COutput class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "Output.h"
#include "CameraGroup.h"

#include "IPCDatabaseProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COutput::COutput(COutputGroup* pGroup, CSecID id)
{
	m_pGroup = pGroup;
	m_Mode = OM_INVALID;
	m_UnitID = id;
	m_bActive = TRUE;
	m_bTimerActive = FALSE;

//    BYTE bMax = (BYTE)pGroup->GetSize();
}
//////////////////////////////////////////////////////////////////////
COutput::~COutput()
{
}
//////////////////////////////////////////////////////////////////////
void COutput::SetClientID(CSecID id)
{
	m_ClientID = id;
	m_sName.Format(_T("%02d"),m_ClientID.GetSubID()+1);
}
//////////////////////////////////////////////////////////////////////
void COutput::SetActive(BOOL bActive)
{
	m_bActive = bActive;
}
//////////////////////////////////////////////////////////////////////
void COutput::SetMode(OutputMode mode)
{
	m_Mode = mode;
}
//////////////////////////////////////////////////////////////////////
void COutput::SetTimerActive(BOOL bTimerActive)
{
	if (m_bTimerActive != bTimerActive)
	{
		m_bTimerActive = bTimerActive;
		WK_TRACE(_T("<%s> TIMER %s\n"),GetName(),m_bTimerActive ? _T("ON") : _T("OFF"));
	}
}
//////////////////////////////////////////////////////////////////////
BOOL COutput::IsTimerActive() const
{
	return m_bTimerActive;
}
//////////////////////////////////////////////////////////////////////
void COutput::SetName(const CString& sName)
{
	m_sName = sName;
}
//////////////////////////////////////////////////////////////////////
void COutput::FromString(const CString& s)
{
	m_sName = CWK_Profile::GetStringFromString(s,_T("CO"),m_sName);
	m_Mode = (OutputMode)CWK_Profile::GetNrFromString(s,_T("TYP"),(int)m_Mode);
}
//////////////////////////////////////////////////////////////////////
CString COutput::ToString()
{
	CString s;

	s.Format(_T("\\CO%s\\TYP%d"),
		m_sName,
		(int)m_Mode);

	return s;
}
//////////////////////////////////////////////////////////////////////
CString COutput::Format()
{
	CString s;
	s.Format(_T("U%08lx %s "),
		GetUnitID().GetID(),
		m_sName);
	return s;
}
//////////////////////////////////////////////////////////////////////
void COutput::Lock()
{
	m_cs.Lock();
}
//////////////////////////////////////////////////////////////////////
void COutput::Unlock()
{
	m_cs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void COutput::Disable()
{
	m_bActive = FALSE;
	theApp.SetReset(FALSE,FALSE,TRUE);
	if (WK_IS_WINDOW(theApp.m_pMainWnd))
	{
		theApp.m_pMainWnd->PostMessage(WM_USER);
	}
}
//////////////////////////////////////////////////////////////////////
void COutput::Enable()
{
	if (!m_bActive)
	{
		m_bActive = TRUE;
		theApp.SetReset(FALSE,TRUE,TRUE);
	}
}