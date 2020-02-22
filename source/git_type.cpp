
#include "git_type.h"

namespace git
{

int2stringmap Cmd::mCommandMap;

Cmd::Cmd()
{
    mCommandMap[GetStatusAll]    = "git -C %1 status -sb";
    mCommandMap[ShowStatus]      = "git status %1";
    mCommandMap[ShowShortStatus] = "git status -sb %1";
    mCommandMap[ShowDifference]  = "git diff %1";
    mCommandMap[Remove]          = "git rm --cached %1";
    mCommandMap[Add]             = "git add %1";
    mCommandMap[Commit]          = "git commit -m \"%1\" %2";

    mCommandMap[MoveOrRename]    = "git mv %1 %2";
    mCommandMap[Push]            = "git push %1";

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

void Type::add(eType aType)
{
    mType = static_cast<eType>(mType|aType);
}

void Type::remove(eType aType)
{
    mType = static_cast<eType>(mType&~aType);
}

bool Type::is(eType aType)
{
    return (mType & aType) != None;
}

const char* Type::name(eType aType)
{
    switch (aType)
    {
        case GitDeleted: return "Del";
        case GitModified:return "Mod";
        case GitAdded:   return "Add";
        case GitUnknown: return "Unknown";
        case AllGitActions: return "AllGitActions";
        case Repository: return "Repository";
        case File:       return "File";
        case Folder:     return "Folder";
        case Hidden:     return "Hidden";
        case WildCard:   return "WildCard";
        case None:       return "None";
        case GitFolder:  return "git folder";
        case Checked:    return "Checked";
        case GitIgnore:  return "git ignore";
    }
    return "";
}

Type::eType Type::translate(const QString& fIdentifier)
{
    int fType = None;
    if (fIdentifier.contains('D'))  fType |= GitDeleted;
    if (fIdentifier.contains('M'))  fType |= GitModified;
    if (fIdentifier.contains('A'))  fType |= GitAdded;
    if (fIdentifier.contains("??")) fType |= GitUnknown;
    if (fIdentifier.contains("##")) fType |= Repository;
    return static_cast<eType>(fType);
}


}
