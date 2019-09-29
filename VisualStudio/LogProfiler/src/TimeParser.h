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
    BOOL    setFormatString(const std::string& aFormat);
    BOOL    parseTimeString(const std::string& aTime);
    BOOL    getSystemTime(SYSTEMTIME& aTime) const;
    BOOL    setSystemTime(const SYSTEMTIME& aTime);
    int64_t calculateTime() const;
    int32_t calculateDay() const;
    int32_t calculateSecondsOfDay() const;
    BOOL    hasDate() const;

    static SYSTEMTIME calcSystemTime(int64_t aTime);
private:
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
