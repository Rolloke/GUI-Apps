/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SecTimeSpan.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/SecTimeSpan.cpp $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 22 $
// LAST CHANGE:	$Modtime: 19.01.06 16:10 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

/////////////////////////////////////////////////////////////////////////////
#include "stdcipc.h"
#include "SecTimeSpan.h"
#include "wkclasses\WK_Trace.h"	// for WK_TRACE

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
//	#define VIT_TEST_TRACE	TRACE
	#define VIT_TEST_TRACE
#else
	#define VIT_TEST_TRACE
#endif

/////////////////////////////////////////////////////////////////////////////
#define CTS_SECONDS(x) CTimeSpan(0,0,0,x)
static const TCHAR szFormat[] = _T("%a %d.%m.%Y %H:%M:%S");

/////////////////////////////////////////////////////////////////////////////
/*{CSecTimeSpan Overview|Overview,CSecTimeSpan}
 {members|CSecTimeSpan}, {CSecTimer}

CSecTimeSpan defines a time span, which is used for
processes in the server. The constructor takes
two strings as input. These strings allow time specifications like
"**:09:00" all days, or "MO:12:00" each monday.
*/
/* 
 {overview|Overview,CSecTimeSpan},
{CSecTimer}
*/
/////////////////////////////////////////////////////////////////////////////
/*Function: Constructs a CSecTimeSpan from two strings.
The format for the strings is:
 @CW{WD:hh:mm}  WeekDay, hour, minutes
2 @CW{WD} can be MO,DI,MI,DO,FR,SA,SO or ** (daily), MO_FR, SA_SU
2	DD:MM,YY:hh:mm, full date format @NORMAL
*/
CSecTimeSpan::CSecTimeSpan(const CTime &ct, const CString &sStartTime, 
							const CString &sEndTime)
{
	Init();
	// same times not valid except for 00:00-00:00
	if (sStartTime!=sEndTime || sStartTime.Find(_T("00:00"))!=-1) {
		m_sStartTime = sStartTime;
		m_sEndTime = sEndTime;
		m_bOkay = ConvertFromString(ct);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecTimeSpan::Init()
{
	m_enumType = WEEKDAY;
	m_bOkay=FALSE;
	// m_startTime;
	// m_endTime;
	// m_sStartTime;
	// m_sEndTime;
}
/////////////////////////////////////////////////////////////////////////////
void CSecTimeSpan::UpdateTimes(const CTime &ct)
{
	if (m_bOkay) {
		ConvertFromString(ct);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CSecTimeSpan::DayNrFromString(const CString &sDay)
{
	// numbers same as in CTime::GetDayOfWeek(), except 0 and >10
	int iDay=-1;
	// does german and english weekday abrevations
	if (sDay==_T("**")) iDay=0;
	else if (sDay.CompareNoCase(_T("SO"))==0 || sDay.CompareNoCase(_T("SU"))==0) iDay=1;	
	else if (sDay.CompareNoCase(_T("MO"))==0)                                iDay=2;
	else if (sDay.CompareNoCase(_T("DI"))==0 || sDay.CompareNoCase(_T("TU"))==0) iDay=3;
	else if (sDay.CompareNoCase(_T("MI"))==0 || sDay.CompareNoCase(_T("WE"))==0) iDay=4;
	else if (sDay.CompareNoCase(_T("DO"))==0 || sDay.CompareNoCase(_T("TH"))==0) iDay=5;
	else if (sDay.CompareNoCase(_T("FR"))==0)                                iDay=6;
	else if (sDay.CompareNoCase(_T("SA"))==0)                                iDay=7;
	else if (sDay.CompareNoCase(_T("MO_FR"))==0)                                   iDay=11;
	else if (sDay.CompareNoCase(_T("SA_SO"))==0 || sDay.CompareNoCase(_T("SA_SU"))==0) iDay=12;
	else {
		WK_TRACE_ERROR(_T("Wrong week day %s\n"),sDay);
	}
	return iDay;
}
/////////////////////////////////////////////////////////////////////////////
// at this point Load has already filled the time strings 01:00-02:00
BOOL CSecTimeSpan::ConvertFromString(const CTime &ct)
{
//	VIT_TEST_TRACE(_T("TEST\nactualtime %ld %s\n"), ct.GetTime(), ct.Format(szFormat));
	BOOL bOkay=FALSE;

	// first count the delimiters and check for format type and matching formats
	// FORMAT1 week day WD:hh:mm,
	//			WD=MO,DI,MI,DO.FR,SA,SO or ** (daily), MO_FR, SA_SU
	// FORMAT2 date		DD.MM.YYYY:hh:mm
	//		YYYYYY=19XX-20XX or ****
	int iNumDotsStart=0;
	int iNumDotsEnd=0;
	int iNumColonsStart=0;
	int iNumColonsEnd=0;
	LPCTSTR szStartTime = m_sStartTime;
	while (szStartTime && *szStartTime) 
	{
		if (*szStartTime=='.')
		{
			iNumDotsStart++;
		}
		else if (*szStartTime==':') 
		{
			iNumColonsStart++;
		}
		szStartTime++;
	}
	LPCTSTR szEndTime = m_sEndTime;
	while (szEndTime && *szEndTime) 
	{
		if (*szEndTime=='.') 
		{
			iNumDotsEnd++;
		} 
		else if (*szEndTime==':')
		{
			iNumColonsEnd++;
		}
		szEndTime++;
	}
	if (   iNumDotsStart != iNumDotsEnd
		|| iNumColonsStart != iNumColonsEnd)
	{
		WK_TRACE_ERROR(_T("CSecTimeSpan mismatching time formats '%s' '%s'\n"),
						LPCTSTR(m_sStartTime), LPCTSTR(m_sEndTime));
	}
	else
	{
		if (iNumDotsStart == 0 && iNumColonsStart == 2)	// FORMAT1, week day or any day
		{
			bOkay = ConvertFormat1(ct);
		}
		else if (iNumDotsStart == 2 && iNumColonsStart == 1)	// FORMAT2 date
		{
			if (ConvertFormat2(ct, m_sStartTime, m_startTime, FALSE))
			{
				if (ConvertFormat2(ct, m_sEndTime, m_endTime, TRUE))
				{
					if (m_startTime <= m_endTime)
					{
						bOkay = TRUE;
					}
					else
					{
						WK_TRACE_ERROR(_T("CSecTimeSpan wrong time format (relation) '%s' '%s'\n"),
										LPCTSTR(m_sStartTime), LPCTSTR(m_sEndTime));
					}
				}
			}
		}
		else
		{
			// num delimiters does not match any valid format
			WK_TRACE_ERROR(_T("CSecTimeSpan missing or mismatching delimiters '%s' '%s'\n"),
									LPCTSTR(m_sStartTime), LPCTSTR(m_sEndTime));
		}
	}
	return bOkay;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecTimeSpan::ConvertFormat1(const CTime &ct)
{
	// FORMAT1 week day WD:hh:mm,
	//			WD=MO,DI,MI,DO.FR,SA,SO or ** (daily), MO_FR, SA_SU

	BOOL bOkay=FALSE;
	// scan the format
	int iHoursStart=0, iMinStart=0, iHoursEnd=0, iMinEnd=0;
	int iNumStart= _stscanf(m_sStartTime.Right(5),_T("%d:%d"),&iHoursStart,&iMinStart);
	int iNumEnd	 = _stscanf(m_sEndTime.Right(5),_T("%d:%d"),&iHoursEnd,&iMinEnd);
	if (iNumStart == 2 && iNumEnd == 2)
	{
		// do some pre checks
		if (iHoursStart>23 || iHoursStart<0 || iHoursEnd>23 || iHoursEnd<0)
		{
			WK_TRACE_ERROR(_T("CSecTimeSpan Invalid hour '%s' '%s'\n"),
								LPCTSTR(m_sStartTime), LPCTSTR(m_sEndTime));
		}
		else if (iMinStart >60 || iMinStart<0 || iMinEnd >60 || iMinEnd<0)
		{
			WK_TRACE_ERROR(_T("CSecTimeSpan Invalid minute '%s' '%s'\n"),
								LPCTSTR(m_sStartTime), LPCTSTR(m_sEndTime));
		}
		else
		{
			CString sDayStart, sDayEnd;
			int iIndex = -1;
			iIndex = m_sStartTime.Find(':');
			if (iIndex != -1)
			{
				sDayStart = m_sStartTime.Left(iIndex); 
			}
			iIndex = m_sEndTime.Find(':');
			if (iIndex != -1)
			{
				sDayEnd = m_sEndTime.Left(iIndex); 
			}
			int iDayNrStart	= DayNrFromString(sDayStart);
			int iDayNrEnd	= DayNrFromString(sDayEnd);
			if (iDayNrStart != -1 && iDayNrEnd != -1)		// any day, daily
			{
				// init the timespan with hours and minutes of today
				m_startTime	= CTime(ct.GetYear(), ct.GetMonth(), ct.GetDay(), iHoursStart, iMinStart, 0);
				m_endTime	= CTime(ct.GetYear(), ct.GetMonth(), ct.GetDay(), iHoursEnd, iMinEnd, 0);
				// "00:00" as end time is midnight
				if (iHoursEnd==0 && iMinEnd==0)
				{
					m_endTime += CTimeSpan(1,0,0,0);
				}
				// adjust end times 18:00 --> 17:59.59
				m_endTime -= CTS_SECONDS(1);
/*				VIT_TEST_TRACE(_T("TEST initial time\nstart time %ld %s\nend   time %ld %s\n"),
						m_startTime.GetTime(), m_startTime.Format(szFormat),
						m_endTime.GetTime(), m_endTime.Format(szFormat));
*/
				// Days have to be adjusted
				// Daily, every day
				if (iDayNrStart == 0 && iDayNrEnd == 0)
				{
					m_enumType = DAILY;
					bOkay = TRUE;
				}
				// known multiple days
				else if  (   (iDayNrStart == iDayNrEnd)
						  && (iDayNrStart > 10)
						 )
				{
					if (iDayNrStart == 11)	// Monday til Friday
					{
						m_enumType = MO_FR;
					}
					else if (iDayNrStart == 12)	// Saturday and Sunday
					{
						m_enumType = SA_SU;
					}
					bOkay = TRUE;
				}
				// specific weekday
				else if (   (0 < iDayNrStart && iDayNrStart < 8)
						 && (0 < iDayNrEnd && iDayNrEnd < 8)
						)
				{
					m_enumType = WEEKDAY;
					bOkay = TRUE;
				}
				if (bOkay)
				{
					switch (m_enumType)
					{
						case DAILY:
							AdjustDailyTimer(ct);
							break;
						case MO_FR:
							AdjustMoFrTimer(ct);
							break;
						case SA_SU:
							AdjustSaSuTimer(ct);
							break;
						case WEEKDAY:
						default:
							AdjustWeekdayTimer(ct, iDayNrStart, iDayNrEnd);
							break;
					}
				}
			}
			if (bOkay)
			{
				VIT_TEST_TRACE(_T("CSecTimeSpan::ConvertFormat1 %s %s %s\n"), m_sStartTime, m_sEndTime, NameOfEnumType());
				VIT_TEST_TRACE(_T("     %s => %s\n"),
									m_startTime.Format(_T("%a %d.%m.%Y %H:%M:%S")),
									m_endTime.Format(_T("%a %d.%m.%Y %H:%M:%S"))
							  );
			}
			else
			{
				WK_TRACE_ERROR(_T("CSecTimeSpan wrong time format (day): '%s' '%s'\n"),
										LPCTSTR(m_sStartTime), LPCTSTR(m_sEndTime));
			}
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("CSecTimeSpan wrong time format (field error): '%s' '%s'\n"),
									LPCTSTR(m_sStartTime), LPCTSTR(m_sEndTime));
	}
	return bOkay;
}
/////////////////////////////////////////////////////////////////////////////
void CSecTimeSpan::AdjustDailyTimer(const CTime &ct)
{
	// if the endtime is already reached, the timer is valid for the next day
	if (ct > m_endTime)
	{
		m_startTime	+= CTimeSpan(1,0,0,0);
		m_endTime	+= CTimeSpan(1,0,0,0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecTimeSpan::AdjustMoFrTimer(const CTime &ct)
{
	int iDayNrCurrent = ct.GetDayOfWeek();
	// if current day is Mon til Thu, it can be handled as daily timer
	if (   1 < iDayNrCurrent
		&&     iDayNrCurrent < 6)
	{
		AdjustDailyTimer(ct);
	}
	// else
	else
	{
		int iDelta = 0;
		// if current day is Fr
		if (iDayNrCurrent == 6)
		{
			// The timer may be still valid today (Friday)
			// only if endtime is already reached the timer is valid for Monday
			if (ct > m_endTime)
			{
				iDelta = 3;
			}
		}
		// else if current day is Sa
		else if (iDayNrCurrent == 7)
		{
			iDelta = 2;
		}
		// else if current day is Su
		else if (iDayNrCurrent == 1)
		{
			iDelta = 1;
		}
		if (iDelta > 0)
		{
			m_startTime	+= CTimeSpan(iDelta,0,0,0);
			m_endTime	+= CTimeSpan(iDelta,0,0,0);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecTimeSpan::AdjustSaSuTimer(const CTime &ct)
{
	int iDayNrCurrent = ct.GetDayOfWeek();
	// if current day is Saturday, it can be handled as daily timer
	if (iDayNrCurrent == 7)
	{
		AdjustDailyTimer(ct);
	}
	// else
	else
	{
		int iDelta = 0;
		// if current day is Monday til Friday, the timer is valid at next Saturday
		if (   1 < iDayNrCurrent
			&&     iDayNrCurrent < 7)
		{
			iDelta = 7 - iDayNrCurrent;
		}
		// if current day is Su
		else if (iDayNrCurrent == 1)
		{
			// The timer may be still valid today (Sunday)
			// only if endtime is already reached the timer is valid for Saturday
			if (ct > m_endTime)
			{
				iDelta = 6;
			}
		}
		m_startTime	+= CTimeSpan(iDelta,0,0,0);
		m_endTime	+= CTimeSpan(iDelta,0,0,0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecTimeSpan::AdjustWeekdayTimer(const CTime &ct, int iDayNrStart, int iDayNrEnd)
{
	// Calculate the start and end delta
	int iDeltaStart	= 0; // CAVEAT can be negativ!
	int iDeltaEnd	= 0;

	// adjust day if not current day
	int iDayNrCurrent = ct.GetDayOfWeek();
	int iDayNrEndAdjusted	= iDayNrEnd;
	// a) if start day is greater than end day, the timespan goes beyond the weekend
	// b) if start day is same as end day, but start time greater than end time
	//    the timespan is greater than a week
	// then the end day has virtually add 7 days to keep the relation
	if (   (iDayNrStart > iDayNrEnd)
		|| (   (iDayNrStart == iDayNrEnd)
			&& (m_startTime > m_endTime)
			)
		)
	{
		iDayNrEndAdjusted += 7;
	}

	// a) if the actual day is in-between the days span
	//    will also work for the actual day itself with diffs = 0
	if (   (iDayNrStart <= iDayNrCurrent)
		&& (iDayNrCurrent <= iDayNrEndAdjusted)
		)
	{
		// iDeltaStart will be negative, so the difference will be subtracted
		iDeltaStart = iDayNrStart - iDayNrCurrent;
		iDeltaEnd	= iDayNrEndAdjusted - iDayNrCurrent;
	}

	// b) if the adjusted actual day (+7) is in-between the days span
	else if (   (iDayNrStart <= (iDayNrCurrent+7))
			 && ((iDayNrCurrent+7) <= iDayNrEndAdjusted)
			)
	{
		// iDeltaStart will be negative, so the difference will be substracted
		iDeltaStart = iDayNrStart - (iDayNrCurrent+7);
		iDeltaEnd	= iDayNrEndAdjusted - (iDayNrCurrent+7);
	}
	// c) actual day is outside the time span, so we have to await it in future
	else
	{
		// c1) if the actual day is smaller than the start day
		if (iDayNrStart > iDayNrCurrent)
		{
			iDeltaStart = iDayNrStart - iDayNrCurrent;
			iDeltaEnd	= iDayNrEndAdjusted - iDayNrCurrent;
		}
		// c2) else the actual day is greater than the end day
		else
		{
			iDeltaStart = iDayNrStart + 7 - iDayNrCurrent;
			iDeltaEnd	= iDayNrEndAdjusted + 7 - iDayNrCurrent;
		}
	}

	if (iDeltaStart < 0)
	{
		m_startTime	-= CTimeSpan(abs(iDeltaStart),0,0,0);
	}
	else
	{
		m_startTime	+= CTimeSpan(iDeltaStart,0,0,0);
	}
	m_endTime	+= CTimeSpan(iDeltaEnd,0,0,0);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecTimeSpan::ConvertFormat2(const CTime &ct, const CString &sTime,
									CTime &result, BOOL bAsEndTime)
{
	// FORMAT2 date		DD.MM.YYYY:hh:mm
	//		YYYYYY=19XX-20XX or ****
	// read year as chars first might be ****

	m_enumType = CALENDER_DAY;
	BOOL bOkay=FALSE;
	int iDay=0;
	int iMonth=0;
	int iYear1=0, iYear2=0, iYear3=0, iYear4=0;
	int iHours=0;
	int iMinutes=0;

	// scan the format
	int iNum = _stscanf(sTime, _T("%d.%d.%c%c%c%c,%d:%d"),
						&iDay, &iMonth,
						&iYear1, &iYear2,  &iYear3, &iYear4,
						&iHours, &iMinutes
						);
	if (iNum == 8)
	{
		char chYear1=(char)iYear1, chYear2=(char)iYear2, chYear3=(char)iYear3, chYear4=(char)iYear4;
		// do some pre checks
		if (iMonth>12 || iMonth<1)
		{
			WK_TRACE_ERROR(_T("CSecTimeSpan Invalid month '%s'\n"),LPCTSTR(sTime));
		}
		else if (iDay>31 || iDay<1)
		{
			WK_TRACE_ERROR(_T("CSecTimeSpan Invalid days'\n"),LPCTSTR(sTime));
		}
		
		else if (iHours>23 || iHours<0)
		{
			WK_TRACE_ERROR(_T("CSecTimeSpan Invalid hours'\n"),LPCTSTR(sTime));
		}
		else if (iMinutes>60 || iMinutes<0)
		{
			WK_TRACE_ERROR(_T("CSecTimeSpan Invalid minute\n"),LPCTSTR(sTime));
		}
		else
		{
			int iYear=0;
			if (	chYear1=='*'
				&&	chYear2=='*'
				&&	chYear3=='*'
				&&	chYear4=='*'
				)
			{
				m_enumType = DAY_EVERY_YEAR;
				// calc the 'next' year depending from the given date
				BOOL bUseNextYear=FALSE;
				if (iMonth < ct.GetMonth())
				{
					// a 'preceding' month has to be in the next year
					bUseNextYear = TRUE;
				}
				else if (iMonth == ct.GetMonth())
				{
					// the hard case, the same month, check the day
					if (iDay < ct.GetDay())
					{
						bUseNextYear = TRUE;
					}
					else if (iDay == ct.GetDay())
					{
						// OOPS, also check the time ? NOT YET
						bUseNextYear = FALSE;
					}
					else if (iDay > ct.GetDay())
					{
						bUseNextYear = FALSE;
					}
					else
					{
						WK_TRACE_ERROR(_T("CSecTimeSpan internal error in next day calculation for '%s'\n"),
													LPCTSTR(sTime));
					}
				}
				else if (iMonth > ct.GetMonth())
				{
					// a following month has to be within the current year
					bUseNextYear=FALSE;
				}
				else
				{
					WK_TRACE_ERROR(_T("CSecTimeSpan internal error in next year calculation for '%s'\n"),
													LPCTSTR(sTime));
				}

				iYear = ct.GetYear();
				if (bUseNextYear)
				{
					iYear++;
				}
			}
			else
			{	// specific year
				CString sYear;
				sYear += chYear1;
				sYear += chYear2;
				sYear += chYear3;
				sYear += chYear4;
				
				iYear = _ttoi(sYear);
			}

			// OOPS what is the lower boundary   
			if (iYear < 1972 || iYear > 2037)
			{
				WK_TRACE_ERROR(_T("CSecTimeSpan Invalid year %d in '%s'\n"),
												iYear, LPCTSTR(sTime));
			}
			else
			{
				// now iYear is read or calculated for the ****
				result = CTime(iYear, iMonth, iDay, iHours, iMinutes, 0);
				// "00-00"as end time is midnight
				if (bAsEndTime && iHours==0 && iMinutes==0)
				{
					result += CTimeSpan(1,0,0,0);
				}
				// adjust end times 18:00 --> 17:59.59
				if (bAsEndTime)
				{
					result -= CTS_SECONDS(1);
				}
				bOkay = TRUE;
			}
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("CSecTimeSpan wrong date format, field error in '%s'\n"),
														LPCTSTR(sTime));
	}
	return bOkay;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecTimeSpan::IsIncluded(const CTime& time) const
{
	BOOL bReturn = FALSE;
	bReturn = (   m_startTime <= time
			   &&                time <=m_endTime
			  );

	// Is checked every second, but normally only the minute change is relevant
	if (   (time.GetSecond() == 59)
		|| (time.GetSecond() == 0)
		)
	{
		VIT_TEST_TRACE(_T("CSecTimeSpan::IsIncluded %s %s %s\n"), m_sStartTime, m_sEndTime, NameOfEnumType());
		VIT_TEST_TRACE(_T("     %s <= %s <= %s ? %s\n"),
							m_startTime.Format(_T("%a %d.%m.%Y %H:%M:%S")),
							time.Format(_T("%a %d.%m.%Y %H:%M:%S")),
							m_endTime.Format(_T("%a %d.%m.%Y %H:%M:%S")),
							(bReturn ? "Yes" : "No")
					  );
	}

/*	BOOL bReturn = FALSE;
	if (   m_enumType == WEEKDAY
		|| m_enumType == DAILY
		|| m_enumType == CALENDER_DAY
		)
	{
		bReturn = (   m_startTime <= time
				   && time <=m_endTime
				  );
		TRACE(_T("%s <= %s <= %s ? %s\n"),
			m_startTime.Format(_T("%a %d.%m.%Y %H:%M:%S")),
			time.Format(_T("%a %d.%m.%Y %H:%M:%S")),
			m_endTime.Format(_T("%a %d.%m.%Y %H:%M:%S")),
			(bReturn ? _T("Yes") : _T("No")));
	}
	else if (m_enumType == MO_FR)
	{
		int iDayOfWeek = time.GetDayOfWeek();
		if (   1 < iDayOfWeek
			&&     iDayOfWeek < 7
			)
		{
			// check only time indepent from day
			CTime ctStart(time.GetYear(), time.GetMonth(), time.GetDay(),
						  m_startTime.GetHour(), m_startTime.GetMinute(), m_startTime.GetSecond());
			CTime ctEnd(time.GetYear(), time.GetMonth(), time.GetDay(),
					    m_endTime.GetHour(), m_endTime.GetMinute(), m_endTime.GetSecond());
			bReturn = ((ctStart <= time) && (time <= ctEnd));
		}
		TRACE(_T("%s <= %s <= %s ? %s\n"),
			m_startTime.Format(_T("MO_FR %H:%M:%S")),
			time.Format(_T("%a %H:%M:%S")),
			m_endTime.Format(_T("MO_FR %H:%M:%S")),
			(bReturn ? _T("Yes") : _T("No")));
	}
	else if (m_enumType == SA_SU)
	{
		int iDayOfWeek = time.GetDayOfWeek();
		if (   iDayOfWeek == 7
			|| iDayOfWeek == 1
			)
		{
			// check only time indepent from day
			CTime ctStart(time.GetYear(), time.GetMonth(), time.GetDay(),
						  m_startTime.GetHour(), m_startTime.GetMinute(), m_startTime.GetSecond());
			CTime ctEnd(time.GetYear(), time.GetMonth(), time.GetDay(),
					    m_endTime.GetHour(), m_endTime.GetMinute(), m_endTime.GetSecond());
			bReturn = ((ctStart <= time) && (time <= ctEnd));
		}
		TRACE(_T("%s <= %s <= %s ? %s\n"),
			m_startTime.Format(_T("SA_SU %H:%M:%S")),
			time.Format(_T("%a %H:%M:%S")),
			m_endTime.Format(_T("SA_SU %H:%M:%S")),
			(bReturn ? _T("Yes") : _T("No")));
	}
*/
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
CTime CSecTimeSpan::GetNextModeChange(const CTime &ct)
{
	#define FAR_FUTURE CTime(2038,1,1,1,1,1)
	CTime time = FAR_FUTURE;
	CString sDebug = _T("");

	// CAVEAT: First check, if endtime has already been reached
	// then the timer has to be updated before!
	if (m_endTime < ct)
	{
		int iDelta = 0;
		switch (m_enumType)
		{
			case WEEKDAY:
				iDelta = 7;
				break;
			case DAILY:
				iDelta = 1;
				break;
			case MO_FR:
				AdjustMoFrTimer(ct);
				break;
			case SA_SU:
				AdjustSaSuTimer(ct);
				break;
			case DAY_EVERY_YEAR:
				// Adjust year, not yet implemented
				break;
			case CALENDER_DAY:
			default:
				// do nothing it is a use-and-throw-away timer
				break;
		}
		if (iDelta > 0)
		{
			m_startTime	+= CTimeSpan(iDelta,0,0,0);
			m_endTime	+= CTimeSpan(iDelta,0,0,0);
		}
	}
	// now one new mode change should be found
	if (ct <= m_startTime)
	{
		time = m_startTime;
	}
	else if (ct <= m_endTime)
	{
		// end event after the span
		time = m_endTime + CTimeSpan(0,0,0,1);
	}
	else
	{
		WK_TRACE(_T("CSecTimeSpan::GetNextModeChange NOT FOUND %s %s %s\n"), m_sStartTime, m_sEndTime, NameOfEnumType());
		WK_TRACE(_T("     ct:%s ; next:%s ; start:%s ; end:%s\n"),
						ct.Format(_T("%a %d.%m.%Y %H:%M:%S")),
						time.Format(_T("%a %d.%m.%Y %H:%M:%S")),
						m_startTime.Format(_T("%a %d.%m.%Y %H:%M:%S")),
						m_endTime.Format(_T("%a %d.%m.%Y %H:%M:%S"))
					  );
		sDebug = _T("CSecTimeSpan::GetNextModeChange NOT FOUND");
	}

	VIT_TEST_TRACE(_T("CSecTimeSpan::GetNextModeChange found %s %s %s\n"), m_sStartTime, m_sEndTime, NameOfEnumType());
	VIT_TEST_TRACE(_T("     ct:%s ; next:%s ; start:%s ; end:%s\n"),
							ct.Format(_T("%a %d.%m.%Y %H:%M:%S")),
							time.Format(_T("%a %d.%m.%Y %H:%M:%S")),
							m_startTime.Format(_T("%a %d.%m.%Y %H:%M:%S")),
							m_endTime.Format(_T("%a %d.%m.%Y %H:%M:%S"))
				  );
	
	return time;

}
/////////////////////////////////////////////////////////////////////////////
LPCTSTR CSecTimeSpan::NameOfEnumType() const
{
	switch (m_enumType) 
	{
		NAME_OF_ENUM(WEEKDAY);
		NAME_OF_ENUM(DAILY);
		NAME_OF_ENUM(MO_FR);
		NAME_OF_ENUM(SA_SU);
		NAME_OF_ENUM(CALENDER_DAY);
		NAME_OF_ENUM(DAY_EVERY_YEAR);
		default : 
			return _T("Unknown Timer type");
	}
}
