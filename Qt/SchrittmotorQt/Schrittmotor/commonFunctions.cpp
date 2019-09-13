#include "commonFunctions.h"


size_t splitString(const std::string& aString,const std::string&aSeparator, stringlist& aList)
{
    size_t fPos, fStart = 0;
    while ((fPos = aString.find(aSeparator, fStart)) != std::string::npos)
    {
        aList.push_back(aString.substr(fStart, fPos-fStart));
        fStart = fPos+aSeparator.size();
    }
    aList.push_back(aString.substr(fStart));
    return aList.size();
}

size_t trim_left(std::string& aString)
{
    size_t count = 0;
    while (!aString.empty() && *aString.begin() == ' ')
    {
        aString.erase(aString.begin());
        ++count;
    }
    return count;
}

size_t trim_right(std::string& aString)
{
    size_t count = 0;
    while (!aString.empty() && *aString.rbegin() == ' ')
    {
        aString.erase(aString.end()-1);
        ++count;
    }
    return count;
}
