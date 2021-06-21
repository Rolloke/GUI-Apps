#ifndef LOGGER_H
#define LOGGER_H

#include <map>
#include <fstream>
#include <vector>
#include <functional>


class Logger
{
public:
	typedef std::function<void(const std::string&)> tLogfunction;
    Logger(const char* fName);
    ~Logger();
    enum eSeverity
    {
        trace       = 0x0001,
        debug       = 0x0002,
        info        = 0x0004,
        notice      = 0x0008,
        warning     = 0x0010,
        error       = 0x0020,
        critical    = 0x0040,
        alert       = 0x0080,
        emergency   = 0x0100,
        to_console  = 0x1000,
        to_syslog   = 0x2000,
        to_function = 0x4000,
        to_browser  = 0x8000
    };

    static void setSeverity(std::uint32_t aFlag, bool aSet);
    static std::uint32_t getSeverity();
    static bool isSeverityActive(eSeverity aSeverity);
    static void printDebug (eSeverity aSeverity, const char * format, ... );
	static void setLogFunction(const tLogfunction& aFunc);
    static void setTextToBrowserFunction(const tLogfunction& aFunc);
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
#ifdef __linux__
    static int convertSeverityToSyslogPriority(const std::uint32_t aFlag);
#elif WIN32
    static  std::uint16_t convertSeverityToEventLog(const std::uint32_t aFlag, std::uint16_t& aCategory, std::uint32_t& aEvtID);
#endif
    static int getCurveColor(const std::string& aTitle);
    static bool openStream(const std::string& aTitle, std::ofstream& aStream);

    static std::uint32_t mSeverity;
    static std::map<std::string, int> mCurveColor;
    static std::string mLogdir;
	static tLogfunction mLogFunction;
    static tLogfunction mTxt2BrowserFunction;
};

//#ifndef NDEBUG
#define TRACE Logger::printDebug
//#else
//#define TRACE //
//#endif


#define UNUSED(x) (void)(x)

#endif // LOGGER_H
