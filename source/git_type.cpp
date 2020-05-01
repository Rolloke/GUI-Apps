
#include "git_type.h"
#include <QStringList>
namespace git
{

int2stringmap Cmd::mCommandMap;
Cmd::tVector Cmd::mContextMenuSourceTree;
Cmd::tVector Cmd::mContextMenuEmptySourceTree;
Cmd::tVector Cmd::mContextMenuHistoryTree;
std::vector<Cmd::tVector> Cmd::mToolbars;

Cmd::Cmd()
{
    mCommandMap[GetStatusAll]    = "git -C %1 status -sb";
    mCommandMap[ShowStatus]      = "git status %1";
    mCommandMap[ShowShortStatus] = "git status -sb %1";
    mCommandMap[CallDiffTool]    = "git difftool --no-prompt %1";
    mCommandMap[ShowDifference]  = "git diff %1";
    mCommandMap[Remove]          = "git rm --cached %1";
    mCommandMap[Add]             = "git add %1";
    mCommandMap[Unstage]         = "git reset HEAD %1";
    mCommandMap[Commit]          = "git commit -m \"%1\" %2";
    mCommandMap[MoveOrRename]    = "git -C %1 mv %2 %3";
    mCommandMap[Restore]         = "git checkout %1";
    mCommandMap[History]         = "git log --pretty=format:\"%H<td>%T<td>%P<td>%s<td>%an<td>%ae<td>%ad<td>%cn<td>%ce<td>%cd<tr>\" %1";
    mCommandMap[Push]            = "git push";
    mCommandMap[CallHistoryDiffTool]    = "git difftool %1 --no-prompt %2";
    mCommandMap[ShowHistoryDifference]  = "git diff %1 %2";

    mContextMenuSourceTree      = { Add, Unstage, Restore, Remove, MoveOrRename, Separator, ShowDifference, CallDiffTool, ShowShortStatus, ShowStatus, Commit, History, Separator, ExpandTreeItems, CollapseTreeItems  };
    mContextMenuEmptySourceTree = { AddGitSourceFolder, UpdateGitStatus, Separator, ExpandTreeItems, CollapseTreeItems};

    mContextMenuHistoryTree     = { CallHistoryDiffTool, ShowHistoryDifference, Separator, ShowHideHistoryTree, ClearHistory };

    mToolbars.push_back({ Add, Unstage, Restore, MoveOrRename, Remove, Separator, ShowDifference , CallDiffTool, History, ShowStatus, ShowShortStatus });
    mToolbars.push_back({ AddGitSourceFolder, UpdateGitStatus, Separator, ShowHideHistoryTree, ClearHistory, ExpandTreeItems, CollapseTreeItems, Separator, Commit, Push });
    // TODO: CustomGitActionSettings
}


QString  Cmd::toString(const tVector& aItems)
{
    QString fString = "|";
    for (auto fItem: aItems)
    {
        fString += QString::number(fItem);
        fString += "|";
    }
    return fString;
}

Cmd::tVector Cmd::fromString(const QString& aString)
{
    QStringList fStrings = aString.split('|');
    tVector fItems;
    for (auto fItem: fStrings)
    {
        if (fItem.size() == 0) continue;
        fItems.push_back(static_cast<Cmd::eCmd>(fItem.toInt()));
    }
    return fItems;
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
        case GitStaged:    return "Staged";
        case GitDeleted:   return "Deleted";
        case GitModified:  return "Modified";
        case GitAdded:     return "Added";
        case GitUnTracked: return "Untracked";
        case GitRenamed:   return "Renamed";
        case GitUnmerged:  return "Unmerged";
        case GitLocal:     return "Local";
        case GitRemote:    return "Remote";
        case GitBoth:      return "Both";
        case GitFolder:    return "git folder";
        case GitIgnore:    return "git ignore";
        case SymLink:      return "symbolic link";
        case FileType:     return "file types (group)";
        case FileFlag:     return "file flags (group)";
        RETURN_NAME(AllGitActions);
        RETURN_NAME(Repository);
        RETURN_NAME(File);
        RETURN_NAME(Folder);
        RETURN_NAME(Hidden);
        RETURN_NAME(WildCard);
        RETURN_NAME(RegExp);
        RETURN_NAME(Negation);
        RETURN_NAME(ContainingNegation);
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
    if      (fIdentifier[1]=='M')   fType |= GitModified;
    else if (fIdentifier[0]=='M')   fType |= GitModified|GitStaged;
    if      (fIdentifier[1]=='A')   fType |= GitAdded;
    else if (fIdentifier[0]=='A')   fType |= GitAdded|GitStaged;
    if      (fIdentifier[1]=='R')   fType |= GitRenamed;
    else if (fIdentifier[0]=='R')   fType |= GitRenamed|GitStaged;
    if      (fIdentifier=="DD")     fType |= GitUnmerged|GitBoth;   // unmerged, both deleted
    else if (fIdentifier=="AU")     fType |= GitUnmerged|GitLocal;  // unmerged, added by us
    else if (fIdentifier=="UD")     fType |= GitUnmerged|GitRemote; // unmerged, deleted by them
    else if (fIdentifier=="UA")     fType |= GitUnmerged|GitRemote; // unmerged, added by them
    else if (fIdentifier=="DU")     fType |= GitUnmerged|GitLocal;  // unmerged, deleted by us
    else if (fIdentifier=="AA")     fType |= GitUnmerged|GitBoth;   // unmerged, both added
    else if (fIdentifier=="UU")     fType |= GitUnmerged|GitBoth;   // unmerged, both modified
    if (fIdentifier.contains("?"))  fType |= GitUnTracked;
    if (fIdentifier.contains("##")) fType |= Repository;
    return static_cast<TypeFlags>(fType);
}


}
