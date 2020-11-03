/*
 * TimeParser.h
 *
 *  Created on: Dec 19, 2017
 *      Author: developer
 */

#ifndef TIMEPARSER_H_
#define TIMEPARSER_H_

#include "DockingFeature\Window.h"
#include <map>
#include <string>
#include <regex>

typedef std::map<std::string, std::string> str_str_map;
typedef std::map<std::string, int> str_int_map;

class TimeParser
{
public:
    TimeParser();
    ~TimeParser();
	/*!
	\brief sets the time format string for the parser
	\param aFormat time format (const std::string&)
	\return TRUE, if the time format is valid (BOOL)
	*/
	BOOL    setFormatString(const std::string& aFormat);

	/*!
	\brief parses a text line containing a time stamp at begin
	\param aTime the text line (const std::string&)
	\return TRUE, if the time could be parsed correctly (BOOL)
	*/
	BOOL    parseTimeString(const std::string& aTime);

	/*!
	\brief retrieve the previously parsed time as system time
	\param the time result (SYSTEMTIME)
	\return TRUE (BOOL)
	*/
	BOOL    getSystemTime(SYSTEMTIME& aTime, WORD& aMicroSecondPart) const;

	/*!
	\brief initializes the time members
	\param the time to be set (const SYSTEMTIME&)
	\return TRUE (BOOL)
	*/
	BOOL    setSystemTime(const SYSTEMTIME& aTime, WORD aMicroSecondPart=0);

	/*!
	\brief retrieves, whether the time format has a date
	\return TRUE, if the date is returned as well (BOOL)
	*/
	BOOL    hasDate() const;

	/*!
	\brief retrieve the previously parsed time as internal time format in us 
	\return  (BOOL)
	*/
	int64_t calculateTime() const;

	/*!
	\brief calculates the system time from the internal time format
	\param aTime internal time format
	\return  (SYSTEMTIME)
	*/
	static SYSTEMTIME calcSystemTime(int64_t aTime, WORD* aMicrosecondPart=0);
private:
	int32_t calculateDay() const;
	int32_t calculateSecondsOfDay() const;
	BOOL    convertItem(int aItem, const std::string& aParam);
    int     getDayOfYear() const ;
    static int convertMonth(const char*);
    static int convertDayOfWeek(const char*);

    std::regex  mRegularExpression;
    std::string mGroupID;
    int         mYear;
    int         mMonth;
    int         mDayOfMonth;
    int         mWeekOfYear;
    int         mDayOfWeek;
    int         mDayOfYear;
    int         mHour;
    int         mHourTimzoneOffset;
    bool        mPM;
    int         mMinute;
    int         mSecond;
    int         mMicroseconds;
    int         mLeapYearDay;

    static const int mMillitoMicroseconds;
    static str_str_map  mRegExMap;
    static str_int_map  mMonthMap;
    static str_int_map  mDayOfWeekMap;
};

#endif /* TIMEPARSER_H_ */
