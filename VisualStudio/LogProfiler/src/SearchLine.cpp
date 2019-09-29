#include "SearchLine.h"

#include "TimeParser.h"
#include "PluginDefinition.h"
#include "TextInterface.h"

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

int SearchLine::searchLine(int64_t aTime)
{
    if (mParser)
    {
        size_t fLines = mTextIF.getLines();
        mMaxRecursion = static_cast<int>(log2(static_cast<double>(fLines))*1.5 + 0.5);
        int64_t fStartTime, fEndtime;
        if (getTimeOfLine(0, fStartTime) && fStartTime > aTime)
        {
            return -1;
        }
        if (getTimeOfLine(fLines - 1, fEndtime) && fEndtime < aTime)
        {
            return -1;
        }
        return search(0, fLines, aTime);
    }
    return -1;
}

int SearchLine::search(int aStart, int aEnd, int64_t aTime)
{
    if (   aEnd - aStart > 1            // limit distance
        && mRecursion < mMaxRecursion)  // limit recursion 
    {
        int fLine = (aStart + aEnd) / 2;
        int64_t fTimeOfLine = 0;
        int fTemp = fLine;
        BOOL bReverse = FALSE;
        ++mRecursion;   
        while (getTimeOfLine(fLine, fTimeOfLine) == FALSE)
        {    
            if (bReverse) // find a time line down
            {
                if (fLine > aStart)
                {
                    fLine--;
                }
                else
                {
                    return -1;
                }
            }
            else        //  find a time line up
            {
                if (fLine < aEnd)
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
            return search(aStart, fLine, aTime);
        }
        else
        {
            return search(fLine, aEnd, aTime);
        }
    }
    else
    {   // return the line with the smallest time distance
        return mDifference.size() > 0 ? mDifference.begin()->second : -1;
    }
}

int64_t SearchLine::getDifference()
{
    return mDifference.size() > 0 ? mDifference.begin()->first : -1;
}

BOOL SearchLine::getTimeOfLine(int aLine, int64_t & aTime)
{
    BOOL fReturn = FALSE;
    std::string fLineText;

    if (mTextIF.getLineText(aLine, fLineText))
    {
        if (mParser->parseTimeString(fLineText))
        {
            aTime = mParser->calculateTime();
            fReturn = TRUE;
        }
    }
    return fReturn;
}
