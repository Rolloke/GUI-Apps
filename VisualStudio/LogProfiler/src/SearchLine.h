#ifndef SEARCH_LINE_H
#define SEARCH_LINE_H

#include "./DockingFeature/Window.h"
#include <string>
#include <map>

class TimeParser;
class ITextInterface;

class SearchLine
{
public:
    SearchLine(const ITextInterface& aTextIF,  TimeParser* aParser);
    ~SearchLine();
    int searchLine(int64_t aTime);
    int64_t getDifference();

private:
    int search(int aStart, int aEnd, int64_t aTime);
    BOOL getTimeOfLine(int aLine, int64_t & aTime);

    const ITextInterface&  mTextIF;
    TimeParser*  mParser;
    std::map<int64_t, int> mDifference;
    int          mMaxRecursion;
    int          mRecursion;
};

#endif // SEARCH_LINE_H

