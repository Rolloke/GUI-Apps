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
	/*!
	\brief constructor with arguments
	\param aTextIF the a class derived from text interface, that deliveres text lines
	\param aParser the time parser object
	*/
	SearchLine(const ITextInterface& aTextIF,  TimeParser* aParser);
    ~SearchLine();
	/*!
	\brief searches the line with the according time stamp
	\param aTime the search time in us
	\return the line, -1 if error occurred (int)
	*/
	int64_t searchLine(int64_t aTime);
	/*!
	\brief retrieves the difference of the time of the line to the search time
	\param aTime the search time
	\return time difference in us (int)
	*/
	int64_t getDifference();

private:
	int64_t search(size_t aStart, size_t aEnd, int64_t aTime);
	int64_t search_linear(size_t aLines, int64_t aTime);

    BOOL getTimeOfLine(size_t aLine, int64_t & aTime);

    const ITextInterface&  mTextIF;
    TimeParser*  mParser;
    std::map<int64_t, int64_t> mDifference;
    int          mMaxRecursion;
    int          mRecursion;
};

#endif // SEARCH_LINE_H

