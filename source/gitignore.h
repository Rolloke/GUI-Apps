#ifndef GITIGNORE_H
#define GITIGNORE_H

#include "git_type.h"

#include <QDir>
#include <QString>
#include <map>


class GitIgnore
{
public:
    GitIgnore();

    void     addGitIgnoreToIgnoreMapLevel(const QDir& aParentDir, std::vector<int>& aMapLevels);
    void     removeIgnoreMapLevel(uint aLevel, GitIgnore *ignored=nullptr);
    bool     ignoreFile(const QFileInfo& aFileInfo);
    void     clear();

    void     add_folder(const QString&entry);
    void     remove_entry(const QString&entry);
    const git::stringt2type_vector& getIgnoreMap()const;

private:
    void     init();
    git::stringt2type_vector  mIgnoreMap;
};

#endif // GITIGNORE_H
