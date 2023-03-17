#include "SearchLine.h"

#include "TimeParser.h"
#include "PluginDefinition.h"
#include "TextInterface.h"
#include "logger.h"

SearchLine::SearchLine(const ITextInterface& aTextIF, TimeParser* aParser)
    : mTextIF(aTextIF)
    , mParser(aParser)
    , mMaxRecursion(0)
    , mRecursion(0)
{
}


SearchLine::~SearchLine()
{
}

int64_t SearchLine::searchLine(int64_t aTime)
{
    if (mParser)
    {
		const size_t fLines = mTextIF.getLines();
		const size_t max_test_lines = 10;
        mMaxRecursion = static_cast<int>(log2(static_cast<double>(fLines))*1.5 + 0.5);
		int64_t fStartTime { 0 };
		int64_t fEndtime   { 0 };
		bool found_start = false;
		for (size_t fStart = 0; !found_start && fStart < max_test_lines; ++fStart)
		{
			found_start = getTimeOfLine(fStart, fStartTime);
		}
		bool found_end = false;
		for (size_t fEnd = fLines - 1; !found_end && fEnd >= fLines - max_test_lines; --fEnd)
		{
			found_end = getTimeOfLine(fEnd, fEndtime);
		}
		if (found_start && found_end)
		{
			if (fStartTime < fEndtime) // should be a sorted file
			{
				if (fStartTime > aTime || fEndtime < aTime)
				{
					TRACE(Logger::to_function, "Not in File time");
					return -1; 
				}
			}
		}
		int64_t fLine = search(0, fLines, aTime);
		if (fLine == -1 && fLines < 5000) // linear search is slow, lines are limited
		{
			TRACE(Logger::to_function, "Searching linear through %ld", fLines);
			return search_linear(fLines, aTime);
		}
		return fLine;
    }
    return -1;
}

int64_t SearchLine::search(size_t aStart, size_t aEnd, int64_t aTime)
{
    if (   aEnd - aStart > 1            // limit distance
        && mRecursion < mMaxRecursion)  // limit recursion 
    {
		int64_t fLine = static_cast<int64_t>((aStart + aEnd) / 2);
        int64_t fTimeOfLine = 0;
		int64_t fTemp = fLine;
        BOOL bReverse = FALSE;
        ++mRecursion;   
        while (getTimeOfLine(static_cast<size_t>(fLine), fTimeOfLine) == FALSE)
        {    
            if (bReverse) // find a time line down
            {
                if (fLine > static_cast<int64_t>(aStart))
                {
                    fLine--;
                }
                else
                {
					TRACE(Logger::to_function, "No time entry found");
					return -1;
                }
            }
            else        //  find a time line up
            {
                if (fLine < static_cast<int64_t>(aEnd))
                {
                    fLine++;
                }
                else
                {
                    fLine = fTemp - 1;
                    bReverse = TRUE;
                }
            }
        }

        int64_t  fDifference = fTimeOfLine - aTime;
        if (fDifference == 0)
        {
            mDifference[abs(fDifference)] = fLine;
            return fLine;
        }
        // store time distances according to line
        mDifference[abs(fDifference)] = fLine;

        if (fDifference > 0)
        {
            return search(aStart, static_cast<size_t>(fLine), aTime);
        }
        else
        {
            return search(static_cast<size_t>(fLine), aEnd, aTime);
        }
    }
    else
    {   // return the line with the smallest time distance
		int64_t fLine = mDifference.size() > 0 ? mDifference.begin()->second : -1;
		TRACE(Logger::to_function, "Recursive search stopped, recursion steps %d, distance %d, fLine", mRecursion, aEnd - aStart, fLine);
		return fLine;
    }
}

int64_t SearchLine::search_linear(size_t aLines, int64_t aTime)
{
	using namespace std;
	int fBestLine = -1;
	int64_t fBestDifference = static_cast<uint64_t>(1) << 63;

	for (size_t fLine=0; fLine < aLines; ++fLine)
	{
		int64_t fTimeOfLine = 0;

		if (getTimeOfLine(fLine, fTimeOfLine))
		{
			int64_t fDifference = abs(fTimeOfLine - aTime);
			if (fDifference < fBestDifference)
			{
				fBestDifference = fDifference;
				fBestLine = static_cast<int>(fLine);
			}
			if (fBestDifference == 0)
			{
				break;
			}
		}
	}
	mDifference[fBestDifference] = fBestLine;
	return fBestLine;
}

int64_t SearchLine::getDifference()
{
    return mDifference.size() > 0 ? mDifference.begin()->first : -1;
}

BOOL SearchLine::getTimeOfLine(size_t aLine, int64_t & aTime)
{
    BOOL fReturn = FALSE;
    std::string fLineText;

    if (mTextIF.getLineText(static_cast<uint32_t>(aLine), fLineText))
    {
        if (mParser->parseTimeString(fLineText))
        {
            aTime = mParser->calculateTime();
            fReturn = TRUE;
        }
    }
    return fReturn;
}
