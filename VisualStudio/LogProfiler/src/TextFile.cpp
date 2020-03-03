#include "TextFile.h"
#include "PluginDefinition.h"


TextFile::TextFile(LPCSTR aFileName) : mFileName (aFileName)
{
    FILE *fp = fopen((const char*)aFileName, "rt" );
    fpos_t fSize = 0;
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        fgetpos(fp, &fSize);
        fseek(fp, 0, SEEK_SET);
    }
    if (fSize)
    {
        size_t fFileSize = static_cast<size_t>(fSize);
        mText.resize(fFileSize);
        if (fread(&mText[0], 1, fFileSize, fp) > 0)
        {
            bool fNL = mText.find("\n") != std::string::npos;
            bool fCR = mText.find("\r") != std::string::npos;
            if (fNL && fCR) mLineFeed = "\r\n";
            else if (fNL)   mLineFeed = "\n";
            else if (fCR)   mLineFeed = "\r";
            else            mLineFeed = " ";

            size_t fPos = 0;
            mLineStart.push_back(fPos);
            while ((fPos = mText.find(mLineFeed, fPos)) != -1)
            {
                fPos += mLineFeed.size();
                mLineStart.push_back(fPos);
            }
            mLineStart.push_back(fFileSize + mLineFeed.size());
        }
        fclose(fp);
    }
    else
    {
        DWORD dwError = GetLastError();
        printf("Error: %d\n", dwError);
    }
}

TextFile::~TextFile()
{
}

LPCSTR TextFile::getFileName()
{
    return mFileName;
}

size_t TextFile::getLines() const
{
    size_t fSize = mLineStart.size();
    return fSize > 0 ? fSize - 1 : 0;
}

bool TextFile::getLineText(uint32_t aLineNo, std::string& aLine) const
{
    if (aLineNo < static_cast<int>(getLines()))
    {
        aLine = mText.substr(mLineStart[aLineNo], mLineStart[aLineNo + 1] - mLineStart[aLineNo] - mLineFeed.size());
        if (mLineLimit != std::string::npos && aLine.size() > mLineLimit)
        {
            aLine.resize(mLineLimit);
        }
        return true;
    }
    return false;
}
