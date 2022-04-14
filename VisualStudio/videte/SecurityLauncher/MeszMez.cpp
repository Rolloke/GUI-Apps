// MeszMez.cpp: implementation of the CMeszMez class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SecurityLauncher.h"
#include "MeszMez.h"

// static TCHAR BASED_CODE szSection[] = _T("SecurityLauncher");
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeszMez::CMeszMez()
{
	m_bAutoMESZ = FALSE;
	m_bDCF = FALSE;
	m_month2MESZ = 0;
	m_month2MEZ = 0;
}
/////////////////////////////////////////////////////////////////////////////
CMeszMez::~CMeszMez()
{

}
/////////////////////////////////////////////////////////////////////////////
int CMeszMez::DaysOfMonth(LPSYSTEMTIME lpST)
{
	int x = 31;

	if (lpST->wMonth>7)
	{
		x = (lpST->wMonth & 1) ? 30 : 31;
	}
	else if (lpST->wMonth==2)
	{
		x = ((lpST->wYear & 3) == 0) ? 29 : 28;
	}
	else
	{
		x = (lpST->wMonth & 1) ? 31 : 30;
	}

	return x;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMeszMez::IsLastSundayOfMonth(LPSYSTEMTIME lpST)
{
	if (lpST->wDayOfWeek == 0)
	{
		// it's a sunday
		if ((lpST->wDay + 7) > DaysOfMonth(lpST))
		{
			return TRUE;
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CMeszMez::Init()
{
	CWK_Profile wkp;

	m_bDCF = wkp.GetInt(theApp.GetSection(),_T("DCFSync"),FALSE);
	if (m_bDCF)
	{
		WK_TRACE(_T("Sommer- Winterzeitumstellung ist aus ; Synchronisation mit Funkuhr\n"));
	}

	if (theApp.IsWin95())
	{
		m_bAutoMESZ = wkp.GetInt(theApp.GetSection(),_T("AutoMESZ"),TRUE);

		if (m_bAutoMESZ)
		{
			m_month2MESZ = wkp.GetInt(theApp.GetSection(),_T("MonthMESZ"),3);
			m_month2MEZ = wkp.GetInt(theApp.GetSection(),_T("MonthMEZ"),10);

			WK_TRACE(_T("Sommer- auf Winterzeit: %02d\n"),m_month2MEZ);
			WK_TRACE(_T("Winter- auf Sommerzeit: %02d\n"),m_month2MESZ);
		}
		else
		{
			WK_TRACE(_T("Sommer- Winterzeitumstellung ist aus\n"));
		}
	}
	else if (!m_bDCF)
	{
		WK_TRACE(_T("Sommer- Winterzeitumstellung unter NT vom System\n"));
		m_month2MESZ = 3;
		m_month2MEZ = 10;
		WK_TRACE(_T("Sommer- auf Winterzeit: %02d\n"),m_month2MEZ);
		WK_TRACE(_T("Winter- auf Sommerzeit: %02d\n"),m_month2MESZ);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMeszMez::IsAfterMesz2Mez(LPSYSTEMTIME lpST)
{
	if (lpST->wMonth == m_month2MEZ)
	{
		if (IsLastSundayOfMonth(lpST))
		{
			if ((lpST->wHour==3) && (lpST->wMinute==0) && (lpST->wSecond<20))
			{
				CWK_Profile wkp;
				CString sTime = wkp.GetString(theApp.GetSection(),_T("Time"),_T(""));
				if ((0==sTime.CompareNoCase(_T("MESZ"))) || sTime.IsEmpty())
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMeszMez::IsAfterMez2Mesz(LPSYSTEMTIME lpST)
{
	if (lpST->wMonth == m_month2MESZ)
	{
		if (IsLastSundayOfMonth(lpST))
		{
			if ((lpST->wHour==2) && (lpST->wMinute==0) && (lpST->wSecond<20))
			{
				CWK_Profile wkp;
				CString sTime = wkp.GetString(theApp.GetSection(),_T("Time"),_T(""));
				if ((0==sTime.CompareNoCase(_T("MEZ"))) || sTime.IsEmpty())
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMeszMez::IsBeforeMesz2Mez(LPSYSTEMTIME lpST)
{
	if (lpST->wMonth == m_month2MEZ)
	{
		if (IsLastSundayOfMonth(lpST))
		{
			if ((lpST->wHour==2) && (lpST->wMinute==59) && (lpST->wSecond>=40))
			{
				CWK_Profile wkp;
				CString sTime = wkp.GetString(theApp.GetSection(),_T("Time"),_T(""));
				if ((0==sTime.CompareNoCase(_T("MESZ"))) || sTime.IsEmpty())
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMeszMez::IsBeforeMez2Mesz(LPSYSTEMTIME lpST)
{
	if (lpST->wMonth == m_month2MESZ)
	{
		if (IsLastSundayOfMonth(lpST))
		{
			if ((lpST->wHour==1) && (lpST->wMinute==59) && (lpST->wSecond>=40))
			{
				CWK_Profile wkp;
				CString sTime = wkp.GetString(theApp.GetSection(),_T("Time"),_T(""));
				if ((0==sTime.CompareNoCase(_T("MEZ"))) || sTime.IsEmpty())
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CMeszMez::Check95()
{
	if (m_bAutoMESZ && !m_bDCF)
	{
		CWK_Profile wkp;
		CSystemTime st;

		st.GetLocalTime();

		if (IsAfterMez2Mesz(&st))
		{
			WK_TRACE(_T("Stelle Uhr auf Sommerzeit um!\n"));
			theApp.m_WatchDog.Reset(300);
			theApp.TerminateAll();
			st.wHour = 3;
			SetLocalTime(&st);
			PostMessage(HWND_BROADCAST,WM_TIMECHANGE,0,0);
			theApp.StartAll();
			WK_TRACE(_T("Uhr auf Sommerzeit umgestellt!\n"));
			wkp.WriteString(theApp.GetSection(),_T("Time"),_T("MESZ"));
		}
		if (IsAfterMesz2Mez(&st))
		{
			WK_TRACE(_T("Stelle Uhr auf Winterzeit um!\n"));
			theApp.m_WatchDog.Reset(300);
			theApp.TerminateAll();
			st.wHour = 2;
			SetLocalTime(&st);
			PostMessage(HWND_BROADCAST,WM_TIMECHANGE,0,0);
			theApp.StartAll();
			WK_TRACE(_T("Uhr auf Winterzeit umgestellt!\n"));
			wkp.WriteString(theApp.GetSection(),_T("Time"),_T("MEZ"));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMeszMez::CheckNT()
{
	CWK_Profile wkp;
	SYSTEMTIME st;

	GetLocalTime(&st);

	if (!m_bDCF)
	{
		if (IsBeforeMez2Mesz(&st))
		{
			WK_TRACE(_T("Software runterfahren wegen Sommerzeit!\n"));
			theApp.m_WatchDog.Reset(300);
			theApp.TerminateAll(TRUE);
			wkp.WriteString(theApp.GetSection(),_T("Time"),_T("MESZ"));
		}
		if (IsBeforeMesz2Mez(&st))
		{
			WK_TRACE(_T("Software runterfahren wegen Winterzeit!\n"));
			theApp.m_WatchDog.Reset(300);
			theApp.TerminateAll(TRUE);
			wkp.WriteString(theApp.GetSection(),_T("Time"),_T("MEZ"));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMeszMez::Check()
{
	if (theApp.IsWin95())
	{
		Check95();
	}
	else if (theApp.IsNT())
	{
		CheckNT();
	}
}
