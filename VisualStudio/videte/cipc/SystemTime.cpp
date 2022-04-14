// SystemTime.cpp: implementation of the CSystemTime class.
//
//////////////////////////////////////////////////////////////////////
#include "stdcipc.h"

#include "SystemTime.h"
#include "CipcExtraMemory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSystemTime::CSystemTime()
{
	// Zeitrechnung beginnt am 1.1.1601
	LARGE_INTEGER li;
	li.QuadPart = 0;
	*this = li;
}

//////////////////////////////////////////////////////////////////////
CSystemTime::CSystemTime(DWORD dwHours)
{
	// Zeitrechnung beginnt am 1.1.1601
	LARGE_INTEGER li; // in 100 NanoSekunden
	li.QuadPart = (__int64)3600 * (__int64)10000000 * (__int64)dwHours;
	*this = li;
}
//////////////////////////////////////////////////////////////////////
CSystemTime::CSystemTime(const CSystemTime& s)
{
	wDay = s.wDay;
	wDayOfWeek = s.wDayOfWeek;
	wMonth = s.wMonth;
	wYear = s.wYear;
	wHour = s.wHour;
	wMinute = s.wMinute;
	wSecond = s.wSecond;
	wMilliseconds = s.wMilliseconds;
}										   

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: CSystemTime
 Zweck   : Konstruktor mit Argumenten

 Parameter:  
  wDa : (E): Tag  (WORD)
  wMo : (E): Monat  (WORD)
  wYe : (E): Jahr  (WORD)
  wHo : (E): Stunde  (WORD)
  wMi : (E): Minute  (WORD)
  wSe : (E): Sekunde  (WORD)
  wMS : (E): Millisekunde  (WORD)

 Rückgabewert:  ()
 <TITLE CSystemTime>
*********************************************************************************************/
CSystemTime::CSystemTime(WORD wDa, WORD wMo, WORD wYe, WORD wHo, WORD wMi, WORD wSe, WORD wMS /*=0*/)
{
	// CAVEAT!!!
	// Zeitrechnung beginnt am 1.1.1601
	// Dieser Konstruktor ist NICHT für Zeitspannen (h, min, sec) geeignet!
	wDay			= wDa;
	wDayOfWeek		= 0;
	wMonth			= wMo;
	wYear			= wYe;
	wHour			= wHo;
	wMinute			= wMi;
	wSecond			= wSe;
	wMilliseconds	= wMS;
	wDayOfWeek		= GetDayOfWeek();
}


//////////////////////////////////////////////////////////////////////
CSystemTime::~CSystemTime()
{
}

//////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: GetLocalTime
 Zweck   : Initialisiert die Lokale Zeit

 Parameter: Keine

 Rückgabewert:  (void)
 <TITLE GetLocalTime>
*********************************************************************************************/
void CSystemTime::GetLocalTime()
{
	::GetLocalTime(this);
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: IncrementTime
 Zweck   : Addiert eine Zeitspanne

 Parameter:  
  dwDays        : (E): Tage  (DWORD)
  dwHours       : (E): Stunden  (DWORD)
  dwMinutes     : (E): Minuten  (DWORD)
  dwSeconds     : (E): Sekunden  (DWORD)
  dwMilliSeconds: (E): Millisekunden  (DWORD)

 Rückgabewert:  (BOOL)
 <TITLE IncrementTime>
*********************************************************************************************/
BOOL CSystemTime::IncrementTime(DWORD dwDays, DWORD dwHours, DWORD dwMinutes,
								DWORD dwSeconds, DWORD dwMilliSeconds)
{
	// Läßt sich ggf. noch um MikroSekunden erweitern
	// Jahr und Monat sind keine festen mathematischen Größen
	BOOL bReturn = TRUE;
	LARGE_INTEGER liLeftOperand,liRightOperand,liResult;

	liLeftOperand = *this;
	liRightOperand.QuadPart  = 0; // in 100 NanoSekunden
//                                      Sekunden
	liRightOperand.QuadPart += (__int64)10000000 * (__int64)3600 * (__int64)24 * (__int64)dwDays;
	liRightOperand.QuadPart += (__int64)10000000 * (__int64)3600 * (__int64)dwHours;
	liRightOperand.QuadPart += (__int64)10000000 * (__int64)60 * (__int64)dwMinutes;
	liRightOperand.QuadPart += (__int64)10000000 * (__int64)dwSeconds;
//                              Millisekunden
	liRightOperand.QuadPart += (__int64)10000 * (__int64)dwMilliSeconds;
	liResult.QuadPart = liLeftOperand.QuadPart + liRightOperand.QuadPart;
	if (   (liRightOperand.QuadPart < 0)
		|| (   (liResult.QuadPart <= liLeftOperand.QuadPart)
			&& (liRightOperand.QuadPart > 0)
			)
		)
	{
		// Error, Überlauf
		bReturn = FALSE;
	}
	else
	{
		*this = liResult;
	}
	return bReturn;
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: DecrementTime
 Zweck   : Subtrahiert eine Zeitspanne

 Parameter:  
  dwDays        : (E): Tage  (DWORD)
  dwHours       : (E): Stunden  (DWORD)
  dwMinutes     : (E): Minuten  (DWORD)
  dwSeconds     : (E): Sekunden  (DWORD)
  dwMilliSeconds: (E): Millisekunden  (DWORD)

 Rückgabewert:  (BOOL)
 <TITLE DecrementTime>
*********************************************************************************************/
BOOL CSystemTime::DecrementTime(DWORD dwDays, DWORD dwHours, DWORD dwMinutes,
								DWORD dwSeconds, DWORD dwMilliSeconds)
{
	// Läßt sich ggf. noch um MikroSekunden erweitern
	// Jahr und Monat sind keine festen mathematischen Größen
	BOOL bReturn = TRUE;
	LARGE_INTEGER liLeftOperand,liRightOperand,liResult;

	liLeftOperand = *this;
	liRightOperand.QuadPart  = 0; // in 100 NanoSekunden
//                                      Sekunden
	liRightOperand.QuadPart += (__int64)10000000 * (__int64)3600 * (__int64)24 * (__int64)dwDays;
	liRightOperand.QuadPart += (__int64)10000000 * (__int64)3600 * (__int64)dwHours;
	liRightOperand.QuadPart += (__int64)10000000 * (__int64)60 * (__int64)dwMinutes;
	liRightOperand.QuadPart += (__int64)10000000 * (__int64)dwSeconds;
//                              Millisekunden
	liRightOperand.QuadPart += (__int64)10000 * (__int64)dwMilliSeconds;
	liResult.QuadPart = liLeftOperand.QuadPart - liRightOperand.QuadPart;
	if (   (liRightOperand.QuadPart < 0)
		|| (liRightOperand.QuadPart >= liLeftOperand.QuadPart)
		)
	{
		// Error, Überlauf
		bReturn = FALSE;
	}
	else
	{
		*this = liResult;
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////
static _TCHAR szDaysOfWeek[7][4] = {_T("Sun"),_T("Mon"),_T("Tue"),_T("Wed"),_T("Thu"),_T("Fri"),_T("Sat")};
static _TCHAR szTimeFormat[] = _T("%s,%02d.%02d.%04d %02d:%02d.%02d,%04d");
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: ToString
 Zweck   : Erzeugt einen Textstring mit der gespeicherten Zeit

 Parameter: Keine

 Rückgabewert:  (CString)
 <TITLE ToString>
*********************************************************************************************/
CString	CSystemTime::ToString()
{
	CString ret;
	CString sDayOfWeek;

	sDayOfWeek = szDaysOfWeek[wDayOfWeek];

	ret.Format(szTimeFormat,LPCTSTR(sDayOfWeek),
							wDay,
							wMonth,
							wYear,
							wHour,
							wMinute,
							wSecond,
							wMilliseconds);

	return ret;
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: FromString
 Zweck   : Initialisiert die Zeit aus einem Textstring

 Parameter:  
  s: (E): Zeit im Textformat  (const CString&)

 Rückgabewert:  (BOOL)
 <TITLE FromString>
*********************************************************************************************/
BOOL CSystemTime::FromString(const CString& s)
{
	CString sT;
	CString sOne;
	int p;
	BOOL bFound = FALSE;

	sT = s;
	// Day of week
	p = sT.Find(',');
	if (p==-1)
	{
		return FALSE;
	}
	sOne = sT.Left(p);
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	for (WORD i=0;i<7;i++)
	{
		if (0==sOne.CompareNoCase(szDaysOfWeek[i]))
		{
			wDayOfWeek = i;
			bFound = TRUE;
			break;
		}
	}
	if (!bFound)
	{
		return FALSE;
	}
	sT = sT.Mid(p+1);

	// day
	p = sT.Find('.');
	if (p==-1)
	{
		return FALSE;
	}
	sOne = sT.Left(p);
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	wDay = (WORD)_ttoi(LPCTSTR(sOne));
	sT = sT.Mid(p+1);

	// month
	p = sT.Find('.');
	if (p==-1)
	{
		return FALSE;
	}
	sOne = sT.Left(p);
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	wMonth = (WORD)_ttoi(LPCTSTR(sOne));
	sT = sT.Mid(p+1);

	// year
	p = sT.Find(' ');
	if (p==-1)
	{
		return FALSE;
	}
	sOne = sT.Left(p);
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	wYear = (WORD)_ttoi(LPCTSTR(sOne));
	sT = sT.Mid(p+1);

	// hour
	p = sT.Find(':');
	if (p==-1)
	{
		return FALSE;
	}
	sOne = sT.Left(p);
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	wHour = (WORD)_ttoi(LPCTSTR(sOne));
	sT = sT.Mid(p+1);

	// minute
	p = sT.Find('.');
	if (p==-1)
	{
		return FALSE;
	}
	sOne = sT.Left(p);
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	wMinute = (WORD)_ttoi(LPCTSTR(sOne));
	sT = sT.Mid(p+1);

	// second
	p = sT.Find(',');
	if (p==-1)
	{
		return FALSE;
	}
	sOne = sT.Left(p);
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	wSecond = (WORD)_ttoi(LPCTSTR(sOne));
	sT = sT.Mid(p+1);

	// millisecond
	sOne = sT;
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	wMilliseconds = (WORD)_ttoi(LPCTSTR(sOne));

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: LARGE_INTEGER
 Zweck   : Operator LARGE_INTEGER

 Parameter: Keine

 Rückgabewert: Zeit im Large Integer Format (LARGE_INTEGER)
 <TITLE LARGE_INTEGER>
*********************************************************************************************/
CSystemTime::operator LARGE_INTEGER() const
{
	LARGE_INTEGER liResult;
	FILETIME ftResult;

	ftResult.dwLowDateTime = 0; 
	ftResult.dwHighDateTime = 0; 
	liResult.LowPart = 0;
	liResult.HighPart = 0;

	if (!SystemTimeToFileTime(this,&ftResult))
	{
		WK_TRACE(_T("SystemTimeToFileTime failed, %s\n"), GetLastErrorString());
	}

	liResult.LowPart = ftResult.dwLowDateTime;
	liResult.HighPart = ftResult.dwHighDateTime;

	return liResult;
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: CSystemTime 
 Zweck   : Konstruktor mit Argument

 Parameter:  
  li: (E): Zeit im Large Integer format  (const LARGE_INTEGER&)

 Rückgabewert:  ()
 <TITLE CSystemTime >
*********************************************************************************************/
CSystemTime::CSystemTime (const LARGE_INTEGER& li)
{
	FILETIME ftResult;

	ftResult.dwHighDateTime = li.HighPart;
	ftResult.dwLowDateTime = li.LowPart;

	FileTimeToSystemTime(&ftResult,this);
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: operator + 
 Zweck   : Addiert zwei Zeiten (tErg = t1 + t2)

 Parameter:  
  timeSpan: (E): zweiter Operand  (const CSystemTime&)

 Rückgabewert: Summe (CSystemTime)
 <TITLE operator + >
*********************************************************************************************/
CSystemTime CSystemTime::operator + (const CSystemTime& timeSpan) const
{
	LARGE_INTEGER liLeftOperand,liRightOperand,liResult;

	liLeftOperand = *this;
	liRightOperand = timeSpan;

	liResult.QuadPart = liLeftOperand.QuadPart + liRightOperand.QuadPart;
	
	return CSystemTime(liResult);
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: operator -
 Zweck   : Subtrahiert zwei Zeiten (tErg = t1 - t2)

 Parameter:  
  timeSpan: (E): zweiter Operand  (const CSystemTime&)

 Rückgabewert: Summe (CSystemTime)
 <TITLE operator - >
*********************************************************************************************/
CSystemTime CSystemTime::operator - (const CSystemTime& timeSpan) const
{
	LARGE_INTEGER liLeftOperand,liRightOperand,liResult;

	liLeftOperand = *this;
	liRightOperand = timeSpan;

	liResult.QuadPart = liLeftOperand.QuadPart - liRightOperand.QuadPart;
	
	return CSystemTime(liResult);
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: operator < 
 Zweck   : Vergleicht zwei Zeiten: if (t1 < t2)

 Parameter:  
  timeSpan: (E): zweiter Operand  (const CSystemTime&)

 Rückgabewert: (TRUE, FALSE) (BOOL)
 <TITLE operator < >
*********************************************************************************************/
BOOL CSystemTime::operator < (const CSystemTime& s)
{
	if (wYear < s.wYear)
	{
		return TRUE;
	}
	else if (wYear == s.wYear)
	{
		if (wMonth < s.wMonth)
		{
			return TRUE;
		}
		else if (wMonth == s.wMonth)
		{
			if (wDay < s.wDay)
			{
				return TRUE;
			}
			else if (wDay == s.wDay)
			{
				if (wHour < s.wHour)
				{
					return TRUE;
				}
				else if (wHour == s.wHour)
				{
					if (wMinute < s.wMinute)
					{
						return TRUE;
					}
					else if (wMinute == s.wMinute)
					{
						if (wSecond < s.wSecond)
						{
							return TRUE;
						}
						else if (wSecond == s.wSecond)
						{
							if (wMilliseconds < s.wMilliseconds)
							{
								return TRUE;
							}
							else if (wMilliseconds == s.wMilliseconds)
							{
								return FALSE;
							}
							else
							{
								return FALSE;
							}
						}
						else
						{
							return FALSE;
						}
					}
					else
					{
						return FALSE;
					}
				}
				else
				{
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: operator <= 
 Zweck   : Vergleicht zwei Zeiten: if (t1 <= t2)

 Parameter:  
  timeSpan: (E): zweiter Operand  (const CSystemTime&)

 Rückgabewert: (TRUE, FALSE) (BOOL)
 <TITLE operator <= >
*********************************************************************************************/
BOOL CSystemTime::operator <= (const CSystemTime& s)
{
	if (*this < s)
	{
		return TRUE;
	}
	else if (*this == s)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: operator > 
 Zweck   : Vergleicht zwei Zeiten: if (t1 > t2)

 Parameter:  
  timeSpan: (E): zweiter Operand  (const CSystemTime&)

 Rückgabewert: (TRUE, FALSE) (BOOL)
 <TITLE operator > >
*********************************************************************************************/
BOOL CSystemTime::operator > (const CSystemTime& s)
{
	if (wYear > s.wYear)
	{
		return TRUE;
	}
	else if (wYear == s.wYear)
	{
		if (wMonth > s.wMonth)
		{
			return TRUE;
		}
		else if (wMonth == s.wMonth)
		{
			if (wDay > s.wDay)
			{
				return TRUE;
			}
			else if (wDay == s.wDay)
			{
				if (wHour > s.wHour)
				{
					return TRUE;
				}
				else if (wHour == s.wHour)
				{
					if (wMinute > s.wMinute)
					{
						return TRUE;
					}
					else if (wMinute == s.wMinute)
					{
						if (wSecond > s.wSecond)
						{
							return TRUE;
						}
						else if (wSecond == s.wSecond)
						{
							if (wMilliseconds > s.wMilliseconds)
							{
								return TRUE;
							}
							else if (wMilliseconds == s.wMilliseconds)
							{
								return FALSE;
							}
							else
							{
								return FALSE;
							}
						}
						else
						{
							return FALSE;
						}
					}
					else
					{
						return FALSE;
					}
				}
				else
				{
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: operator >= 
 Zweck   : Vergleicht zwei Zeiten: if (t1 >= t2)

 Parameter:  
  timeSpan: (E): zweiter Operand  (const CSystemTime&)

 Rückgabewert: (TRUE, FALSE) (BOOL)
 <TITLE operator >= >
*********************************************************************************************/
BOOL CSystemTime::operator >= (const CSystemTime& s)
{
	if (*this > s)
	{
		return TRUE;
	}
	else if (*this == s)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: operator == 
 Zweck   : Vergleicht zwei Zeiten: if (t1 == t2)

 Parameter:  
  timeSpan: (E): zweiter Operand  (const CSystemTime&)

 Rückgabewert: (TRUE, FALSE) (BOOL)
 <TITLE operator == >
*********************************************************************************************/
BOOL CSystemTime::operator == (const CSystemTime& s)
{
	return (wYear == s.wYear) &&
		   (wMonth == s.wMonth) &&
		   (wDay == s.wDay) &&
		   (wHour == s.wHour) &&
		   (wMinute == s.wMinute) &&
		   (wSecond == s.wSecond) &&
		   (wMilliseconds == s.wMilliseconds);
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: GetTime
 Zweck   : Gibt die Zeit in einen Textstring aus

 Parameter: Keine

 Rückgabewert: Zeit im Textformat (CString)
 <TITLE GetTime>
*********************************************************************************************/
CString CSystemTime::GetTime() const
{
	CString s;
	s.Format(_T("%02d:%02d:%02d"),wHour,wMinute,wSecond);
	return s;
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: GetDate
 Zweck   : Gibt das Datum in einen Textstring aus

 Parameter: Keine

 Rückgabewert: Datum im Textformat (CString)
 <TITLE GetDate>
*********************************************************************************************/
CString CSystemTime::GetDate() const
{
	CString s;
	s.Format(_T("%02d.%02d.%04d"),wDay,wMonth,wYear);
	return s;
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: GetLocalizedTime
 Zweck   : Gibt die Zeit in der länderspezifischen Formatierung in einem Textstring aus

 Parameter: Keine

 Rückgabewert: Zeit im Textformat (CString)
 <TITLE GetLocalizedTime>
*********************************************************************************************/
CString CSystemTime::GetLocalizedTime() const
{
	CString sLocalInfo;
	::GetLocaleInfo(LOCALE_USER_DEFAULT,
					LOCALE_STIMEFORMAT,
				    sLocalInfo.GetBufferSetLength(_MAX_PATH),
					_MAX_PATH);
	sLocalInfo.ReleaseBuffer();
	CString sTime;
	::GetTimeFormat(LOCALE_USER_DEFAULT,
					0,
					this,
					sLocalInfo,
					sTime.GetBufferSetLength(_MAX_PATH),
					_MAX_PATH);
	sTime.ReleaseBuffer();
	return sTime;
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: GetLocalizedDate
 Zweck   : Gibt das Datum in der länderspezifischen Formatierung in einem Textstring aus

 Parameter: Keine

 Rückgabewert: Datum im Textformat (CString)
 <TITLE GetLocalizedDate>
*********************************************************************************************/
CString CSystemTime::GetLocalizedDate(BOOL bShort/*=TRUE*/) const
{
	CString sLocalInfo;
	::GetLocaleInfo(LOCALE_USER_DEFAULT,
					bShort ? LOCALE_SSHORTDATE : LOCALE_SLONGDATE ,
				    sLocalInfo.GetBufferSetLength(_MAX_PATH),
					_MAX_PATH);
	sLocalInfo.ReleaseBuffer();
	CString sDate;
	::GetDateFormat(LOCALE_USER_DEFAULT,
					0,
					this,
					sLocalInfo,
					sDate.GetBufferSetLength(_MAX_PATH),
					_MAX_PATH);
	sDate.ReleaseBuffer();
	return sDate;
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: GetLocalizedDateTime
 Zweck   : Gibt die Zeit in der länderspezifischen Formatierung in einem Textstring aus

 Parameter: Keine

 Rückgabewert: Zeit im Textformat (CString)
 <TITLE GetLocalizedDateTime>
*********************************************************************************************/
CString CSystemTime::GetLocalizedDateTime(BOOL bShort/*=TRUE*/) const
{
	return GetLocalizedDate(bShort) + _T(" ") + GetLocalizedTime();
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: GetDBTime
 Zweck   : Liefert die Zeit im Datenbanktextformat

 Parameter: Keine

 Rückgabewert: Datenbanktextformat (CString)
 <TITLE GetDBTime>
*********************************************************************************************/
CString CSystemTime::GetDBTime() const
{
	CString s;
	s.Format(_T("%02d%02d%02d"),wHour,wMinute,wSecond);
	return s;
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: GetDBDate
 Zweck   : Liefert das Datum im Datenbanktextformat

 Parameter: Keine

 Rückgabewert: Datenbanktextformat (CString)
 <TITLE GetDBDate>
*********************************************************************************************/
CString CSystemTime::GetDBDate() const
{
	CString s;
	s.Format(_T("%04d%02d%02d"),wYear,wMonth,wDay);
	return s;
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: SetDBTime
 Zweck   : Initialsiert die Zeit aus dem Datenbanktextformat

 Parameter:  
  sDBTime: (E): Datenbanktextformat  (const CString&)

 Rückgabewert: Zeit Initalisiert (True, False) (BOOL)
 <TITLE SetDBTime>
*********************************************************************************************/
BOOL CSystemTime::SetDBTime(const CString& sDBTime)
{
	wMilliseconds = 0;
	unsigned int nHour, nMinute, nSecond;
	if (3 == _stscanf(sDBTime, _T("%02hu%02hu%02hu"),&nHour,&nMinute,&nSecond))
	{
		wHour   = (WORD) nHour;
		wMinute = (WORD) nMinute;
		wSecond = (WORD) nSecond;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: SetDBDate
 Zweck   : Initialsiert das Datum aus dem Datenbanktextformat

 Parameter:  
  sDBTime: (E): Datenbanktextformat  (const CString&)

 Rückgabewert: Zeit Initalisiert (True, False) (BOOL)
 <TITLE SetDBDate>
*********************************************************************************************/
BOOL CSystemTime::SetDBDate(const CString& sDBDate)
{
	wDayOfWeek = 0;
	unsigned int nYear, nMonth, nDay;
	if (3==_stscanf(sDBDate,_T("%04hu%02hu%02hu"),&nYear,&nMonth,&nDay))
	{
		wYear  = (WORD) nYear;
		wMonth = (WORD) nMonth;
		wDay   = (WORD) nDay;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: GetDateTime
 Zweck   : Gibt Datum und Zeit in einen Textstring aus

 Parameter: Keine

 Rückgabewert: Datum und Zeit im Textformat (CString)
 <TITLE GetDateTime>
*********************************************************************************************/
CString CSystemTime::GetDateTime() const
{
	return GetDate() + _T(" ") + GetTime();
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: GetCTime
 Zweck   : Initialsiert ein CTime-Objekt mit der gespeicherten Zeit

 Parameter: Keine

 Rückgabewert: gespeicherte Zeit (CTime)
 <TITLE GetCTime>
*********************************************************************************************/
CTime CSystemTime::GetCTime()
{
	return CTime(*this);
}

/*********************************************************************************************
 Klasse  : CSystemTime
 Funktion: FromTime
 Zweck   : Initialsiert ein die Zeit aus einem CTime-Objekt

 Parameter:  
  t: (E): CTime Objekt  (<EXTLINK CTime>CTime</EXTLINK>)

 Rückgabewert: Objekt wurde initialsisert (BOOL)
 <TITLE FromTime>
*********************************************************************************************/
BOOL CSystemTime::FromTime(CTime t)
{
	return t.GetAsSystemTime(*this);
}
/////////////////////////////////////////////////////////////////////////////
DWORD CSystemTime::GetBubbleLength()
{
	return 8*sizeof(WORD);
}
/////////////////////////////////////////////////////////////////////////////
void CSystemTime::WriteIntoBubble(BYTE*& pByte) const
{
	// 8 * WORD = 16 Bytes
	CIPCExtraMemory::WORD2Memory(pByte,wYear);			// 1
	CIPCExtraMemory::WORD2Memory(pByte,wMonth);			// 2
	CIPCExtraMemory::WORD2Memory(pByte,wDayOfWeek);		// 3
	CIPCExtraMemory::WORD2Memory(pByte,wDay);			// 4
	CIPCExtraMemory::WORD2Memory(pByte,wHour);			// 5
	CIPCExtraMemory::WORD2Memory(pByte,wMinute);		// 6
	CIPCExtraMemory::WORD2Memory(pByte,wSecond);		// 7
	CIPCExtraMemory::WORD2Memory(pByte,wMilliseconds);	// 8
}
/////////////////////////////////////////////////////////////////////////////
void CSystemTime::ReadFromBubble(const BYTE*& pByte)
{
	// 8 * WORD = 16 Bytes
	wYear = CIPCExtraMemory::Memory2WORD(pByte);		// 1
	wMonth = CIPCExtraMemory::Memory2WORD(pByte);		// 2
	wDayOfWeek = CIPCExtraMemory::Memory2WORD(pByte);	// 3
	wDay = CIPCExtraMemory::Memory2WORD(pByte);			// 4
	wHour = CIPCExtraMemory::Memory2WORD(pByte);		// 5
	wMinute = CIPCExtraMemory::Memory2WORD(pByte);		// 6
	wSecond = CIPCExtraMemory::Memory2WORD(pByte);		// 7
	wMilliseconds = CIPCExtraMemory::Memory2WORD(pByte);// 8
}
/////////////////////////////////////////////////////////////////////////////
CSystemTimeSpan::CSystemTimeSpan()
{
}
/////////////////////////////////////////////////////////////////////////////
CSystemTimeSpan::CSystemTimeSpan(const CSystemTime& s,const CSystemTime& e)
{
	SetTimes(s,e);
}
/////////////////////////////////////////////////////////////////////////////
CSystemTimeSpans::CSystemTimeSpans() 
{
	SetAutoDelete(TRUE);
};
/////////////////////////////////////////////////////////////////////////////
BOOL CSystemTimeSpans::IsIncluded(const CSystemTime& s)
{
	BOOL bIncluded = FALSE;

	Lock();

	for (int i=0;i<GetSize() && !bIncluded;i++)
	{
		bIncluded = GetAtFast(i)->IsIncluded(s);
	}

	Unlock();

	return bIncluded;
}
/////////////////////////////////////////////////////////////////////////////
void CSystemTimeSpans::Load(CWK_Profile& wkp, const CString& sSection)
{
	Lock();
	CSystemTime st,et;
	int c;
	CString s,v;
	c = wkp.GetInt(sSection,_T("Count"),0);
	for (int i=0;i<c;i++)
	{
		v = wkp.GetString(sSection,i,_T(""));
		if (!v.IsEmpty())
		{
			s = wkp.GetStringFromString(v,_T("SD"),_T(""));
			st.SetDBDate(s);
			s = wkp.GetStringFromString(v,_T("ST"),_T(""));
			st.SetDBTime(s);
			s = wkp.GetStringFromString(v,_T("ED"),_T(""));
			et.SetDBDate(s);
			s = wkp.GetStringFromString(v,_T("ET"),_T(""));
			et.SetDBTime(s);

			Add(new CSystemTimeSpan(st,et));
		}
	}
	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CSystemTimeSpans::Save(CWK_Profile& wkp, const CString& sSection)
{
	Lock();
	wkp.DeleteSection(sSection);
	wkp.WriteInt(sSection,_T("Count"),GetSize());
	CString sd,st,ed,et,v;
	for (int i=0;i<GetSize();i++)
	{
		sd = GetAtFast(i)->GetStartTime().GetDBDate();
		st = GetAtFast(i)->GetStartTime().GetDBTime();
		ed = GetAtFast(i)->GetEndTime().GetDBDate();
		et = GetAtFast(i)->GetEndTime().GetDBTime();
		v.Format(_T("\\SD%s\\ST%s\\ED%s\\ET%s"),sd,st,ed,et);
		wkp.WriteStringIndex(sSection,i,v);
	}
	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
CIPCExtraMemory* CSystemTimeSpans::BubbleFromSystemTimeSpans(CIPC *pCipc) const
{
	CSystemTimeSpans*pThis = (CSystemTimeSpans*) this;
	pThis->Lock();
	DWORD dwSum = 4;
	dwSum += 2*GetSize()*CSystemTime::GetBubbleLength();
	CIPCExtraMemory *pBubble = new CIPCExtraMemory();
	if (pBubble->Create(pCipc,dwSum))
	{
		BYTE *pByte = (BYTE*) pBubble->GetAddressForWrite();
		CIPCExtraMemory::DWORD2Memory(pByte,(DWORD)GetSize());

		for (int i=0;i<GetSize();i++) 
		{
			GetAtFast(i)->GetStartTime().WriteIntoBubble(pByte);
			GetAtFast(i)->GetEndTime().WriteIntoBubble(pByte);
		}

		DWORD dwDelta = pByte-(BYTE*)pBubble->GetAddressForWrite();
		WK_ASSERT(dwDelta==dwSum);
	}
	else
	{
		WK_DELETE(pBubble);
		WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in BubbleFromSystemTimeSpans\n"));
	}
	pThis->Unlock();
	return pBubble;
}
/////////////////////////////////////////////////////////////////////////////
CSystemTimeSpan* CSystemTimeSpans::SystemTimeSpansFromBubble(int iNumRecords,
															 const CIPCExtraMemory *pExtraMem)
{
	const BYTE *pByte = (const BYTE*)pExtraMem->GetAddress();
	DWORD dw;
	dw = CIPCExtraMemory::Memory2DWORD(pByte);
	WK_ASSERT(dw==(DWORD)iNumRecords);

	if (iNumRecords==0)
	{
		return NULL;
	}

	CSystemTimeSpan *pData = new CSystemTimeSpan[iNumRecords];
	for (int i=0;i<iNumRecords;i++)
	{
		pData[i].GetStartTime().ReadFromBubble(pByte);
		pData[i].GetEndTime().ReadFromBubble(pByte);
	}
	return pData;
}
/////////////////////////////////////////////////////////////////////////////
void CSystemTimeSpans::FromArray(int iNumRecords,const CSystemTimeSpan data[])
{
	Lock();
	DeleteAll();
	for (int i=0;i<iNumRecords;i++)
	{
		Add(new CSystemTimeSpan(data[i]));
	}
	Unlock();
}
