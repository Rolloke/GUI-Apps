
#include "git_type.h"
#include <QStringList>
#include <QObject>
#include <QFileInfo>

namespace git
{

int2stringmap Cmd::mCommandMap;
Cmd::tVector Cmd::mContextMenuSourceTree;
Cmd::tVector Cmd::mContextMenuEmptySourceTree;
Cmd::tVector Cmd::mContextMenuHistoryTree;
Cmd::tVector Cmd::mContextMenuBranchTree;
std::vector<Cmd::tVector> Cmd::mToolbars;

Cmd::Cmd()
{
    mCommandMap[GetStatusAll]           = "git -C %1 status -sb";
    mCommandMap[ShowStatus]             = "git status %1";
    mCommandMap[ShowShortStatus]        = "git status -sb %1";
    mCommandMap[CallDiffTool]           = "git difftool --no-prompt %1";
    mCommandMap[ShowDifference]         = "git diff %1";
    mCommandMap[Remove]                 = "git rm --cached %1";
    mCommandMap[Add]                    = "git add %1";
    mCommandMap[Unstage]                = "git reset HEAD %1";
    mCommandMap[Commit]                 = "git -C %1 commit -m \"%2\"";
    mCommandMap[MoveOrRename]           = "git -C %1 mv %2 %3";
    mCommandMap[Restore]                = "git checkout %1";
    mCommandMap[History]                = "git log --pretty=format:\"%H<td>%T<td>%P<td>%B<td>%an<td>%ae<td>%ad<td>%cn<td>%ce<td>%cd<tr>\" %1";
    mCommandMap[Push]                   = "git -C %1 push";
    mCommandMap[Pull]                   = "git -C %1 pull";
    mCommandMap[Show]                   = "git show %1";
    mCommandMap[ShowHistoryDifference]  = "git diff %1 %2";
    mCommandMap[CallHistoryDiffTool]    = "git difftool %1 --no-prompt %2";
    mCommandMap[BranchList]             = "git -C %1 branch --list";
    mCommandMap[BranchDelete]           = "git -C %1 branch --delete --force %2";
    mCommandMap[BranchShow]             = "git -C %1 show-branch %2";
    mCommandMap[BranchListRemote]       = "git -C %1 branch --list --remotes";
    mCommandMap[BranchListMerged]       = "git -C %1 branch --list --merged";
    mCommandMap[BranchListNotMerged]    = "git -C %1 branch --list --no-merged";
    mCommandMap[BranchCheckout]         = "git -C %1 checkout %2";

    mContextMenuSourceTree      = { Add, Unstage, Restore, Remove, MoveOrRename, Separator, ShowDifference, CallDiffTool, ShowShortStatus, ShowStatus, Commit, History, Separator, ExpandTreeItems, CollapseTreeItems  };
    mContextMenuEmptySourceTree = { AddGitSourceFolder, UpdateGitStatus, Separator, ExpandTreeItems, CollapseTreeItems};

    mContextMenuHistoryTree     = { CallHistoryDiffTool, ShowHistoryDifference, Separator, ShowHideTree, ClearTreeItems };
    mContextMenuBranchTree      = { BranchList, BranchListRemote, BranchListMerged, BranchListNotMerged, Separator, BranchShow, BranchCheckout, BranchDelete, Separator, ShowHideTree, ClearTreeItems };

    mToolbars.push_back({ Add, Unstage, Restore, MoveOrRename, Remove, Separator, ShowDifference, CallDiffTool, History, Separator, ShowStatus, ShowShortStatus, BranchList});
    mToolbars.push_back({ AddGitSourceFolder, UpdateGitStatus, Separator, ShowHideTree, ClearTreeItems, ExpandTreeItems, CollapseTreeItems, Separator, Commit, Push, Pull, CustomGitActionSettings});
}


QString Cmd::toString(const ePostAction anAction)
{
    switch (anAction)
    {
    case DoNothing:             return QObject::tr("Do Nothing");
    case UpdateItemStatus:      return QObject::tr("Update Item Status");
    case ParseHistoryText:      return QObject::tr("Parse History List Text");
    case ParseBranchListText:   return QObject::tr("Parse Branch List Text");
    default:
        break;
    }
    return "";
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

QString Type::getStates()
{
    const QString fSep = "|";
    QString fState = fSep;
    if (is(GitAdded   ))  fState += name(GitAdded)     + fSep;
    if (is(GitDeleted ))  fState += name(GitDeleted)   + fSep;
    if (is(GitModified))  fState += name(GitModified)  + fSep;
    if (is(GitUnTracked)) fState += name(GitUnTracked) + fSep;
    if (is(GitRenamed ))  fState += name(GitRenamed)   + fSep;
    if (is(GitStaged  ))  fState += name(GitStaged)    + fSep;
    if (is(GitUnmerged))  fState += name(GitUnmerged)  + fSep;
    if (is(GitLocal   ))  fState += name(GitLocal)     + fSep;
    if (is(GitRemote  ))  fState += name(GitRemote)    + fSep;
    if (is(GitBoth    ))  fState += name(GitBoth)      + fSep;
    return fState;
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

QString Type::type_name()
{
    return Type::name(static_cast<Type::TypeFlags>(Type::FileType&mType));
}

void Type::translate(const QString& fIdentifier)
{

    if (fIdentifier.contains('D'))  add(GitDeleted);
    if      (fIdentifier[1]=='M')   add(GitModified);
    else if (fIdentifier[0]=='M')   add(static_cast<TypeFlags>(GitModified|GitStaged));
    if      (fIdentifier[1]=='A')   add(GitAdded);
    else if (fIdentifier[0]=='A')   add(static_cast<TypeFlags>(GitAdded|GitStaged));
    if      (fIdentifier[1]=='R')   add(GitRenamed);
    else if (fIdentifier[0]=='R')   add(static_cast<TypeFlags>(GitRenamed|GitStaged));
    if      (fIdentifier=="DD")     add(static_cast<TypeFlags>(GitUnmerged|GitBoth));   // unmerged, both deleted
    else if (fIdentifier=="AU")     add(static_cast<TypeFlags>(GitUnmerged|GitLocal));  // unmerged, added by us
    else if (fIdentifier=="UD")     add(static_cast<TypeFlags>(GitUnmerged|GitRemote)); // unmerged, deleted by them
    else if (fIdentifier=="UA")     add(static_cast<TypeFlags>(GitUnmerged|GitRemote)); // unmerged, added by them
    else if (fIdentifier=="DU")     add(static_cast<TypeFlags>(GitUnmerged|GitLocal));  // unmerged, deleted by us
    else if (fIdentifier=="AA")     add(static_cast<TypeFlags>(GitUnmerged|GitBoth));   // unmerged, both added
    else if (fIdentifier=="UU")     add(static_cast<TypeFlags>(GitUnmerged|GitBoth));   // unmerged, both modified
    if (fIdentifier.contains("?"))  add(GitUnTracked);
    if (fIdentifier.contains("##")) add(Repository);

}

void Type::translate(const QFileInfo& fInfo)
{
    if (fInfo.isFile())       add(File);
    if (fInfo.isDir())        add(Folder);
    if (fInfo.isHidden())     add(Hidden);
    if (fInfo.isExecutable()) add(Executeable);
    if (fInfo.isSymLink())    add(SymLink);
}


}
