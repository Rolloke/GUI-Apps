#include "gitignore.h"
#include "logger.h"
#include "helper.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRegularExpression>
#endif

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

const git::stringt2type_vector& GitIgnore::getIgnoreMap() const
{
    return mIgnoreMap;
}

void GitIgnore::clear()
{
    mIgnoreMap.clear();
    init();
}

void GitIgnore::remove_entry(const QString& entry)
{
    auto item = std::find_if(mIgnoreMap.begin(), mIgnoreMap.end(), [&entry](stringt2type_vector::const_reference it)
    {
        return entry == it.first;
    });
    if (item != mIgnoreMap.end())
    {
        mIgnoreMap.erase(item);
    }
}

void GitIgnore::add_folder(const QString&entry)
{
    auto item = std::find_if(mIgnoreMap.begin(), mIgnoreMap.end(), [&entry](stringt2type_vector::const_reference it)
    {
        return entry == it.first;
    });
    if (item == mIgnoreMap.end())
    {
        Type fType(Type::Folder);
        auto pair = std::pair<const QString, Type>(entry, fType);
        mIgnoreMap.push_back(pair);
    }
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
            if (fLine.startsWith('#'))  // ignore comment
            {
                continue;
            }
            fLine.remove(QChar::CarriageReturn);
            fLine.remove(QChar::LineFeed);

            Type fType(Type::None, fMapLevel);

            if (fLine.startsWith('!'))      // Negation
            {
                fType.add(Type::Negation);
                fLine.remove(0, 1);
            }
            if (fLine.startsWith("**/"))    // name behind match files or folders in all directories
            {
                fType.add(Type::Folder);
                fType.add(Type::File);
                fLine.remove(0, 3);
            }
            else if (fLine.endsWith("/**")) // folder name before matches all files inside
            {
                fType.add(Type::Folder);
                fType.add(Type::IncludeAll);
                fLine.remove(fLine.size()-4, 3);
            }
            else if (fLine.contains("/**/")) // matches consecutive folders e.g.: a/**/b a/b
            {
                fType.add(Type::Folder);
                fType.add(Type::RegExp);
                fType.add(Type::Consecutive);
                fLine = fLine.replace("/**/", "/(.*/|)");
            }
            if (fLine.contains("\\"))       // Trailing backslash disables control characters
            {
                fLine.replace("\\!", "!");  // use exclamation mark trailing a file or folder name
                fLine.replace("\\#", "#");  // use # not as comment use within words
                fLine.replace("\\ ", " ");  // use space within a word
            }
            if (fLine.endsWith('/'))        // markes a Folder
            {
                fType.add(Type::Folder);
                fLine.remove(fLine.size()-1, 1);
            }
            else
            {
                fType.add(Type::File);
            }

            if (   (fLine.contains('[') && fLine.contains(']')) // identify regular expression
                || (fLine.contains('{') && fLine.contains('}'))
                || (fLine.contains('(') && fLine.contains(')'))
                ||  fLine.contains('\\') )
            {
                fType.add(Type::RegExp);
            }
            else if (fLine.contains('*') || fLine.contains('?')) // identifiy wildcards
            {
                fType.add(Type::WildCard);
            }                                                    // otherwise use exact match

            if (fLine.size())
            {
                auto item = std::find_if(mIgnoreMap.begin(), mIgnoreMap.end(), [&fLine, &fType](stringt2type_vector::const_reference it)
                {
                    return fLine == it.first && fType.type() == it.second.type();
                });
                if (item != mIgnoreMap.end())
                {
                    TRACEX(Logger::warning, "\"" << fLine << "\": " << fType.getStates(true) << " not inserted from " << file.fileName());
                }
                else
                {
                    if (Logger::isSeverityActive(Logger::debug))
                    {
                        TRACEX(Logger::debug, "\"" << fLine << "\": " << fType.getStates(true) << " not inserted from " << file.fileName());
                    }
                    auto pair = std::pair<const QString, Type>(fLine, fType);
                    mIgnoreMap.push_back(pair);
                }
            }
        }
        while (!file.atEnd());
    }
}

void GitIgnore::removeIgnoreMapLevel(uint aMapLevel, GitIgnore* ignored)
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
                if (ignored && fElem->second.is(Type::Folder))
                {
                    ignored->mIgnoreMap.push_back(*fElem);
                }
                mIgnoreMap.erase(fElem);
            }
            else break;
        }
    }
}

namespace
{
    bool compare(stringt2type_vector::const_iterator fItem, const QString& fFileName)
    {
        if (fItem->second.is(Type::WildCard) || fItem->second.is(Type::RegExp))
        {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            if ( fItem->second.is(Type::WildCard))
            {
                QString wildcard = QRegularExpression::wildcardToRegularExpression(fItem->first);
                QRegularExpression reg_ex(wildcard);
                QRegularExpressionMatch match = reg_ex.match(fFileName, 0);
                if (match.isValid() && match.hasMatch())
                {
                    return true;
                }
            }
            else
            {
                QRegularExpression reg_ex(fItem->first);
                QRegularExpressionMatch match = reg_ex.match(fFileName, 0);
                if (match.isValid() && match.hasMatch())
                {
                    return true;
                }
            }
#else
            QRegExp fRegEx(fItem->first);
            fRegEx.setPatternSyntax(fItem->second.is(Type::WildCard) ? QRegExp::Wildcard : QRegExp::RegExp);
            if (fRegEx.exactMatch(fFileName))
            {
                return true;
            }
#endif
        }
        else if (fItem->first == fFileName)
        {
            return true;
        }
        return false;
    }
}

bool GitIgnore::ignoreFile(const QFileInfo& aFileInfo)
{
    const QString& fFileName = aFileInfo.fileName();
    stringt2type_vector ignored_entries;
    for (auto fItem = mIgnoreMap.begin(); fItem != mIgnoreMap.end(); ++fItem)
    {
        if (   !(fItem->second.is(Type::Folder) && fItem->second.is(Type::File))// not folder and file set AND
            && ((fItem->second.is(Type::Folder) && !aFileInfo.isDir())          // type folder required but is no folder OR
             || (fItem->second.is(Type::File)   && !aFileInfo.isFile())         // type file required but is no file
             ||  fItem->second.is(Type::IncludeAll)
             ||  fItem->second.is(Type::Consecutive)))                          // ignore this
        {
            continue;
        }
        if (compare(fItem, fFileName))
        {
            ignored_entries.push_back(*fItem);
        }
    }

    for (auto fItem = mIgnoreMap.begin(); fItem != mIgnoreMap.end(); ++fItem)
    {
        if (fItem->second.is(Type::IncludeAll))
        {
            QStringList path_parts = aFileInfo.filePath().split('/');
            int size = path_parts.size();
            if (size > 1 && compare(fItem, path_parts[size-2]))
            {
                ignored_entries.clear();
            }
        }
        else if (fItem->second.is(Type::Consecutive))
        {
            if (compare(fItem, aFileInfo.filePath()))
            {
                ignored_entries.push_back(*fItem);
            }
        }
    }

    if (   ignored_entries.size() > 2
        && Logger::isSeverityActive(Logger::debug))
    {
        QString str;
        for (const auto& entry : std::as_const(ignored_entries))
        {
            str += entry.first;
            str += " -> ";
            str += entry.second.getStates(true);
            str += ", ";
        }
        TRACEX(Logger::debug, "ignored_entries: " << ignored_entries.size() << ": " << str);
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
        for (auto entry = ignored_entries.begin()+1; entry != ignored_entries.end(); ++entry)
        {
            if (entry->second.is(Type::Negation))
            {
                if (Logger::isSeverityActive(Logger::debug))
                {
                    TRACEX(Logger::debug, "!ignored_entry: " << std::distance(ignored_entries.begin(), entry) << ": " << entry->first);
                }
                return false;
            }
        }
        return true;
    }

    return false;
}

