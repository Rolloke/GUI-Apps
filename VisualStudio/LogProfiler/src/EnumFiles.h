///////////////////////////////////////////////////////////////////////////////
///
/// (C) Copyright 2010 ELAC Nautik GmbH.
///
/// EnumFiles.h.
///
///
///////////////////////////////////////////////////////////////////////////////
///
/// \file EnumFiles.h.
///
/// \date   2010-06-09.
/// \author Rolf Kary-Ehlers (KE).
///
///////////////////////////////////////////////////////////////////////////////

#ifndef ENUMFILES_INCLUDED
#define ENUMFILES_INCLUDED

#include <string>
#include <vector>

#include ".\DockingFeature\Window.h"

class EnumFiles
{
public:
    EnumFiles(const std::string& aPath, const std::string& aPattern, bool aRecursive=false);
    ~EnumFiles(void);

    void enumFiles(std::vector<std::string>& aFiles);

private:
    EnumFiles();
    void recurseFiles(const std::string& aPath, std::vector<std::string>& aFiles);
    bool isDirectory() const;
    bool isReadOnly() const;
    bool isSubDirectory() const;
    const std::string& getPath() const;

    std::string mPath;
    std::string mPattern;
    bool        mRecursive;
    WIN32_FIND_DATAA mFindData;

    static const std::string mThisDirectory;
    static const std::string mUpperDirectory;
    static const std::string mSlash;
};

#endif // ENUMFILES_INCLUDED
