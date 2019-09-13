#ifndef MAIN_H
#define MAIN_H

#include <QPalette>
#include <map>
#include <fstream>
#include <boost/function.hpp>

class QWidget;
//class QColor;

// helpers
void setWidgetStyleSheet(QWidget& aWidget, const QColor* aColor, const QColor* aBackgroundColor=0);
void setWidgetColor(QWidget& aWidget, QPalette::ColorRole aRole, const QColor& aColor);

class Logger
{
public:
    Logger();
    ~Logger();
    enum eSeverity {
        trace      = 0x0001,
        debug      = 0x0002,
        info       = 0x0004,
        notice     = 0x0008,
        warning    = 0x0010,
        error      = 0x0020,
        critical   = 0x0040,
        alert      = 0x0080,
        emergency  = 0x0100,
        to_console = 0x1000,
        to_syslog  = 0x2000,
        to_info    = 0x4000
    };
    typedef boost::function< void (const char*) > tLogFunction;

    static void setSeverity(uint aFlag, bool aSet);
    static void setLogFunction(tLogFunction aF);
    static uint getSeverity();
    static bool isSeverityActive(eSeverity aSeverity);
    static void printDebug (eSeverity aSeverity, const char * format, ... );
    template <class Type >
    static void printCurve(const std::vector<Type>& fX, const std::vector<Type>& fY, const std::string& aTitle)
    {
        std::ofstream fOutput;
        if (openStream(aTitle, fOutput))
        {
            int n = std::min(fX.size(), fY.size());
            for(int i=0; i<n; ++i)
            {
                fOutput << fX[i] << " " << fY[i] << std::endl;
            }
        }
    }
private:
    static int convertSeverityToSyslogPriority(const uint aFlag);
    static int getCurveColor(const std::string& aTitle);
    static bool openStream(const std::string& aTitle, std::ofstream& aStream);

    static uint mSeverity;
    static std::map<std::string, int> mCurveColor;
    static std::string mLogdir;
    static tLogFunction mLogFunction;
};

//#ifndef NDEBUG
#define TRACE Logger::printDebug
//#else
//#define TRACE //
//#endif


#endif // MAIN_H
