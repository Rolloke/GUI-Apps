#include "gitignore.h"
#include "logger.h"
#include "helper.h"

using namespace git;

GitIgnore::GitIgnore()
{
    init();
}

void GitIgnore::init()
{
    mIgnoreMap.push_back(string2typepair(Folder::FolderSelf, Type(Type::Folder|Type::FolderForNavigation)));
    mIgnoreMap.push_back(string2typepair(Folder::FolderUp, Type(Type::Folder|Type::FolderForNavigation)));
    mIgnoreMap.push_back(string2typepair(Folder::GitRepository, Type(Type::GitFolder)));
}

void GitIgnore::clear()
{
    mIgnoreMap.clear();
    init();
}

void GitIgnore::addGitIgnoreToIgnoreMapLevel(const QDir& aParentDir, std::vector<int>& aMapLevels)
{
    QFile file(aParentDir.filePath(Folder::GitIgnoreFile));

    if (file.open(QIODevice::ReadOnly))
    {
        int fMapLevel = std::max_element(mIgnoreMap.begin(), mIgnoreMap.end(), [] (stringt2typemap::const_reference fE1, stringt2typemap::const_reference fE2)
        {
            return fE1.second.level() < fE2.second.level();
        })->second.level() + 1;

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
            fLine.remove(QChar::CarriageReturn);
            fLine.remove(QChar::LineFeed);
            Type fType(Type::None, fMapLevel);
            fIndex = fLine.lastIndexOf('/');
            if (fIndex != -1 && fIndex == fLine.size() - 1)
            {
                fType.add(Type::Folder);
                fLine.remove(fIndex, 1);
            }
            else
            {
                fType.add(Type::File);
            }
            if (fLine.contains('*')) fType.add(Type::WildCard);
            if (fLine.contains('?')) fType.add(Type::WildCard);
            if (fLine.contains('[') && fLine.contains(']')) fType.add(Type::RegExp);
            if (fLine.contains('!')) fType.add(Type::Negation);

            fIndex = fLine.indexOf('!');
            if (fIndex == 0)
            {
                fLine.remove(fIndex, 1);
            }

            if (fLine.size())
            {
                //! TODO: are double entries git conform
                //! take flags into account

                auto item = std::find_if(mIgnoreMap.begin(), mIgnoreMap.end(), [&fLine, &fType](stringt2type_vector::const_reference it)
                {
                    return fLine.toStdString() == it.first && fType.type() == it.second.type();
                });
                if (item != mIgnoreMap.end())
                {
                    auto fMessage = QObject::tr("\"%1\" not inserted from %2").arg(fLine).arg(file.fileName());
                    TRACE(Logger::warning, fMessage.toStdString().c_str() );
                }
                else
                {
                    if (Logger::isSeverityActive(Logger::debug))
                    {
                        auto fMessage = QObject::tr("\"%1\" inserted from %2").arg(fLine).arg(file.fileName());
                        TRACE(Logger::debug, fMessage.toStdString().c_str() );
                    }
                    auto pair = std::pair<const std::string, Type>(fLine.toStdString(), fType);
                    mIgnoreMap.push_back(pair);
                }
            }
        }
        while (!file.atEnd());
    }
}

void GitIgnore::removeIgnoreMapLevel(uint aMapLevel)
{
    if (aMapLevel)
    {
        for (;;)
        {
            auto fElem = std::find_if(mIgnoreMap.begin(), mIgnoreMap.end(), [aMapLevel](stringt2typemap::const_reference fE)
            {
                return fE.second.level() == aMapLevel;
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
    stringt2type_vector ignored_entries;
    for (auto fItem = mIgnoreMap.begin(); fItem != mIgnoreMap.end(); ++fItem)
    {
        if (fItem->second.is(Type::WildCard) || fItem->second.is(Type::RegExp))
        {
            QRegExp fRegEx(fItem->first.c_str());
            fRegEx.setPatternSyntax(fItem->second.is(Type::WildCard) ? QRegExp::Wildcard : QRegExp::RegExp);
            if (fRegEx.exactMatch(fFileName.c_str()))
            {
                ignored_entries.push_back(*fItem);
            }
        }
        else if (fItem->first == fFileName)
        {
            ignored_entries.push_back(*fItem);
        }
    }
    if (   ignored_entries.size() > 2
        && Logger::isSeverityActive(Logger::debug))
    {
        std::string str;
        for (const auto& entry : ignored_entries)
        {
            str += entry.first;
            str += " -> ";
            str += entry.second.getStates(true).toStdString();
            str += ", ";
        }
        TRACE(Logger::debug, "ignored_entries: %d: %s", ignored_entries.size(), str.c_str());
    }

    if (ignored_entries.size() == 1)
    {
        return !ignored_entries[0].second.is(Type::Negation);
    }
    else if (ignored_entries.size() > 1)
    {
        if (ignored_entries[0].second.is(Type::FolderForNavigation))
        {
            return true;
        }
        if (ignored_entries.back().second.is(Type::Negation))
        {
            return false;
        }
        return true;
    }

    return false;
}

