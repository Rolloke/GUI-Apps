
#include "git_type.h"

namespace git
{

int2stringmap Cmd::mCommandMap;

Cmd::Cmd()
{
    mCommandMap[GetStatusAll]    = "git -C %1 status -sb";
    mCommandMap[ShowStatus]      = "git status %1";
    mCommandMap[ShowShortStatus] = "git status -sb %1";
    mCommandMap[CallDiffTool]    = "git difftool --no-prompt %1";
    mCommandMap[ShowDifference]  = "git diff %1";
    mCommandMap[Remove]          = "git rm --cached %1";
    mCommandMap[Add]             = "git add %1";
    mCommandMap[Commit]          = "git commit -m \"%1\" %2";
    mCommandMap[MoveOrRename]    = "git -C %1 mv %2 %3";
    mCommandMap[Restore]         = "git checkout %1";
    mCommandMap[History]         = "git log --pretty=format:\"%H<td>%T<td>%P<td>%s<td>%an<td>%ae<td>%ad<td>%cn<td>%ce<td>%cd<tr>\" %1";
    mCommandMap[Push]            = "git push %1";
    mCommandMap[CallHistoryDiffTool]    = "git difftool %1 --no-prompt %2";
    mCommandMap[ShowHistoryDifference]  = "git diff %1 %2";
}

const QString& Cmd::getCommand(eCmd aCmd)
{
    return mCommandMap[aCmd];
}

Cmd mInstance;

namespace Folder
{
    const char GitIgnoreFile[] = ".gitignore";
    const char GitRepository[] = ".git";
    const char FolderUp[]      = "..";
    const char FolderSelf[]    = ".";
}

void Type::add(TypeFlags aType)
{
    mType = static_cast<TypeFlags>(mType|aType);
}

void Type::remove(TypeFlags aType)
{
    mType = static_cast<TypeFlags>(mType&~aType);
}

bool Type::is(TypeFlags aType) const
{
    return (mType & aType) != None;
}

#define RETURN_NAME(NAME) case NAME: return #NAME
const char* Type::name(TypeFlags aType)
{
    switch (aType)
    {
        case GitDeleted: return "Deleted";
        case GitModified:return "Modified";
        case GitAdded:   return "Added";
        case GitUnknown: return "Unknown";
        case GitRenamed: return "Renamed";
        case GitFolder:  return "git folder";
        case GitIgnore:  return "git ignore";
        case SymLink:    return "symbolic link";
        case FileType:   return "file types (group)";
        case FileFlag:   return "file flags (group)";
        RETURN_NAME(AllGitActions);
        RETURN_NAME(Repository);
        RETURN_NAME(File);
        RETURN_NAME(Folder);
        RETURN_NAME(Hidden);
        RETURN_NAME(WildCard);
        RETURN_NAME(None);
        RETURN_NAME(Checked);
        RETURN_NAME(Executeable);
    }
    return "";
}

Type::TypeFlags Type::translate(const QString& fIdentifier)
{
    int fType = None;
    if (fIdentifier.contains('D'))  fType |= GitDeleted;
    if (fIdentifier.contains('M'))  fType |= GitModified;
    if (fIdentifier.contains('A'))  fType |= GitAdded;
    if (fIdentifier.contains('R'))  fType |= GitRenamed;
    if (fIdentifier.contains("?"))  fType |= GitUnknown;
    if (fIdentifier.contains("##")) fType |= Repository;
    return static_cast<TypeFlags>(fType);
}


}
