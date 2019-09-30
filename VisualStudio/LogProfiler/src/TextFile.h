#ifndef TEXT_FILE_H
#define TEXT_FILE_H

#include "./DockingFeature/Window.h"
#include "TextInterface.h"
#include <string>
#include <vector>

class TextFile : public ITextInterface
{
public:
    TextFile(LPCSTR aFileName);
    ~TextFile();

    size_t getLines() const;
    bool   getLineText(uint32_t aLineNo, std::string& aLine) const;
    LPCSTR getFileName();

private:
    LPCSTR mFileName;
    std::string mText;
    std::vector<size_t> mLineStart;
    std::string mLineFeed;
};

#endif // TEXT_FILE_H