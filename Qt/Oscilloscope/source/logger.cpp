
#ifdef __linux__
 #include <syslog.h>
#endif
#ifdef WIN32
#include <windows.h>
#endif
#include <stdarg.h>
#include "logger.h"

using namespace std;

std::uint32_t Logger::mSeverity = Logger::error | Logger::warning | Logger::info | Logger::to_syslog | Logger::to_function;
map<std::string, int> Logger::mCurveColor;
string Logger::mLogdir("/tmp");
Logger::tLogfunction Logger::mLogFunction;

#ifndef UNICODE
void   convertToUnicode(const std::string& aSource, std::string& aDest)
{
        aDest = aSource;
}
void convertToMBCS(const std::string& aSource, std::string& aDest)
{
        aDest = aSource;
}
#endif

void convertToUnicode(const std::string& aSource, std::wstring& aDest)
{
    aDest.resize(aSource.size());
    ::MultiByteToWideChar(CP_ACP, 0, aSource.c_str(), static_cast<int>(aSource.size()), &aDest[0], static_cast<int>(aDest.size()));
    aDest.resize(wcslen(aDest.c_str()));
}

void convertToMBCS(const std::wstring& aSource, std::string& aDest)
{
    aDest.resize(aSource.size() * 2);
    ::WideCharToMultiByte(CP_ACP, 0, aSource.c_str(), static_cast<int>(aSource.size()), &aDest[0], static_cast<int>(aDest.size()), NULL, NULL);
    aDest.resize(strlen(aDest.c_str()));
}

#ifdef WIN32
namespace
{
HANDLE hEventLog = nullptr;
}
#endif

Logger::Logger(const char* fName)
{
#ifdef __linux__
    int fFlags = LOG_NDELAY | LOG_PID ;
    // Open syslog
    openlog(fName, fFlags, LOG_USER);
#endif
#ifdef WIN32
#ifdef UNICODE
	std::wstring fNameW;
	convertToUnicode(fName, fNameW);
	hEventLog = OpenEventLogW(nullptr, fNameW.c_str());
#else
	hEventLog = OpenEventLogA(nullptr, fName);
#endif 

#endif
}

Logger::~Logger()
{
#ifdef __linux__
    closelog();
#endif
#ifdef WIN32
    CloseEventLog(hEventLog);
#endif
}

void Logger::printDebug (eSeverity aSeverity, const char * format, ... )
{
    if (isSeverityActive(aSeverity))
    {
        va_list args;
        if (isSeverityActive(to_console))
        {
            va_start (args, format);
            vprintf (format, args);
            va_end (args);
            fflush(stdout);
        }
		bool fToSyslog   = isSeverityActive(to_syslog);
		const bool fToFunction = (mLogFunction.operator bool() && (aSeverity&to_function) != 0);
        const size_t fSize = 1024;
        char fMessage[fSize] {0};
		if (fToSyslog || fToFunction)
		{
			va_start(args, format);
            vsprintf_s(fMessage, fSize, format, args);
            va_end (args);
        }


		if (fToFunction)
		{
			mLogFunction(fMessage);
			fToSyslog = false;
		}

		if (fToSyslog)
		{
#ifdef __linux__
            vsyslog(convertSeverityToSyslogPriority(aSeverity), format, args);
#endif
#ifdef WIN32
            std::uint16_t fCategory;
            std::uint32_t fEventID;
            std::uint16_t fType = convertSeverityToEventLog(aSeverity, fCategory, fEventID);
#ifdef UNICODE
			std::wstring fMessageW;
			convertToUnicode(fMessage, fMessageW);
			LPWSTR  fString[2] = { (LPWSTR)fMessageW.c_str(), NULL };
			ReportEventW(hEventLog, fType, fCategory, fEventID, nullptr, 1, 0, (LPCWSTR*)fString, nullptr);
#else
			LPSTR  fString[2] = { (LPSTR)fMessage, NULL };
			ReportEventA(hEventLog, fType, fCategory, fEventID, nullptr, 1, 0, (LPCSTR*)fString, nullptr);
#endif
#endif
        }
    }
}

void Logger::setSeverity(std::uint32_t aFlag, bool aSet)
{
    if (aSet)
    {
        mSeverity |= aFlag;
    }
    else
    {
        mSeverity &= ~aFlag;
    }
}

std::uint32_t Logger::getSeverity()
{
    return mSeverity;
}

bool Logger::isSeverityActive(eSeverity aSeverity)
{
    return (aSeverity & mSeverity) != 0;
}

void Logger::setLogFunction(const tLogfunction& aLogFunc)
{
	mLogFunction = aLogFunc;
}

#ifdef __linux__
int Logger::convertSeverityToSyslogPriority(const std::uint32_t aSeverity)
{
    int fSyslogPrio = LOG_INFO;

    switch (aSeverity)
    {
    case emergency: fSyslogPrio = LOG_EMERG;    break;
    case alert:     fSyslogPrio = LOG_ALERT;    break;
    case critical:  fSyslogPrio = LOG_CRIT;     break;
    case error:     fSyslogPrio = LOG_ERR;      break;
    case warning:   fSyslogPrio = LOG_WARNING;  break;
    case info:      fSyslogPrio = LOG_INFO;     break;
    case notice:    fSyslogPrio = LOG_NOTICE;   break;
    case trace:     fSyslogPrio = LOG_DEBUG;    break;
    case debug:     fSyslogPrio = LOG_DEBUG;    break;
    default:        fSyslogPrio = LOG_INFO;     break;
    }

    return fSyslogPrio;
}
#elif WIN32
std::uint16_t Logger::convertSeverityToEventLog(const std::uint32_t aSeverity, std::uint16_t& aCategory, std::uint32_t& aEvtID)
{
    std::uint16_t fEventType = EVENTLOG_SUCCESS;
    const std::uint32_t fIDError   = 0xC0000000;
    const std::uint32_t fIDWarning = 0x80000000;
    const std::uint32_t fIDInfo    = 0x40000000;

    aEvtID = 0x20000000;
    aCategory = static_cast<std::uint16_t>(aSeverity);

    switch (aSeverity)
    {
    case emergency:
    case alert:
    case critical:
        aEvtID |= fIDError;
        fEventType = EVENTLOG_AUDIT_FAILURE;
        break;
    case error:
        aEvtID |= fIDError;
        fEventType = EVENTLOG_ERROR_TYPE;
        break;
    case warning:
        aEvtID |= fIDWarning;
        fEventType = EVENTLOG_WARNING_TYPE;
        break;
    case notice:
        aEvtID |= fIDInfo;
        fEventType = EVENTLOG_SUCCESS;
        break;
    case trace:
        aEvtID |= fIDInfo;
        fEventType = EVENTLOG_AUDIT_SUCCESS;
        break;
    case info:
    case debug:
    default:
        aEvtID |= fIDInfo;
        fEventType = EVENTLOG_INFORMATION_TYPE;
        break;
    }

    return fEventType;
}
#endif


int Logger::getCurveColor(const std::string& sTitle)
{
    if (mCurveColor.count(sTitle) == 0)
    {
        mCurveColor[sTitle] = static_cast<int>(mCurveColor.size()+2);
    }
    return mCurveColor[sTitle];
}

bool Logger::openStream(const std::string& aTitle, std::ofstream& aStream)
{
    bool fOpen = false;
    aStream.open((mLogdir + "/" + aTitle + ".dat").c_str());
    if (aStream.is_open())
    {
        fOpen = true;
        aStream << "title=" << aTitle.c_str() << std::endl;
    }
    return fOpen;
}
