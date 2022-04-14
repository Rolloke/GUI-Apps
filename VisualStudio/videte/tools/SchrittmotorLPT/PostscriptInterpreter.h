#pragma once

#include <vector>
#include <string>
#include <map>

class CPlotterDC;
    // BeginProlog
    // BeginResource
    // EndResource
    // EndProlog

    // PageBoundingBox
    // BeginPageSetup
    // EndPageSetup

typedef std::vector< std::string > stringlist;

class PostscriptInterpreter
{
public:
    PostscriptInterpreter(CPlotterDC&aDC);
    ~PostscriptInterpreter();
    // enums
    enum eNames { moveto, lineto, curveto, arc, newpath, closepath, stroke, setlinewidth, rotate, scale};
    // worker
    void readPostscript(LPCTSTR sFileName);
    void parsePostscript();
    void parseInitial(const std::string& aLine);
    void parseProlog(const std::string& aLine);
    void parsePageSetup(const std::string& aLine);
    void parseDraw(const std::string& aLine);

    static size_t splitString(const std::string& aString, const std::string&aSeparator, stringlist& aList);
    static void decodeNumbers(const stringlist& aStrings, size_t aCount, std::vector<float>& aNumbers);

private:
    stringlist  mText;
    std::map< std::string, eNames > mNameMap;
    std::map< std::string, eNames > mShortNameMap;
    CPlotterDC &mDC;
    void (PostscriptInterpreter::*mParse)(const std::string& aLine);
    CRect mBoundingBox;
    float mScaleX;
    float mScaleY;
    float mRotate;
    CPen  mPen;
};
