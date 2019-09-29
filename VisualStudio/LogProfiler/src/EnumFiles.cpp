
#include "EnumFiles.h"
#include <algorithm>

const std::string EnumFiles::mThisDirectory(".");
const std::string EnumFiles::mUpperDirectory("..");
const std::string EnumFiles::mSlash("\\");

EnumFiles::EnumFiles(const std::string& aPath, const std::string&  aPattern, bool aRecursive/* =false */):
  mRecursive(aRecursive)
{
    mPath = aPath;
    if (!mPath.empty() && *mPath.rbegin() != mSlash[0])
    {
        mPath += mSlash;
    }
    mPattern = aPattern;

    ZeroMemory(&mFindData, sizeof(mFindData));
}


EnumFiles::~EnumFiles(void)
{
}

void EnumFiles::enumFiles(std::vector<std::string>& aFiles)
{
    recurseFiles(mPath, aFiles);
}

void EnumFiles::recurseFiles(const std::string& aPath, std::vector<std::string>& aFiles)
{
    HANDLE fHandle;
    BOOL fResult;

    if (mRecursive)
    {
        fHandle = FindFirstFileA((aPath + "*.*").c_str(), &mFindData);
        fResult = fHandle != INVALID_HANDLE_VALUE;
        while (fResult)
        {
            fResult = FindNextFileA(fHandle, &mFindData);
            if (fResult)
            {
                if (isSubDirectory())
                {
                    strcat(mFindData.cFileName, "\\");
                    recurseFiles(aPath + mFindData.cFileName, aFiles);
                }
            }
        }
        if (fHandle != INVALID_HANDLE_VALUE)
        {
            FindClose(fHandle);
        }
    }
    fHandle = FindFirstFileA((aPath + mPattern).c_str(), &mFindData);
    fResult = fHandle != INVALID_HANDLE_VALUE;
    while (fResult)
    {
        fResult = FindNextFileA(fHandle, &mFindData);
        if (fResult)
        {
            if (!isDirectory())
            {
                aFiles.push_back(aPath + mFindData.cFileName);
            }
        }
    }
    if (fHandle != INVALID_HANDLE_VALUE)
    {
        FindClose(fHandle);
    }
}

bool EnumFiles::isDirectory() const
{
    return ((mFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

bool EnumFiles::isReadOnly() const
{
    return ((mFindData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0);
}

bool EnumFiles::isSubDirectory() const 
{
    return (   (mFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0
            && mThisDirectory != mFindData.cFileName
            && mUpperDirectory != mFindData.cFileName);
}


const std::string& EnumFiles::getPath() const
{
   return mPath;
}

