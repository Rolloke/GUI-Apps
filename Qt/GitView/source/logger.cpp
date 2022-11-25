
#ifdef __linux__
 #include <syslog.h>
#endif

#ifdef WIN32

//#define USE_WINDOWS_LOG
#define USE_OUTPUT_DEBUG_STRING 1
#include <windows.h>

#endif

#include <stdarg.h>
#include <stdio.h>
#include "logger.h"

using namespace std;



std::uint32_t Logger::mSeverity = Logger::error | Logger::warning | Logger::notice | Logger::info | Logger::to_syslog | Logger::to_function;
map<std::string, int> Logger::mCurveColor;
string Logger::mLogdir("/tmp");
Logger::tLogfunction Logger::mLogFunction;
Logger::tLogfunction Logger::mTxt2BrowserFunction;

#ifdef USE_WINDOWS_LOG
namespace
{
HANDLE hEventLog = nullptr;
}

void convertToUnicode(const std::string& aSource, std::wstring& aDest)
{
    aDest.resize(aSource.size());
    ::MultiByteToWideChar(CP_ACP, 0, aSource.c_str(), static_cast<int>(aSource.size()), &aDest[0], static_cast<int>(aDest.size()));
    aDest.resize(wcslen(aDest.c_str()));
}

#endif

Logger::Logger(const char* fName)
{
#ifdef __linux__
    int fFlags = LOG_NDELAY | LOG_PID ;
    // Open syslog
    openlog(fName, fFlags, LOG_USER);
#endif
#ifdef USE_WINDOWS_LOG
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
#ifdef USE_WINDOWS_LOG
    CloseEventLog(hEventLog);
#endif
}

void Logger::printDebug (eSeverity aSeverity, const char * format, ... )
{
    if (isSeverityActive(aSeverity))
    {
        const bool log_to_syslog           = isSeverityActive(to_syslog);
        const bool log_to_function         = isSeverityActive(to_function) && mLogFunction;
        const bool log_to_browser_function = (aSeverity&to_browser) != 0 && mTxt2BrowserFunction;
        const bool log_to_console          = isSeverityActive(to_console);

        if (log_to_console)
        {
            va_list args;
            va_start (args, format);
            vprintf (format, args);
            va_end (args);
            fflush(stdout);
        }

        if (log_to_function)
        {
            char fMessage[2048]="";
            va_list args;
            va_start (args, format);
#ifdef __linux__
            vsnprintf(fMessage, sizeof (fMessage), format, args);
#else
            vsprintf_s(fMessage, sizeof (fMessage), format, args);
#endif
            va_end (args);
            mLogFunction(fMessage);
        }

        if (log_to_browser_function)
        {
            char fMessage[2048]="";
            va_list args;
            va_start (args, format);
#ifdef __linux__
            vsnprintf(fMessage, sizeof (fMessage), format, args);
#else
            vsprintf_s(fMessage, sizeof (fMessage), format, args);
#endif
            va_end (args);
            mTxt2BrowserFunction(fMessage);
        }

        if (log_to_syslog)
        {
#ifdef __linux__
            va_list args;
            va_start (args, format);
            vsyslog(convertSeverityToSyslogPriority(aSeverity), format, args);
            va_end (args);
#endif
#ifdef USE_OUTPUT_DEBUG_STRING
            char fMessage[2048]="";
            va_list args;
            va_start (args, format);
            vsprintf_s(fMessage, sizeof (fMessage), format, args);
            OutputDebugStringA(fMessage);
            va_end (args);
#endif
#ifdef USE_WINDOWS_LOG
            {
            va_list args;
            va_start (args, format);
            vsprintf(fMessage, format, args);
            va_end (args);
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
            }
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
    setSeverity(to_function, static_cast<bool>(aLogFunc));
    mLogFunction = aLogFunc;
}

void Logger::setTextToBrowserFunction(const tLogfunction &aFunc)
{
    setSeverity(to_browser, static_cast<bool>(aFunc));
    mTxt2BrowserFunction = aFunc;
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
#endif
#ifdef USE_WINDOWS_LOG
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
