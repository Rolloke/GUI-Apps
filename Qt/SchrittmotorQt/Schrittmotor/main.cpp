#include "main.h"
#include "mainwindow.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <QApplication>
#else
    #include <QtGui/QApplication>
#endif

#include <syslog.h>
#include <cstdio>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Q_INIT_RESOURCE(resource);
    MainWindow w;

    w.show();
    
    return a.exec();
}

Logger gTrace;

uint Logger::mSeverity = Logger::error | Logger::warning | Logger::info | Logger::to_syslog | Logger::to_info;
std::map<std::string, int> Logger::mCurveColor;
std::string Logger::mLogdir("/tmp");
Logger::tLogFunction Logger::mLogFunction;

Logger::Logger()
{
    int fFlags = LOG_NDELAY | LOG_PID ;
    // Open syslog
    openlog("Plotter", fFlags, LOG_USER);
}

Logger::~Logger()
{
    closelog();
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
        if (isSeverityActive(to_syslog))
        {
            va_start (args, format);
            vsyslog(convertSeverityToSyslogPriority(aSeverity), format, args);
            va_end (args);
        }
        if (aSeverity == to_info && mLogFunction)
        {
            char fText[1024];
            va_start (args, format);
            vsprintf (fText, format, args);
            mLogFunction(fText);
            va_end (args);
        }
    }
}

void Logger::setSeverity(uint aFlag, bool aSet)
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

void Logger::setLogFunction(Logger::tLogFunction aF)
{
    mLogFunction = aF;
}

uint Logger::getSeverity()
{
    return mSeverity;
}

bool Logger::isSeverityActive(eSeverity aSeverity)
{
    return (aSeverity & mSeverity) != 0;
}


int Logger::convertSeverityToSyslogPriority(const uint aSeverity)
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
        aStream << "title=" << aTitle << std::endl;
    }
    return fOpen;
}
