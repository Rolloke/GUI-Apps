#ifndef GITIGNORE_H
#define GITIGNORE_H

#include "git_type.h"

#include <QDir>
#include <map>


class GitIgnore
{
public:
    GitIgnore();

    void     addGitIgnoreToIgnoreMapLevel(const QDir& aParentDir, std::vector<int>& aMapLevels);
    void     removeIgnoreMapLevel(uint aLevel);
    bool     ignoreFile(const QFileInfo& aFileInfo);
    void     clear();

private:
    void     init();
    git::stringt2type_vector  mIgnoreMap;
};

#endif // GITIGNORE_H
