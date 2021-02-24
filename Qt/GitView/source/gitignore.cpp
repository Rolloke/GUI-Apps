#include "gitignore.h"
#include "logger.h"
#include "helper.h"

using namespace git;

GitIgnore::GitIgnore()
{
    mIgnoreMap[Folder::FolderSelf]    = Type(Type::Folder);
    mIgnoreMap[Folder::FolderUp]      = Type(Type::Folder);
    mIgnoreMap[Folder::GitRepository] = Type(Type::GitFolder);
}

void GitIgnore::addGitIgnoreToIgnoreMapLevel(const QDir& aParentDir, std::vector<int>& aMapLevels)
{
    QFile file(aParentDir.filePath(Folder::GitIgnoreFile));

    if (file.open(QIODevice::ReadOnly))
    {
        int fMapLevel = std::max_element(mIgnoreMap.begin(), mIgnoreMap.end(), [] (stringt2typemap::const_reference fE1, stringt2typemap::const_reference fE2)
        {
            return fE1.second.mLevel < fE2.second.mLevel;
        })->second.mLevel + 1;

        aMapLevels.push_back(fMapLevel);
        do
        {
            QString fLine = file.readLine();
            fLine.remove(getLineFeed());
            int fIndex = fLine.indexOf('#');
            if (fIndex != -1)
            {
                fLine.remove(fIndex,fLine.size()-fIndex);
                fLine = fLine.trimmed();
            }
            Type fType(fLine.contains('/') ? Type::Folder : Type::File, fMapLevel);
            if (fLine.contains('*')) fType.add(Type::WildCard);
            if (fLine.contains('?')) fType.add(Type::WildCard);
            if (fLine.contains('[') && fLine.contains(']')) fType.add(Type::RegExp);
            if (fLine.contains('!')) fType.add(Type::Negation);
            fLine.remove(QChar::CarriageReturn);
            fIndex = fLine.lastIndexOf('/');
            if (fIndex != -1 && fIndex == fLine.size() - 1)
            {
                fLine.remove(fIndex, 1);
            }
            fIndex = fLine.indexOf('!');
            if (fIndex == 0)
            {
                fLine.remove(fIndex, 1);
            }

            if (fLine.size())
            {
                if (mIgnoreMap.count(fLine.toStdString()))
                {
                    auto fMessage = QObject::tr("\"%1\" not inserted from %2").arg(fLine).arg(file.fileName());
                    TRACE(Logger::warning, fMessage.toStdString().c_str() );
                }
                else
                {
                    auto fMessage = QObject::tr("\"%1\" inserted from %2").arg(fLine).arg(file.fileName());
                    TRACE(Logger::debug, fMessage.toStdString().c_str() );
                    mIgnoreMap[fLine.toStdString()] = fType;
                }
            }
        }
        while (!file.atEnd());
    }
    for (const auto& fItem : mIgnoreMap)
    {
        if (fItem.second.is(Type::Negation))
        {
            for (auto& fSearchItem : mIgnoreMap)
            {
                if (   fItem.first.find(fSearchItem.first) != std::string::npos
                    && fItem.first != fSearchItem.first)
                {
                    // TODO: implement negation, but first understand how it works
//                    fSearchItem.second.add(Type::ContainingNegation);
                }
            }
        }
    }
}

void GitIgnore::removeIgnoreMapLevel(int aMapLevel)
{
    if (aMapLevel)
    {
        for (;;)
        {
            auto fElem = std::find_if(mIgnoreMap.begin(), mIgnoreMap.end(), [aMapLevel](stringt2typemap::const_reference fE)
            {
                 return fE.second.mLevel==aMapLevel;
            });

            if (fElem != mIgnoreMap.end())
            {
                mIgnoreMap.erase(fElem);
            }
            else break;
        }
    }
}

bool GitIgnore::ignoreFile(const QFileInfo& aFileInfo)
{
    const std::string& fFileName = aFileInfo.fileName().toStdString();
//    const std::string& fFilePath = aFileInfo.filePath().toStdString();

    auto fFound = mIgnoreMap.find(fFileName);
    if (fFound == mIgnoreMap.end())
    {
        for (auto fItem = mIgnoreMap.begin(); fItem != mIgnoreMap.end(); ++fItem)
        {
            if (fItem->second.is(Type::WildCard) || fItem->second.is(Type::RegExp))
            {
                QRegExp fRegEx(fItem->first.c_str());
                fRegEx.setPatternSyntax(fItem->second.is(Type::WildCard) ? QRegExp::Wildcard : QRegExp::RegExp);
                if (fRegEx.exactMatch(fFileName.c_str()))
                {
                    fFound = fItem;
                    break;
                }
            }
        }
    }

    if (mIgnoreContainingNegation)
    {
        if (fFound != mIgnoreMap.end() && fFound->second.is(Type::Negation))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        if (fFound != mIgnoreMap.end())
        {
            if (fFound->second.is(Type::ContainingNegation))
            {
                mIgnoreContainingNegation = *fFound;
                return false;
            }
            if (fFound->second.is(Type::File)   && aFileInfo.isFile()) return true;
            if (fFound->second.is(Type::Folder) && aFileInfo.isDir() ) return true;
        }
    }
    return false;
}

