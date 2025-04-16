#include "gitignore.h"
#include "logger.h"
#include "helper.h"
#include <memory>


using namespace git;

GitIgnore::GitIgnore()
{
    init();
}

void GitIgnore::init()
{
    mIgnoreMap.push_back(string2typepair(Folder::FolderSelf, Type(Type::Folder|Type::FolderForNavigation), nullptr));
    mIgnoreMap.push_back(string2typepair(Folder::FolderUp, Type(Type::Folder|Type::FolderForNavigation), nullptr));
    mIgnoreMap.push_back(string2typepair(Folder::GitRepository, Type(Type::GitFolder), nullptr));
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
        return entry == std::get<_string>(it);
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
        return entry == std::get<_string>(it);
    });
    if (item == mIgnoreMap.end())
    {
        Type fType(Type::Folder);
        mIgnoreMap.push_back(string2typepair(entry, fType, nullptr));
    }
}


void GitIgnore::addGitIgnoreToIgnoreMapLevel(const QDir& aParentDir, std::vector<int>& aMapLevels)
{
    QFile file(aParentDir.filePath(Folder::GitIgnoreFile));

    if (file.open(QIODevice::ReadOnly))
    {
        int fMapLevel = std::get<_type>(*std::max_element(mIgnoreMap.begin(), mIgnoreMap.end(), [] (auto fE1, auto fE2)
        {
            return std::get<_type>(fE1).level() < std::get<_type>(fE2).level();
        })).level() + 1;

        aMapLevels.push_back(fMapLevel);
        do
        {
            QString line = file.readLine();
            line.remove(getLineFeed());
            if (line.startsWith('#'))  // ignore comment
            {
                continue;
            }
            line.remove(QChar::CarriageReturn);
            line.remove(QChar::LineFeed);

            Type type(Type::None, fMapLevel);

            if (line.startsWith('!'))      // Negation
            {
                type.add(Type::Negation);
                line.remove(0, 1);
            }
            if (line.startsWith("**/"))    // name behind match files or folders in all directories
            {
                type.add(Type::Folder);
                type.add(Type::File);
                line.remove(0, 3);
            }
            else if (line.endsWith("/**")) // folder name before matches all files inside
            {
                type.add(Type::Folder);
                type.add(Type::IncludeAll);
                line.remove(line.size()-4, 3);
            }
            else if (line.contains("/**/")) // matches consecutive folders e.g.: a/**/b a/b
            {
                type.add(Type::Folder);
                type.add(Type::RegExp);
                type.add(Type::Consecutive);
                line = line.replace("/**/", "/(.*/|)");
            }
            if (line.contains("\\"))       // Trailing backslash disables control characters
            {
                line.replace("\\!", "!");  // use exclamation mark trailing a file or folder name
                line.replace("\\#", "#");  // use # not as comment use within words
                line.replace("\\ ", " ");  // use space within a word
            }
            if (line.endsWith('/'))        // markes a Folder
            {
                type.add(Type::Folder);
                line.remove(line.size()-1, 1);
            }
            else
            {
                type.add(Type::File);
            }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            std::shared_ptr<QRegularExpression> regular_expr;
#else
            std::shared_ptr<QRegExp> regular_expr;
#endif
            if (   (line.contains('[') && line.contains(']')) // identify regular expression
                || (line.contains('{') && line.contains('}'))
                || (line.contains('(') && line.contains(')'))
                ||  line.contains('\\')
                ||  type.is(Type::RegExp))
            {
                type.add(Type::RegExp);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                regular_expr = std::make_shared<QRegularExpression>(line);
#else
                regular_expr = std::make_shared<QRegExp>(line);
                regular_expr->setPatternSyntax(QRegExp::RegExp);
#endif
            }
            else if (line.contains('*') || line.contains('?')) // identifiy wildcards
            {
                type.add(Type::WildCard);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                regular_expr = std::make_shared<QRegularExpression>(QRegularExpression::wildcardToRegularExpression(line));
#else
                regular_expr = std::make_shared<QRegExp>(line);
                regular_expr->setPatternSyntax(QRegExp::Wildcard);
#endif
            }                                                    // otherwise use exact match

            if (line.size())
            {
                auto item = std::find_if(mIgnoreMap.begin(), mIgnoreMap.end(), [&line, &type](stringt2type_vector::const_reference it)
                {
                    return line == std::get<_string>(it) && type.type() == std::get<_type>(it).type();
                });
                if (item != mIgnoreMap.end())
                {
                    TRACEX(Logger::warning, "\"" << line << "\": " << type.getStates(true) << " not inserted from " << file.fileName());
                }
                else
                {
                    if (Logger::isSeverityActive(Logger::debug))
                    {
                        TRACEX(Logger::debug, "\"" << line << "\": " << type.getStates(true) << " not inserted from " << file.fileName());
                    }
                    mIgnoreMap.push_back(string2typepair(line, type, regular_expr));
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
            auto fElem = std::find_if(mIgnoreMap.begin(), mIgnoreMap.end(), [aMapLevel](auto fE)
            {
                return std::get<_type>(fE).level() == aMapLevel;
            });

            if (fElem != mIgnoreMap.end())
            {
                if (ignored && std::get<_type>(*fElem).is(Type::Folder))
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
        const auto &type= std::get<_type>(*fItem);
        if (type.is(Type::WildCard) || type.is(Type::RegExp))
        {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            QRegularExpression &reg_ex = *std::get<_regex>(*fItem);
            QRegularExpressionMatch match = reg_ex.match(fFileName, 0);
            if (match.isValid() && match.hasMatch())
            {
                return true;
            }
#else
            QRegExp &fRegEx = *std::get<_regex>(*fItem);
            if (fRegEx.exactMatch(fFileName))
            {
                return true;
            }
#endif
        }
        else if (std::get<_string>(*fItem) == fFileName)
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
        const auto &type= std::get<_type>(*fItem);
        if (   !(type.is(Type::Folder) && type.is(Type::File))// not folder and file set AND
            && ((type.is(Type::Folder) && !aFileInfo.isDir())          // type folder required but is no folder OR
            ||  (type.is(Type::File)   && !aFileInfo.isFile())         // type file required but is no file
            ||   type.is(Type::IncludeAll)
            ||   type.is(Type::Consecutive)))                          // ignore this
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
        const auto &type= std::get<_type>(*fItem);
        if (type.is(Type::IncludeAll))
        {
            QStringList path_parts = aFileInfo.filePath().split('/');
            int size = path_parts.size();
            if (size > 1 && compare(fItem, path_parts[size-2]))
            {
                ignored_entries.clear();
            }
        }
        else if (type.is(Type::Consecutive))
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
            str += std::get<_string>(entry);
            str += " -> ";
            str += std::get<_type>(entry).getStates(true);
            str += ", ";
        }
        TRACEX(Logger::debug, "ignored_entries: " << ignored_entries.size() << ": " << str);
    }

    if (ignored_entries.size() == 1)
    {
        return !std::get<_type>(ignored_entries[0]).is(Type::Negation);
    }
    else if (ignored_entries.size() > 1)
    {
        if (std::get<_type>(ignored_entries[0]).is(Type::FolderForNavigation))
        {
            return true;
        }
        for (auto entry = ignored_entries.begin()+1; entry != ignored_entries.end(); ++entry)
        {
            if (std::get<_type>(*entry).is(Type::Negation))
            {
                if (Logger::isSeverityActive(Logger::debug))
                {
                    TRACEX(Logger::debug, "!ignored_entry: " << std::distance(ignored_entries.begin(), entry) << ": " << std::get<_string>(*entry));
                }
                return false;
            }
        }
        return true;
    }

    return false;
}

