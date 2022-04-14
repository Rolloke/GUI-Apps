/* GlobalReplace: WeekDayForEndTime --> DynamicPatternForEndTime */
/* GlobalReplace: m_bHasDynamicPatternForEndTime --> m_bHasDynamicEndTime */
/* GlobalReplace: HasWeekDayForStartTime --> HasDynamicStartTime */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SecTimeSpan.h $
// ARCHIVE:		$Archive: /Project/CIPC/SecTimeSpan.h $
// CHECKIN:		$Date: 26.09.03 15:51 $
// VERSION:		$Revision: 17 $
// LAST CHANGE:	$Modtime: 26.09.03 14:51 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef _SecTimeSpan_H
#define _SecTimeSpan_H

#include "wk.h"
#include "SecID.h"

//

// FORMAT1 week day WD:hh:mm, 
//			WD=MO,DI,MI,DO.FR,SA,SO or ** (daily), MO_FR, SA_SU
// FORMAT2 date		DD:MM:YY:hh:mm
//		YY=19XX-20XX or ****

// Defines a span of time, given by start and end time.
// Start and end can contain dynamic 'patterns', like weekdays or any year.
class AFX_EXT_CLASS CSecTimeSpan 
{
	enum EnumType
	{
		WEEKDAY,
		DAILY,
		MO_FR,
		SA_SU,
		CALENDER_DAY,
		DAY_EVERY_YEAR
	};

public:
	//  public
	//!ic! default constructor
	CSecTimeSpan(const CTime& ct, const CString& sStartTime, const CString& sEndTime);

	//  access fns.
	BOOL IsIncluded(const CTime& time) const;
	//!ic! nomen est omen.
	inline BOOL IsValid() const;	// constructor okay ?
	//!ic! current start time as CTime
	inline CTime GetStartTime() const;
	//!ic! current end time as CTime.
	inline CTime GetEndTime() const;
	//!ic! start time as string, including dynamic patterns
	inline const CString& GetStartString() const;
	//!ic! end timee as string, including dynamic patterns
	inline const CString& GetEndString() const;

	CTime GetNextModeChange(const CTime& ct);
	
	//  utility fns.
	//!ic! updates the internal times, depending from the strings
	void UpdateTimes(const CTime& ct);

	//  static utility fns
	//!ic! abrevation to weekDayNr
	static int DayNrFromString(const CString& sDay);

private:
	//  private
	//!ic! convert the member CStrings to member CTimes
	BOOL ConvertFromString(const CTime& ct);
	BOOL ConvertFormat1(const CTime& ct);
	void AdjustDailyTimer(const CTime& ct);
	void AdjustMoFrTimer(const CTime& ct);
	void AdjustSaSuTimer(const CTime& ct);
	void AdjustWeekdayTimer(const CTime& ct, int iDayNrStart, int iDayNrEnd);
	BOOL ConvertFormat2(const CTime &ct, const CString &sTime, CTime &result, BOOL bAsEndTime);
	LPCTSTR NameOfEnumType() const;

	// data
private:
	void Init();
	//

	EnumType	m_enumType;
	CTime		m_startTime;
	CTime		m_endTime;
	CString		m_sStartTime;
	CString		m_sEndTime;
	BOOL		m_bOkay;
};

typedef CSecTimeSpan * CSecTimeSpanPtr;
WK_PTR_ARRAY(CSecTimeSpanArray, CSecTimeSpanPtr)

///////////////////////
// inlined functions //
///////////////////////

/*Function: constructor succesfull ? */
inline BOOL CSecTimeSpan::IsValid() const
{
	return m_bOkay;
}
// Function: NYD
inline CTime CSecTimeSpan::GetStartTime() const
{
	return m_startTime;
}
// Function: NYD
inline CTime CSecTimeSpan::GetEndTime() const
{
	return m_endTime;
}

/*Function: the string of the start time, as given in the constructor */
inline const CString& CSecTimeSpan::GetStartString() const
{
	return m_sStartTime;
}
/*Function: the string of the end time, as given in the constructor */
inline const CString& CSecTimeSpan::GetEndString() const
{
	return m_sEndTime;
}

#endif
