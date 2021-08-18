#ifndef GITIGNORE_H
#define GITIGNORE_H

#include "git_type.h"

#include <QDir>
#include <map>
#include <boost/optional.hpp>


class GitIgnore
{
public:
    GitIgnore();

    void     addGitIgnoreToIgnoreMapLevel(const QDir& aParentDir, std::vector<int>& aMapLevels);
    void     removeIgnoreMapLevel(int aLevel);
    bool     ignoreFile(const QFileInfo& aFileInfo);
    void     clear();

private:
    void     init();
    git::stringt2typemap  mIgnoreMap;
    boost::optional<git::stringt2typemap::const_reference> mIgnoreContainingNegation;
    bool m_negation_is_completely_ignored {false};
};

#endif // GITIGNORE_H
