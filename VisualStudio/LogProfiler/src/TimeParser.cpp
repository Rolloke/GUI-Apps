/*
 * TimeParser.cpp
 *
 *  Created on: Dec 19, 2017
 *      Author: developer
 */

#include "TimeParser.h"
#include "logger.h"
#include <iostream>
#include <sstream>


const int TimeParser::mMillitoMicroseconds = 1000;
str_str_map  TimeParser::mRegExMap;
str_int_map  TimeParser::mMonthMap;
str_int_map  TimeParser::mDayOfWeekMap;

TimeParser::TimeParser()
    : mYear(0)
    , mMonth(0)
    , mDayOfMonth(0)
    , mWeekOfYear(0)
    , mDayOfWeek(0)
    , mDayOfYear(0)
    , mHour(0)
    , mHourTimzoneOffset(0)
    , mPM(false)
    , mMinute(0)
    , mSecond(0)
    , mMicroseconds(0)
    , mLeapYearDay(0)
{
    if (mRegExMap.empty())
    {
        mRegExMap[" "] = "\\s+";        // One or more spaces
        mRegExMap["["] = "\\[";         // square bracket
        mRegExMap["]"] = "\\]";         // square bracket
        mRegExMap["("] = "\\(";         // round bracket
        mRegExMap[")"] = "\\)";         // round bracket
        mRegExMap["{"] = "\\{";         // currled bracket
        mRegExMap["}"] = "\\}";         // currled bracket
        mRegExMap["."] = "\\.";         // dot
        mRegExMap["+"] = "\\+";         // plus
        mRegExMap["*"] = "\\*";         // star

        mRegExMap["%n"] = "\\n";        // New-line character ('\n')
        mRegExMap["%t"] = "\\t";        // Horizontal-tab character ('\t')

        mRegExMap["%a"] = "(\\w{3})";   // Abbreviated weekday name * Thu
        mRegExMap["%A"] = "(\\w+)";     // Full weekday name *  Thursday
        mRegExMap["%b"] = "(\\w{3})";   // Abbreviated month name * Aug
        mRegExMap["%B"] = "(\\w+)";     // Full month name * August
        mRegExMap["%C"] = "(\\d{2})";   // Year divided by 100 and truncated to integer (00-99) 20
        mRegExMap["%d"] = "(\\d{2})";   // Day of the month, zero-padded (01-31) 23
        mRegExMap["%e"] = "(\\d{1,2})"; // Day of the month, space-padded ( 1-31) 23
        mRegExMap["%H"] = "(\\d{1,2})"; // Hour in 24h format (00-23)
        mRegExMap["%I"] = "(\\d{1,2})"; // Hour in 12h format (01-12)
        mRegExMap["%j"] = "(\\d{1,2})"; // Day of the year (001-366)
        mRegExMap["%l"] = "(\\d{8,10})";// Linux/Unix time: Seconds since 1970-01-01 00:00:00 +0000 (UTC).
        mRegExMap["%m"] = "(\\d{1,2})"; // Month as a decimal number (01-12)
        mRegExMap["%M"] = "(\\d{1,2})"; // Minute (00-59)
        mRegExMap["%p"] = "(\\w{2})";   // AM or PM designation PM
        mRegExMap["%S"] = "(\\d{1,2})"; // Second (00-61)
        mRegExMap["%u"] = "(\\d{1})";   // ISO 8601 weekday as number with Monday as 1 (1-7)
        mRegExMap["%U"] = "(\\d{1,2})"; // Week number with the first Sunday as the first day of week one (00-53)
        mRegExMap["%V"] = "(\\d{1,2})"; // ISO 8601 week number (01-54)
        mRegExMap["%w"] = "(\\d{1})";   // Weekday as a decimal number with Sunday as 0 (0-6)
        mRegExMap["%W"] = "(\\d{1,2})"; // Week number with the first Monday as the first day of week one (00-53)
        mRegExMap["%y"] = "(\\d{1,2})"; // Year, last two digits (00-99)
        mRegExMap["%Y"] = "(\\d{4})";   // Year 2001
        mRegExMap["%z"] = "(\\d{1,2})"; // ISO 8601 offset from UTC in timezone (1 minute=1, 1 hour=100)
        mRegExMap["%i"] = "(\\d{3})";   // Milliseconds
        mRegExMap["%c"] = "(\\d{6})";   // Microseconds
        mRegExMap["%x"] = "(\\d+)";     // Ignored integers
        mRegExMap["%X"] = "(\\w+)";     // Ignored characters
    }

    if (mMonthMap.empty())
    {
        mMonthMap["January"] = 1;
        mMonthMap["Jan"] = 1;
        mMonthMap["February"] = 2;
        mMonthMap["Feb"] = 2;
        mMonthMap["March"] = 3;
        mMonthMap["Mar"] = 3;
        mMonthMap["April"] = 4;
        mMonthMap["Apr"] = 4;
        mMonthMap["May"] = 5;
        mMonthMap["June"] = 6;
        mMonthMap["Jun"] = 6;
        mMonthMap["July"] = 7;
        mMonthMap["Jul"] = 7;
        mMonthMap["August"] = 8;
        mMonthMap["Aug"] = 8;
        mMonthMap["September"] = 9;
        mMonthMap["Sep"] = 9;
        mMonthMap["October"] = 10;
        mMonthMap["Oct"] = 10;
        mMonthMap["November"] = 11;
        mMonthMap["Nov"] = 11;
        mMonthMap["December"] = 12;
        mMonthMap["Dec"] = 12;
    }

    if (mDayOfWeekMap.empty())
    {
        mDayOfWeekMap["Monday"]    = 1;
        mDayOfWeekMap["Mon"]       = 1;
        mDayOfWeekMap["Tuesday"]   = 2;
        mDayOfWeekMap["Tue"]       = 2;
        mDayOfWeekMap["Wednesday"] = 3;
        mDayOfWeekMap["Wed"]       = 3;
        mDayOfWeekMap["Thursday"]  = 4;
        mDayOfWeekMap["Thu"]       = 4;
        mDayOfWeekMap["Friday"]    = 5;
        mDayOfWeekMap["Fri"]       = 5;
        mDayOfWeekMap["Saturday"]  = 6;
        mDayOfWeekMap["Sat"]       = 6;
        mDayOfWeekMap["Sunday"]    = 7;
        mDayOfWeekMap["Sun"]       = 7;
    }
}

TimeParser::~TimeParser()
{

}

BOOL TimeParser::setFormatString(const std::string& aFormat)
{
    BOOL fReturn = FALSE;
    std::string fRegExString = ".*"; // starts with any character
	size_t fSize = aFormat.size();
    BOOL fAdded = FALSE;
    mGroupID.clear();
    for (int i=0; i<fSize; ++i)
    {
        fAdded = FALSE;
        str_str_map::iterator fExpression = mRegExMap.find(aFormat.substr(i, 1));
        if (fExpression != mRegExMap.end())
        {
            fRegExString += fExpression->second;
            fAdded = TRUE;
        }
        else
        {
            fExpression = mRegExMap.find(aFormat.substr(i, 2));
            if (fExpression != mRegExMap.end())
            {
                fRegExString += fExpression->second;
                mGroupID     += fExpression->first[1];
                ++i;
                fAdded  = TRUE;
                fReturn = TRUE;
            }
        }
        if (!fAdded)
        {
            fRegExString += aFormat.substr(i, 1);
        }
    }
    if (!hasDate())
    {
        mMonth = 1;
    }
    mRegularExpression = fRegExString;

    return fReturn;
}

BOOL TimeParser::parseTimeString(const std::string& aTime)
{
    std::smatch fMatch;
    BOOL fReturn = TRUE;
    if (std::regex_search(aTime, fMatch, mRegularExpression))
    {
        int fGroupIndex = 0;
        for (unsigned int i = 1; i < fMatch.size(); ++i, ++fGroupIndex)
        {
            fReturn = fReturn && fMatch[i].matched;
            fReturn = fReturn && convertItem(fGroupIndex, fMatch.str(i));
        }
        if (mPM)
        {
            mHour += 12;
        }
        if (mYear != 0 && (mYear % 4) == 0)
        {
            mLeapYearDay = 1;
        }
    }
    else
    {
        fReturn = FALSE;
    }
    return fReturn;
}

BOOL TimeParser::getSystemTime(SYSTEMTIME& aTime, WORD& aMicroSecondPart) const
{
    aTime.wDay       = static_cast<WORD>(mDayOfMonth);
    aTime.wDayOfWeek = static_cast<WORD>(mDayOfWeek);
    aTime.wMonth     = static_cast<WORD>(mMonth);
    aTime.wYear      = static_cast<WORD>(mYear);
    aTime.wHour      = static_cast<WORD>(mHour);
    aTime.wMinute    = static_cast<WORD>(mMinute);
    aTime.wSecond    = static_cast<WORD>(mSecond);
    aTime.wMilliseconds = static_cast<WORD>(mMicroseconds / mMillitoMicroseconds);
	aMicroSecondPart = mMicroseconds % mMillitoMicroseconds;

    return TRUE;
}

BOOL TimeParser::setSystemTime(const SYSTEMTIME& aTime, WORD aMicroSecondPart) 
{
    mDayOfMonth   = aTime.wDay;
    mDayOfWeek    = aTime.wDayOfWeek;
    mMonth        = aTime.wMonth;
    mYear         = aTime.wYear;
    mHour         = aTime.wHour;
    mMinute       = aTime.wMinute;
    mSecond       = aTime.wSecond;
    mMicroseconds = aTime.wMilliseconds * mMillitoMicroseconds + aMicroSecondPart;
    return TRUE;
}

BOOL TimeParser::convertItem(int aItem, const std::string& aParam) 
{
    BOOL fReturn = TRUE;
    switch (mGroupID[aItem])
    {
    case 'a':                                                   // Abbreviated weekday name * Thu
    case 'A': mDayOfWeek = convertDayOfWeek(aParam.c_str()); break; // Full weekday name *  Thursday
    case 'b':                                                   // Abbreviated month name * Aug
    case 'B': mMonth = convertMonth(aParam.c_str());    break;  // Full month name * August
    case 'H': mHour = atoi(aParam.c_str());             break;  // Hour in 24h format (00-23)
    case 'I': mHour = atoi(aParam.c_str());             break;  // Hour in 12h format (01-12)
    case 'p': mPM = aParam == "PM";                     break;  // AM or PM designation PM
    case 'M': mMinute = atoi(aParam.c_str());           break;  // Minute (00-59)
    case 'S': mSecond = atoi(aParam.c_str());           break;  // Second (00-61)
    case 'l':
    {
        time_t fTimeSeconds = atoi(aParam.c_str());
        struct tm fTime;
        if (_gmtime64_s(&fTime, &fTimeSeconds) == 0)
        {
            mYear       = fTime.tm_year + 1900;
            mMonth      = fTime.tm_mon + 1;
            mDayOfMonth = fTime.tm_mday;
            mHour       = fTime.tm_hour;
            mMinute     = fTime.tm_min;
            mSecond     = fTime.tm_sec;
        }
        else
        {
			TRACE(Logger::to_function, "TimeParser::convertItem failed at %c, %s", mGroupID[aItem],  aParam.c_str());
            fReturn = FALSE;
        } 
    }   break;  // Seconds since 1970-01-01 00:00:00 +0000 (UTC).
    case 'u': mDayOfWeek = atoi(aParam.c_str());        break;  // ISO 8601 weekday as number with Monday as 1 (1-7)
    case 'w': mDayOfWeek = atoi(aParam.c_str()) + 1;    break;  // Weekday as a decimal number with Sunday as 0 (0-6)
    case 'd': mDayOfMonth = atoi(aParam.c_str());       break;  // Day of the month, zero-padded (01-31)
    case 'e': mDayOfMonth = atoi(aParam.c_str());       break;  // Day of the month, space-padded ( 1-31)
    case 'j': mDayOfYear = atoi(aParam.c_str());        break;  // Day of the year (001-366)
    case 'U': mWeekOfYear = atoi(aParam.c_str()) + 1;   break;  // Week number with the first Sunday as the first day of week one (00-53)
    case 'V': mWeekOfYear = atoi(aParam.c_str());       break;  // ISO 8601 week number (01-54)
    case 'W': mWeekOfYear = atoi(aParam.c_str()) + 1;   break;  // Week number with the first Monday as the first day of week one (00-53)
    case 'm': mMonth = atoi(aParam.c_str());            break;  // Month as a decimal number (01-12)
    case 'i': mMicroseconds = atoi(aParam.c_str()) * mMillitoMicroseconds; break;  // Miliseconds as a decimal number (000-999)
    case 'c': mMicroseconds = atoi(aParam.c_str());      break;  // Microeconds as a decimal number (000000-999999)
    case 'C': mYear = static_cast<int>(atof(aParam.c_str()) * 100.0f) + 2000; break;// Year divided by 100 and truncated to integer (00-99)
    case 'y': mYear = atoi(aParam.c_str()) + 2000;      break;  // Year, last two digits (00-99)
    case 'Y': mYear = atoi(aParam.c_str());             break;  // Year 2001
    case 'z': mHourTimzoneOffset = atoi(aParam.c_str()) / 100; break;// ISO 8601 offset from UTC in timezone (1 minute=1, 1 hour=100)
    default:
		TRACE(Logger::to_function, "TimeParser::convertItem failed at %c, %s", mGroupID[aItem], aParam.c_str());
		fReturn = FALSE; 
		break;
    }
    return fReturn;
}

int64_t TimeParser::calculateTime() const
{
    // number range integer:   2 ^ 63                                 = 9223372036854775808 
    // Microseconds per year: 1000000 * 60 * 60 * 24 * 366            = 31622400000000
    // Maximum number of years:  9223372036854775808 / 31622400000000 = 291672
    const int64_t fMicroseconds         = 1000000;
    const int64_t fMicroSecondsOfMinute =   60 * fMicroseconds;
    const int64_t fMicroSecondsOfHour   =   60 * fMicroSecondsOfMinute;
    const int64_t fMicroSecondsOfDay    =   24 * fMicroSecondsOfHour;
    const int64_t fMicroSecondsOfYear   =  366 * fMicroSecondsOfDay;
    return mYear * fMicroSecondsOfYear + getDayOfYear() * fMicroSecondsOfDay + mHour * fMicroSecondsOfHour + mMinute * fMicroSecondsOfMinute + mSecond * fMicroseconds + mMicroseconds;
}

SYSTEMTIME TimeParser::calcSystemTime(int64_t aTime, WORD* aMicrosecondPart)
{
    const int fMicroSecondsPerSecond = 1000000;
    const int fSecondsPerMinute = 60;
    const int fMinutesPerHour   = 60;
    const int fHoursPerDay      = 24;
    const int fDaysPerYear      = 366;
    SYSTEMTIME fST = { 0 };
	DWORD fMicroSeconds = static_cast<WORD>(aTime % fMicroSecondsPerSecond);
	fST.wMilliseconds = static_cast<WORD>(fMicroSeconds / 1000);
	fMicroSeconds %= 1000;
	if (aMicrosecondPart)
	{
		*aMicrosecondPart = static_cast<WORD>(fMicroSeconds);
	}
    aTime /= fMicroSecondsPerSecond;
    fST.wSecond = static_cast<WORD>(aTime % fSecondsPerMinute);
    aTime /= fSecondsPerMinute;
    fST.wMinute = static_cast<WORD>(aTime % fMinutesPerHour);
    aTime /= fMinutesPerHour;
    fST.wHour   = static_cast<WORD>(aTime % fHoursPerDay);
    aTime /= fHoursPerDay;
    fST.wDay    = static_cast<WORD>(aTime % fDaysPerYear);
    return fST;
}

int32_t TimeParser::calculateDay() const
{
    const int32_t fDaysOfYear = 366;
    return mYear * fDaysOfYear + getDayOfYear();
}

int32_t TimeParser::calculateSecondsOfDay() const
{
    const int32_t fSecondsOfMinute = 60;
    const int32_t fSecondsOfHour = 60 * fSecondsOfMinute;
    return mHour * fSecondsOfHour + mMinute * fSecondsOfMinute + mSecond;
}

int TimeParser::getDayOfYear() const 
{
    const int fOffsetJan =              31;
    const int fOffsetFeb = fOffsetJan + 28;
    const int fOffsetMar = fOffsetFeb + 31;
    const int fOffsetApr = fOffsetMar + 30;
    const int fOffsetMay = fOffsetApr + 31;
    const int fOffsetJun = fOffsetMay + 30;
    const int fOffsetJul = fOffsetJun + 31;
    const int fOffsetAug = fOffsetJul + 31;
    const int fOffsetSep = fOffsetAug + 30;
    const int fOffsetOkt = fOffsetSep + 31;
    const int fOffsetNov = fOffsetOkt + 30;

    switch (mMonth)
    {
     case  0: 
       if (mWeekOfYear > 0)
        {
            return (mWeekOfYear - 1) * 7 + mDayOfWeek;
        }
        else
        {
            return mDayOfYear;
        }
    case  1: return mDayOfMonth;
    case  2: return fOffsetJan + mDayOfMonth;
    case  3: return fOffsetFeb + mDayOfMonth + mLeapYearDay;
    case  4: return fOffsetMar + mDayOfMonth + mLeapYearDay;
    case  5: return fOffsetApr + mDayOfMonth + mLeapYearDay;
    case  6: return fOffsetMay + mDayOfMonth + mLeapYearDay;
    case  7: return fOffsetJun + mDayOfMonth + mLeapYearDay;
    case  8: return fOffsetJul + mDayOfMonth + mLeapYearDay;
    case  9: return fOffsetAug + mDayOfMonth + mLeapYearDay;
    case 10: return fOffsetSep + mDayOfMonth + mLeapYearDay;
    case 11: return fOffsetOkt + mDayOfMonth + mLeapYearDay;
    case 12: return fOffsetNov + mDayOfMonth + mLeapYearDay;
    }
	TRACE(Logger::warning, "TimeParser::getDayOfYear failed (m:%d, dom %d), (woy: %d, dow: %d), doy: %d", mMonth, mDayOfMonth, mWeekOfYear, mDayOfWeek, mDayOfYear);
	return 0;
}

int TimeParser::convertMonth(const char*aStr)
{
    str_int_map::iterator fIt = mMonthMap.find(aStr);
    if (fIt != mMonthMap.end())
    {
        return fIt->second;
    }
	TRACE(Logger::warning, "TimeParser::convertMonth failed: %s", aStr);
	return 0;
}

int TimeParser::convertDayOfWeek(const char*aStr)
{
    str_int_map::iterator fIt = mDayOfWeekMap.find(aStr);
    if (fIt != mDayOfWeekMap.end())
    {
        return fIt->second;
    }
	TRACE(Logger::warning, "TimeParser::convertDayOfWeek failed: %s", aStr);
	return 0;
}

BOOL TimeParser::hasDate() const
{
    std::regex fRegExpr("[aAbBuwdejUVWmCyY]");
    std::smatch fM;
    BOOL fResult = std::regex_search(mGroupID, fM, fRegExpr);

    return fResult;
}