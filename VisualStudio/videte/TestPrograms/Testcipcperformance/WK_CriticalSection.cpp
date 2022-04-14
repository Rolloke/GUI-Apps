// wk_CriticalSection.cpp : 
//

#include "stdafx.h"
#include "wk_CriticalSection.h"
#include "wkclasses\wk_utilities.h"

//////////////////////////////////////////////////////////////////////////
CPtrList CWK_Criticalsection::gm_CriticalSections;
BOOL CWK_Criticalsection::gm_bTraceLocks = -1;
DWORD CWK_Criticalsection::gm_dwLockTimeout = 250;

// RTTI
IMPLEMENT_DYNAMIC(CWK_Criticalsection, CCriticalSection)
//////////////////////////////////////////////////////////////////////////
// Initialisierung der Variablen, die den Zustand der CS speichern
// Speichern der CS in einer globalen Liste für zusätzliche Debugausgaben
CWK_Criticalsection::CWK_Criticalsection()
{
	m_nLockedThreadID = 0;
	m_nLockCount = 0;
//	m_sLockedPosition;
	if (gm_bTraceLocks == -1)
	{
		CWK_Profile wkp;
		gm_bTraceLocks   = wkp.GetInt(_T("Log"), _T("TraceLocks"), FALSE);
		gm_dwLockTimeout = wkp.GetInt(_T("Log"), _T("LockTimeOut"), gm_dwLockTimeout);
	}
	if (gm_bTraceLocks)
	{
		gm_CriticalSections.AddTail(this);
	}
}
CWK_Criticalsection::CWK_Criticalsection(LPCTSTR sName)
{
	m_nLockedThreadID = 0;
	m_nLockCount = 0;
	m_sObjectName = sName;
//	m_sLockedPosition;
	if (gm_bTraceLocks)
	{
		gm_CriticalSections.AddTail(this);
	}
}
//////////////////////////////////////////////////////////////////////////
// Debugausgabe, wenn die CS hier noch gelockt ist
// Entfernen der CS aus der globalen Liste
CWK_Criticalsection::~CWK_Criticalsection()
{
	if (gm_bTraceLocks)
	{
		if (m_nLockedThreadID != 0)
		{
			WK_TRACE(_T("Locked section %d times: %s\n"), m_sLockedPosition, m_nLockCount);
		}
		POSITION pos = gm_CriticalSections.Find(this);
		if (pos)
		{
			gm_CriticalSections.RemoveAt(pos);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
// zusätzliche Features:
// Abfrage, ob die CS gelocked ist.
BOOL CWK_Criticalsection::IsLocked()
{
	return m_nLockCount > 0;
}
//////////////////////////////////////////////////////////////////////////
// Lock within a timeout value
BOOL CWK_Criticalsection::Lock(DWORD dwTimeout)
{
	DWORD dwTick = GetTickCount();
	while (m_nLockCount > 0)
	{
		Sleep(1);
		if (dwTimeout <= (GetTickCount() - dwTick))
		{
			return FALSE;
		}
	}
	return Lock((LPCTSTR)NULL);
}
#if(_WIN32_WINNT >= 0x0400)
//////////////////////////////////////////////////////////////////////////
// TryEnterCriticalSection
BOOL CWK_Criticalsection::TryLock()
{
	return TryEnterCriticalSection(&m_sect);
}
#endif
#if (_WIN32_WINNT >= 0x0403)
//////////////////////////////////////////////////////////////////////////
// SetSpinCount: for multiprozessor systems
DWORD CWK_Criticalsection::SetSpinCount(DWORD dwSpinCount)
{
	return SetCriticalSectionSpinCount(&m_sect, dwSpinCount);
}
#endif
//////////////////////////////////////////////////////////////////////////
// Überschreiben der virtuellen Funktion der Basisklasse, um den Lockcounter
// korrekt zu zählen
BOOL CWK_Criticalsection::Lock()
{
	return CWK_Criticalsection::Lock((LPCTSTR)NULL);
}
//////////////////////////////////////////////////////////////////////////
// Positionsangabe im Quellcode
BOOL CWK_Criticalsection::Lock(LPCTSTR sFile, int nLine)
{
	CString sPosition;
	sPosition.Format(_T("%s(%d):"), sFile, nLine);
	return CWK_Criticalsection::Lock(sPosition);
}
//////////////////////////////////////////////////////////////////////////
// Positionsangabe mit Funktionsnamen
// Erhöhen des LockCounters
// Ermitteln von Timeout und ggf. Ausgabe wer von wem blockiert wird
// Speichern der aktuellen Position und der ThreadID bei LockCount == 0
BOOL CWK_Criticalsection::Lock(LPCTSTR pszPosition/*=NULL*/)
{
	BOOL bReturn;
	if (gm_bTraceLocks)
	{
		DWORD dwTick = GetTickCount();
		bReturn = CCriticalSection::Lock();
		if (0 == m_nLockCount++) 
		{
			DWORD dwTimeout = GetTickCount() - dwTick;
			if (dwTimeout > gm_dwLockTimeout)
			{
				WK_TRACE(_T("Timeout entering CS %s locked by: %s for %d ms\n"), m_sObjectName, m_sLockedPosition, dwTimeout);
			}
            m_nLockedThreadID = GetCurrentThreadId();
			m_sLockedPosition = XTIB::GetThreadName();
			if (pszPosition)
			{
				m_sLockedPosition = m_sLockedPosition + _T(" in ") + pszPosition;
			}
			if (dwTimeout > gm_dwLockTimeout)
			{
				WK_TRACE(_T("Entering: %s in %s\n"), m_sLockedPosition, XTIB::GetThreadName());
			}
		}
	}
	else
	{
		bReturn = CCriticalSection::Lock();
		m_nLockCount++;
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////////
// Verringern des LockCounters
// Zurücksetzen der Variablen mit Ausnahme der Position, die evtl.
// noch gebraucht wird.
BOOL CWK_Criticalsection::Unlock()
{
	if (gm_bTraceLocks)
	{
		if (0 == --m_nLockCount)
		{
//			m_sLockedPosition.Empty();
			m_nLockedThreadID = 0;
		}
		if (m_nLockCount < 0)
		{
			WK_TRACE(_T("Error LockCount negative in (%s), %s in %s\n"), m_sObjectName, m_sLockedPosition, XTIB::GetThreadName());
			m_nLockCount = 0;
		}
	}
	else if (m_nLockCount > 0)
	{
		m_nLockCount--;
	}
	return CCriticalSection::Unlock();
}
//////////////////////////////////////////////////////////////////////////
void CWK_Criticalsection::SetTraceLocks(BOOL bTrace, DWORD dwLockTimeout)
{
	gm_bTraceLocks = bTrace;
	gm_dwLockTimeout = dwLockTimeout;
}
//////////////////////////////////////////////////////////////////////////
// Ausgabe
void CWK_Criticalsection::TraceLockedSections(CDWordArray*pLockedThreads/*=NULL*/)
{
	if (gm_bTraceLocks)
	{
		WK_TRACE(_T("Locked sections:\n"));
		POSITION pos = gm_CriticalSections.GetHeadPosition();
		while (pos)
		{
			CWK_Criticalsection*pCs = (CWK_Criticalsection*)gm_CriticalSections.GetNext(pos);
			if (pCs->m_nLockedThreadID)
			{
				WK_TRACE(_T("* %s\n"), pCs->m_sLockedPosition);
				if (pLockedThreads)
				{
					pLockedThreads->Add(pCs->m_nLockedThreadID);
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
