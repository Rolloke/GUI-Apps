#pragma once

#include <string>

class ITextInterface
{
public:
    ITextInterface() : mLineLimit(std::string::npos) {};
    virtual size_t getLines() const = 0;
    virtual bool getLineText(uint32_t aLine, std::string& aLineText) const  = 0 ;
    void setLineLimit(size_t aLimit) { mLineLimit = aLimit; };
protected:
    size_t mLineLimit;
};
